#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdatomic.h>

// Assumons que votre malloc/free sont déclarés quelque part
// extern void* my_malloc(size_t size);
// extern void free(void* ptr);

#define MAX_THREADS 16
#define ALLOCS_PER_THREAD 2000
#define TORTURE_MINUTES 1
#define INTEGRITY_CHECK_INTERVAL_MS 100
#define GLOBAL_INTEGRITY_CHECK_INTERVAL_S 2
#define MAX_ALLOC_SIZE 16384
#define MIN_ALLOC_SIZE 8

// Structure pour tracker TOUTES les allocations
typedef struct allocation_node {
    void* ptr;
    size_t size;
    unsigned char pattern;
    int thread_id;
    int alloc_id;
    time_t timestamp;
    struct allocation_node* next;
    atomic_int alive;
    pthread_mutex_t mutex;
} allocation_node_t;

// Pool global d'allocations trackées
#define GLOBAL_ALLOC_POOL_SIZE 50000
allocation_node_t global_alloc_pool[GLOBAL_ALLOC_POOL_SIZE];
allocation_node_t* global_active_list = NULL;
pthread_mutex_t global_list_mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int global_alloc_count = 0;
atomic_int global_free_count = 0;
atomic_int global_corruption_count = 0;
atomic_int global_integrity_checks = 0;
atomic_int test_running = 1;
atomic_int pool_index = 0;

// Statistiques par thread
typedef struct {
    int thread_id;
    atomic_int local_allocs;
    atomic_int local_frees;
    atomic_int local_corruptions;
    atomic_int local_integrity_checks;
} thread_stats_t;

thread_stats_t thread_stats[MAX_THREADS];

// Fonction pour obtenir le temps en millisecondes
long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

// Pattern de données ultra-complexe pour détecter la moindre corruption
void write_ultimate_pattern(void* ptr, size_t size, unsigned char base_pattern, int thread_id, int alloc_id) {
    if (!ptr || size == 0) return;
    
    unsigned char* data = (unsigned char*)ptr;
    
    for (size_t i = 0; i < size; i++) {
        unsigned char pattern = base_pattern;
        
        // Pattern multicouche ultra-complexe
        pattern ^= (unsigned char)(i & 0xFF);                    // Position XOR
        pattern += (unsigned char)((i >> 8) & 0xFF);             // Page offset
        pattern ^= (unsigned char)(thread_id * 0x37);            // Thread signature
        pattern += (unsigned char)((alloc_id * 0x5A5A) & 0xFF); // Alloc signature
        pattern ^= (unsigned char)((i * 0xDEADBEEF) & 0xFF);    // Multiplicative chaos
        pattern += (unsigned char)((i >> 4) ^ (i >> 12)) & 0xFF; // Fractal pattern
        pattern ^= (unsigned char)(size & 0xFF);                 // Size signature
        pattern += (unsigned char)((i ^ (i >> 16)) & 0xFF);     // High-order bits
        
        data[i] = pattern;
    }
}

// Vérification ultra-stricte
int verify_ultimate_pattern(void* ptr, size_t size, unsigned char base_pattern, int thread_id, int alloc_id) {
    if (!ptr || size == 0) return 0;
    
    unsigned char* data = (unsigned char*)ptr;
    
    for (size_t i = 0; i < size; i++) {
        unsigned char expected = base_pattern;
        
        // Recalculer le pattern exact
        expected ^= (unsigned char)(i & 0xFF);
        expected += (unsigned char)((i >> 8) & 0xFF);
        expected ^= (unsigned char)(thread_id * 0x37);
        expected += (unsigned char)((alloc_id * 0x5A5A) & 0xFF);
        expected ^= (unsigned char)((i * 0xDEADBEEF) & 0xFF);
        expected += (unsigned char)((i >> 4) ^ (i >> 12)) & 0xFF;
        expected ^= (unsigned char)(size & 0xFF);
        expected += (unsigned char)((i ^ (i >> 16)) & 0xFF);
        
        if (data[i] != expected) {
            printf("💀 CORRUPTION THREAD %d ALLOC %d: offset %zu, attendu 0x%02x, trouvé 0x%02x\n", 
                   thread_id, alloc_id, i, expected, data[i]);
            atomic_fetch_add(&global_corruption_count, 1);
            return 0;
        }
    }
    return 1;
}

// Ajouter une allocation à la liste globale
allocation_node_t* add_to_global_list(void* ptr, size_t size, unsigned char pattern, int thread_id, int alloc_id) {
    int idx = atomic_fetch_add(&pool_index, 1) % GLOBAL_ALLOC_POOL_SIZE;
    allocation_node_t* node = &global_alloc_pool[idx];
    
    // Attendre que le slot soit disponible
    while (atomic_load(&node->alive)) {
        usleep(1);
        idx = atomic_fetch_add(&pool_index, 1) % GLOBAL_ALLOC_POOL_SIZE;
        node = &global_alloc_pool[idx];
    }
    
    pthread_mutex_init(&node->mutex, NULL);
    node->ptr = ptr;
    node->size = size;
    node->pattern = pattern;
    node->thread_id = thread_id;
    node->alloc_id = alloc_id;
    node->timestamp = time(NULL);
    atomic_store(&node->alive, 1);
    
    pthread_mutex_lock(&global_list_mutex);
    node->next = global_active_list;
    global_active_list = node;
    pthread_mutex_unlock(&global_list_mutex);
    
    return node;
}

// Retirer une allocation de la liste globale
void remove_from_global_list(allocation_node_t* node) {
    if (!node) return;
    
    pthread_mutex_lock(&node->mutex);
    atomic_store(&node->alive, 0);
    pthread_mutex_unlock(&node->mutex);
    pthread_mutex_destroy(&node->mutex);
}

// Thread dédié aux vérifications d'intégrité globales
void* integrity_checker_thread(void* arg) {
    printf("🛡️  INTEGRITY CHECKER THREAD STARTED\n");
    
    long long start_time = get_time_ms();
    int check_count = 0;
    
    while (atomic_load(&test_running)) {
        check_count++;
        printf("🔍 INTEGRITY CHECK GLOBAL #%d (%.1fs elapsed)\n", 
               check_count, (get_time_ms() - start_time) / 1000.0);
        
        int checked = 0, corruptions = 0;
        
        pthread_mutex_lock(&global_list_mutex);
        allocation_node_t* current = global_active_list;
        
        while (current && checked < 10000) { // Limiter pour éviter les boucles infinies
            if (atomic_load(&current->alive)) {
                pthread_mutex_lock(&current->mutex);
                
                if (atomic_load(&current->alive) && current->ptr) {
                    if (!verify_ultimate_pattern(current->ptr, current->size, current->pattern, 
                                               current->thread_id, current->alloc_id)) {
                        corruptions++;
                        printf("💀 CORRUPTION DETECTEE: Thread %d, Alloc %d, Ptr %p, Size %zu\n",
                               current->thread_id, current->alloc_id, current->ptr, current->size);
                    }
                    checked++;
                }
                
                pthread_mutex_unlock(&current->mutex);
            }
            current = current->next;
        }
        
        pthread_mutex_unlock(&global_list_mutex);
        
        atomic_fetch_add(&global_integrity_checks, checked);
        
        printf("✅ Checked %d allocations, %d corruptions found\n", checked, corruptions);
        printf("📊 STATS: Allocs=%d, frees=%d, Corruptions=%d, IntegrityChecks=%d\n",
               atomic_load(&global_alloc_count), atomic_load(&global_free_count),
               atomic_load(&global_corruption_count), atomic_load(&global_integrity_checks));
        
        sleep(GLOBAL_INTEGRITY_CHECK_INTERVAL_S);
    }
    
    printf("🛡️  INTEGRITY CHECKER THREAD FINISHED\n");
    return NULL;
}

// Thread de stats en temps réel
void* stats_thread(void* arg) {
    printf("📊 STATS THREAD STARTED\n");
    
    long long start_time = get_time_ms();
    
    while (atomic_load(&test_running)) {
        sleep(5);
        
        double elapsed = (get_time_ms() - start_time) / 1000.0;
        printf("\n📈 === STATS TEMPS RÉEL (%.1fs) ===\n", elapsed);
        printf("🔢 Allocations globales: %d\n", atomic_load(&global_alloc_count));
        printf("🗑️  Libérations globales: %d\n", atomic_load(&global_free_count));
        printf("💀 Corruptions détectées: %d\n", atomic_load(&global_corruption_count));
        printf("🔍 Vérifications d'intégrité: %d\n", atomic_load(&global_integrity_checks));
        
        printf("📋 Stats par thread:\n");
        for (int i = 0; i < MAX_THREADS; i++) {
            printf("  Thread %2d: Allocs=%d, frees=%d, Corruptions=%d, Checks=%d\n",
                   i, atomic_load(&thread_stats[i].local_allocs),
                   atomic_load(&thread_stats[i].local_frees),
                   atomic_load(&thread_stats[i].local_corruptions),
                   atomic_load(&thread_stats[i].local_integrity_checks));
        }
        printf("=====================================\n\n");
    }
    
    return NULL;
}

// Thread de torture malloc/my_free
void* death_torture_thread(void* arg) {
    int thread_id = *(int*)arg;
    thread_stats[thread_id].thread_id = thread_id;
    
    printf("🧵 DEATH THREAD %d STARTING ABSOLUTE TORTURE\n", thread_id);
    
    allocation_node_t* my_allocations[ALLOCS_PER_THREAD];
    memset(my_allocations, 0, sizeof(my_allocations));
    
    srand((unsigned int)time(NULL) ^ thread_id ^ getpid());
    
    long long start_time = get_time_ms();
    long long end_time = start_time + (TORTURE_MINUTES * 60 * 1000);
    
    int round = 0;
    
    while (get_time_ms() < end_time && atomic_load(&test_running)) {
        round++;
        
        if (round % 100 == 0) {
            printf("🔥 Thread %d: Round %d (%.1fs remaining)\n", 
                   thread_id, round, (end_time - get_time_ms()) / 1000.0);
        }
        
        // Phase 1: Allocations frénétiques
        for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
            if (!my_allocations[i] && (rand() % 100) < 70) {
                size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
                void* ptr = malloc(size);
                
                if (ptr) {
                    unsigned char pattern = (unsigned char)(rand() % 256);
                    int alloc_id = round * 10000 + i;
                    
                    write_ultimate_pattern(ptr, size, pattern, thread_id, alloc_id);
                    
                    // Vérification immédiate post-allocation
                    if (!verify_ultimate_pattern(ptr, size, pattern, thread_id, alloc_id)) {
                        printf("💀 CORRUPTION IMMEDIATE après malloc! Thread %d\n", thread_id);
                        atomic_fetch_add(&thread_stats[thread_id].local_corruptions, 1);
                    }
                    
                    my_allocations[i] = add_to_global_list(ptr, size, pattern, thread_id, alloc_id);
                    atomic_fetch_add(&global_alloc_count, 1);
                    atomic_fetch_add(&thread_stats[thread_id].local_allocs, 1);
                }
            }
        }
        
        // Phase 2: Vérification d'intégrité intensive de MES allocations
        if (round % 10 == 0) {
            for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
                if (my_allocations[i] && atomic_load(&my_allocations[i]->alive)) {
                    pthread_mutex_lock(&my_allocations[i]->mutex);
                    
                    if (atomic_load(&my_allocations[i]->alive) && my_allocations[i]->ptr) {
                        if (!verify_ultimate_pattern(my_allocations[i]->ptr, my_allocations[i]->size,
                                                   my_allocations[i]->pattern, thread_id, my_allocations[i]->alloc_id)) {
                            atomic_fetch_add(&thread_stats[thread_id].local_corruptions, 1);
                        }
                        atomic_fetch_add(&thread_stats[thread_id].local_integrity_checks, 1);
                    }
                    
                    pthread_mutex_unlock(&my_allocations[i]->mutex);
                }
            }
        }
        
        // Phase 3: Libérations avec vérification pré-free
        for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
            if (my_allocations[i] && atomic_load(&my_allocations[i]->alive) && (rand() % 100) < 30) {
                pthread_mutex_lock(&my_allocations[i]->mutex);
                
                if (atomic_load(&my_allocations[i]->alive) && my_allocations[i]->ptr) {
                    // Vérification OBLIGATOIRE avant free
                    if (!verify_ultimate_pattern(my_allocations[i]->ptr, my_allocations[i]->size,
                                               my_allocations[i]->pattern, thread_id, my_allocations[i]->alloc_id)) {
                        printf("💀 CORRUPTION avant free()! Thread %d, Alloc %d\n", thread_id, my_allocations[i]->alloc_id);
                        atomic_fetch_add(&thread_stats[thread_id].local_corruptions, 1);
                    }
                    
                    free(my_allocations[i]->ptr);
                    atomic_fetch_add(&global_free_count, 1);
                    atomic_fetch_add(&thread_stats[thread_id].local_frees, 1);
                }
                
                pthread_mutex_unlock(&my_allocations[i]->mutex);
                remove_from_global_list(my_allocations[i]);
                my_allocations[i] = NULL;
            }
        }
        
        // Phase 4: Micro-pause pour laisser les autres threads agir
        if (rand() % 1000 == 0) {
            usleep(INTEGRITY_CHECK_INTERVAL_MS * 1000);
        }
    }
    
    printf("🧹 Thread %d: Nettoyage final...\n", thread_id);
    
    // Nettoyage final avec vérifications
    for (int i = 0; i < ALLOCS_PER_THREAD; i++) {
        if (my_allocations[i] && atomic_load(&my_allocations[i]->alive)) {
            pthread_mutex_lock(&my_allocations[i]->mutex);
            
            if (atomic_load(&my_allocations[i]->alive) && my_allocations[i]->ptr) {
                verify_ultimate_pattern(my_allocations[i]->ptr, my_allocations[i]->size,
                                      my_allocations[i]->pattern, thread_id, my_allocations[i]->alloc_id);
                free(my_allocations[i]->ptr);
                atomic_fetch_add(&global_free_count, 1);
                atomic_fetch_add(&thread_stats[thread_id].local_frees, 1);
            }
            
            pthread_mutex_unlock(&my_allocations[i]->mutex);
            remove_from_global_list(my_allocations[i]);
        }
    }
    
    printf("☠️  THREAD %d DEATH TORTURE COMPLETE!\n", thread_id);
    return NULL;
}

int main() {
    printf("🔥🔥🔥 === TEST MULTITHREAD DE LA MORT ABSOLUE === 🔥🔥🔥\n");
    printf("⏰ Durée: %d minutes de TORTURE PURE\n", TORTURE_MINUTES);
    printf("🧵 Threads: %d threads de mort + checker d'intégrité + stats\n", MAX_THREADS);
    printf("💾 Allocations par thread: %d (total théorique: %d)\n", ALLOCS_PER_THREAD, MAX_THREADS * ALLOCS_PER_THREAD);
    printf("🔍 Vérifications d'intégrité: CONSTANTES et IMPITOYABLES\n\n");
    
    printf("🚨 ATTENTION: Ce test va MASSACRER votre malloc pendant %d minutes!\n", TORTURE_MINUTES);
    printf("Appuyez sur Ctrl+C pour arrêter si nécessaire...\n\n");
    sleep(3);
    
    // Initialiser les statistiques
    memset(thread_stats, 0, sizeof(thread_stats));
    memset(global_alloc_pool, 0, sizeof(global_alloc_pool));
    
    pthread_t threads[MAX_THREADS + 2]; // +2 pour integrity checker et stats
    int thread_ids[MAX_THREADS];
    
    long long start_time = get_time_ms();
    
    // Lancer le thread de vérification d'intégrité globale
    pthread_create(&threads[MAX_THREADS], NULL, integrity_checker_thread, NULL);
    
    // Lancer le thread de stats
    pthread_create(&threads[MAX_THREADS + 1], NULL, stats_thread, NULL);
    
    // Lancer tous les threads de torture
    printf("🚀 LANCEMENT DE L'APOCALYPSE malloc!\n\n");
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, death_torture_thread, &thread_ids[i]);
    }
    
    // Attendre tous les threads de torture
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Arrêter les threads de service
    atomic_store(&test_running, 0);
    pthread_join(threads[MAX_THREADS], NULL);
    pthread_join(threads[MAX_THREADS + 1], NULL);
    
    long long total_time = get_time_ms() - start_time;
    
    printf("\n🎉🎉🎉 === TEST DE LA MORT TERMINÉ === 🎉🎉🎉\n");
    printf("⏱️  Durée totale: %.2f secondes\n", total_time / 1000.0);
    printf("📊 STATISTIQUES FINALES:\n");
    printf("  🔢 Total allocations: %d\n", atomic_load(&global_alloc_count));
    printf("  🗑️  Total libérations: %d\n", atomic_load(&global_free_count));
    printf("  💀 Total corruptions: %d\n", atomic_load(&global_corruption_count));
    printf("  🔍 Total vérifications: %d\n", atomic_load(&global_integrity_checks));
    printf("  🏃‍♂️ Débit moyen: %.1f allocs/sec\n", atomic_load(&global_alloc_count) / (total_time / 1000.0));
    
    if (atomic_load(&global_corruption_count) == 0) {
        printf("\n🏆🏆🏆 VOTRE malloc EST INVINCIBLE! AUCUNE CORRUPTION DÉTECTÉE! 🏆🏆🏆\n");
    } else {
        printf("\n⚠️  %d corruptions détectées - vérifiez votre implémentation!\n", atomic_load(&global_corruption_count));
    }
    
    return 0;
}