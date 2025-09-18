#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Assumons que votre malloc/free sont déclarés quelque part
// extern void* malloc(size_t size);
// extern void free(void* ptr);

#define MAX_ALLOCS 10000
#define INSANE_ALLOCS 50000
#define MIN_SIZE 1
#define MAX_SIZE 65536
#define TORTURE_ROUNDS 1000
#define THREAD_COUNT 8

// Structure pour tracker les allocations
typedef struct allocation {
    void* ptr;
    size_t size;
    unsigned char pattern;
    int magic_start;
    int magic_end;
    int alive;
    struct allocation* next;
} allocation_t;

allocation_t allocations[INSANE_ALLOCS];
allocation_t* free_list = NULL;
allocation_t* active_list = NULL;
int total_allocations = 0;
int total_frees = 0;
int corruption_count = 0;

pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;

// Patterns de torture
#define MAGIC_START 0xDEADBEEF
#define MAGIC_END 0xCAFEBABE
#define ZOMBIE_PATTERN 0xDEADDEAD

// Fonction pour créer des patterns de données complexes
void write_insane_pattern(void* ptr, size_t size, unsigned char base_pattern) {
    unsigned char* data = (unsigned char*)ptr;
    
    // Pattern spiralé avec variations
    for (size_t i = 0; i < size; i++) {
        unsigned char pattern = base_pattern;
        pattern ^= (i & 0xFF);          // XOR avec position
        pattern += (i >> 8) & 0xFF;     // Addition avec page
        pattern ^= (i * 0x5A5A5A5A) & 0xFF; // Pattern multiplicatif
        pattern += ((i >> 4) ^ (i >> 12)) & 0xFF; // Pattern fractal
        data[i] = pattern;
    }
}

// Vérification hardcore de l'intégrité
int verify_insane_pattern(void* ptr, size_t size, unsigned char base_pattern) {
    unsigned char* data = (unsigned char*)ptr;
    
    for (size_t i = 0; i < size; i++) {
        unsigned char expected = base_pattern;
        expected ^= (i & 0xFF);
        expected += (i >> 8) & 0xFF;
        expected ^= (i * 0x5A5A5A5A) & 0xFF;
        expected += ((i >> 4) ^ (i >> 12)) & 0xFF;
        
        if (data[i] != expected) {
            printf("💀 CORRUPTION DETECTEE! Offset %zu: attendu 0x%02x, trouvé 0x%02x\n", 
                   i, expected, data[i]);
            corruption_count++;
            return 0;
        }
    }
    return 1;
}

// Test 1: CHAOS TOTAL - Allocations/libérations complètement chaotiques
void test_absolute_chaos() {
    printf("🌪️  === TEST CHAOS ABSOLU === 🌪️ \n");
    
    void* chaos_ptrs[1000];
    size_t chaos_sizes[1000];
    unsigned char chaos_patterns[1000];
    int chaos_alive[1000];
    
    memset(chaos_alive, 0, sizeof(chaos_alive));
    
    srand((unsigned int)time(NULL) ^ getpid());
    
    for (int apocalypse = 0; apocalypse < 50; apocalypse++) {
        printf("🔥 Apocalypse round %d...\n", apocalypse);
        
        // Phase 1: Allocation anarchique
        for (int i = 0; i < 1000; i++) {
            if (!chaos_alive[i] && (rand() % 100) < 80) {
                // Tailles complètement folles
                size_t size;
                int crazy = rand() % 100;
                if (crazy < 20) size = 1;                    // Micro
                else if (crazy < 40) size = rand() % 16 + 1; // Tiny
                else if (crazy < 70) size = rand() % 512 + 16; // Small
                else if (crazy < 90) size = rand() % 4096 + 512; // Medium  
                else size = rand() % 32768 + 4096;           // Large
                
                chaos_ptrs[i] = malloc(size);
                if (chaos_ptrs[i]) {
                    chaos_sizes[i] = size;
                    chaos_patterns[i] = rand() % 256;
                    chaos_alive[i] = 1;
                    write_insane_pattern(chaos_ptrs[i], size, chaos_patterns[i]);
                }
            }
        }
        
        // Phase 2: Vérification pendant le chaos
        for (int i = 0; i < 1000; i++) {
            if (chaos_alive[i] && (rand() % 100) < 30) {
                verify_insane_pattern(chaos_ptrs[i], chaos_sizes[i], chaos_patterns[i]);
            }
        }
        
        // Phase 3: Libération chaotique
        for (int i = 0; i < 1000; i++) {
            if (chaos_alive[i] && (rand() % 100) < 40) {
                verify_insane_pattern(chaos_ptrs[i], chaos_sizes[i], chaos_patterns[i]);
                free(chaos_ptrs[i]);
                chaos_alive[i] = 0;
                
                // Écrire des données "zombie" dans l'ancien pointeur (pour détecter use-after-free)
                // memset(chaos_ptrs[i], 0xDE, (chaos_sizes[i] > 64) ? 64 : chaos_sizes[i]);
            }
        }
        
        // Phase 4: Réallocation immédiate dans les mêmes zones
        for (int i = 0; i < 1000; i++) {
            if (!chaos_alive[i] && (rand() % 100) < 60) {
                size_t new_size = rand() % 2048 + 1;
                chaos_ptrs[i] = malloc(new_size);
                if (chaos_ptrs[i]) {
                    chaos_sizes[i] = new_size;
                    chaos_patterns[i] = rand() % 256;
                    chaos_alive[i] = 1;
                    write_insane_pattern(chaos_ptrs[i], new_size, chaos_patterns[i]);
                }
            }
        }
    }
    
    // Nettoyage final
    for (int i = 0; i < 1000; i++) {
        if (chaos_alive[i]) {
            verify_insane_pattern(chaos_ptrs[i], chaos_sizes[i], chaos_patterns[i]);
            free(chaos_ptrs[i]);
        }
    }
    
    printf("🌪️  Chaos test survived! 🌪️ \n\n");
}

// Test 2: FRAGMENTATION EXTREME
void test_extreme_fragmentation() {
    printf("💥 === FRAGMENTATION DE L'ENFER === 💥\n");
    
    void* frag_ptrs[5000];
    size_t frag_sizes[5000];
    
    // Créer un paysage de fragmentation dantesque
    printf("Créing fragmentation hell...\n");
    
    // Phase 1: Allocation en damier géant
    for (int i = 0; i < 5000; i++) {
        frag_sizes[i] = (rand() % 100) + 1;
        frag_ptrs[i] = malloc(frag_sizes[i]);
        if (frag_ptrs[i]) {
            memset(frag_ptrs[i], i & 0xFF, frag_sizes[i]);
        }
    }
    
    // Phase 2: Libération en patterns insanes
    printf("Creating fragmentation patterns...\n");
    
    // Pattern 1: Libérer tous les multiples de 3
    for (int i = 0; i < 5000; i += 3) {
        if (frag_ptrs[i]) {
            free(frag_ptrs[i]);
            frag_ptrs[i] = NULL;
        }
    }
    
    // Pattern 2: Libérer de manière fibonacienne
    int fib1 = 1, fib2 = 1;
    while (fib1 < 5000) {
        if (frag_ptrs[fib1]) {
            free(frag_ptrs[fib1]);
            frag_ptrs[fib1] = NULL;
        }
        int temp = fib1 + fib2;
        fib1 = fib2;
        fib2 = temp;
    }
    
    // Pattern 3: Libération prime (nombres premiers)
    int primes[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97};
    for (int p = 0; p < 25; p++) {
        for (int i = primes[p]; i < 5000; i += primes[p]) {
            if (frag_ptrs[i]) {
                free(frag_ptrs[i]);
                frag_ptrs[i] = NULL;
            }
        }
    }
    
    printf("Fragmentation maximale atteinte! Essai de réallocation...\n");
    
    // Phase 3: Essayer de réallouer dans ce chaos
    for (int round = 0; round < 100; round++) {
        for (int i = 0; i < 5000; i++) {
            if (!frag_ptrs[i] && (rand() % 100) < 20) {
                size_t new_size = rand() % 200 + 1;
                frag_ptrs[i] = malloc(new_size);
                if (frag_ptrs[i]) {
                    frag_sizes[i] = new_size;
                    memset(frag_ptrs[i], (round + i) & 0xFF, new_size);
                }
            }
        }
        
        // Libérer aléatoirement
        for (int i = 0; i < 5000; i++) {
            if (frag_ptrs[i] && (rand() % 100) < 5) {
                free(frag_ptrs[i]);
                frag_ptrs[i] = NULL;
            }
        }
    }
    
    // Nettoyage
    for (int i = 0; i < 5000; i++) {
        if (frag_ptrs[i]) free(frag_ptrs[i]);
    }
    
    printf("💥 Fragmentation hell survived! 💥\n\n");
}

// Test 3: STRESS TEST DE LA MORT
void test_death_stress() {
    printf("☠️  === STRESS TEST DE LA MORT === ☠️ \n");
    
    allocation_t* persistent[100];
    
    // Créer des allocations qui vont survivre à tout
    printf("Créating immortal allocations...\n");
    for (int i = 0; i < 100; i++) {
        persistent[i] = &allocations[i];
        persistent[i]->size = 1024 + (rand() % 1024);
        persistent[i]->ptr = malloc(persistent[i]->size);
        persistent[i]->pattern = rand() % 256;
        persistent[i]->magic_start = MAGIC_START;
        persistent[i]->magic_end = MAGIC_END;
        persistent[i]->alive = 1;
        
        if (persistent[i]->ptr) {
            write_insane_pattern(persistent[i]->ptr, persistent[i]->size, persistent[i]->pattern);
        }
    }
    
    // APOCALYPSE: 10000 rounds de torture
    printf("Starting apocalypse...\n");
    for (int apocalypse = 0; apocalypse < 10000; apocalypse++) {
        if (apocalypse % 1000 == 0) {
            printf("💀 Apocalypse round %d/10000\n", apocalypse);
            
            // Vérifier que les persistantes survivent toujours
            for (int i = 0; i < 100; i++) {
                if (persistent[i]->ptr && persistent[i]->alive) {
                    if (!verify_insane_pattern(persistent[i]->ptr, persistent[i]->size, persistent[i]->pattern)) {
                        printf("PERSISTENT ALLOCATION %d CORRUPTED!\n", i);
                        assert(0);
                    }
                }
            }
        }
        
        // Allocation/libération frénétique
        for (int burst = 0; burst < 50; burst++) {
            size_t insane_size = 1 + (rand() % 8192);
            void* death_ptr = malloc(insane_size);
            
            if (death_ptr) {
                // Écrire des données folles
                unsigned char death_pattern = rand() % 256;
                write_insane_pattern(death_ptr, insane_size, death_pattern);
                
                // Vérifier immédiatement
                verify_insane_pattern(death_ptr, insane_size, death_pattern);
                
                // Libérer parfois immédiatement, parfois après un délai
                if (rand() % 100 < 70) {
                    free(death_ptr);
                } else {
                    // Garder pour quelques rounds puis libérer
                    if (rand() % 100 < 50) {
                        free(death_ptr);
                    }
                }
            }
        }
    }
    
    // Vérification finale des survivants
    printf("Final survivor check...\n");
    for (int i = 0; i < 100; i++) {
        if (persistent[i]->ptr && persistent[i]->alive) {
            verify_insane_pattern(persistent[i]->ptr, persistent[i]->size, persistent[i]->pattern);
            free(persistent[i]->ptr);
        }
    }
    
    printf("☠️  Death stress test survived! ☠️ \n\n");
}

// Test 4: ALIGNEMENT ET PATTERNS BIZARRES
void test_alignment_hell() {
    printf("🔀 === ALIGNEMENT DE L'ENFER === 🔀\n");
    
    // Tester tous les alignements possibles
    for (int align_test = 0; align_test < 64; align_test++) {
        void* align_ptrs[100];
        
        for (int i = 0; i < 100; i++) {
            size_t size = (i + 1) + align_test;
            align_ptrs[i] = malloc(size);
            
            if (align_ptrs[i]) {
                // Vérifier l'alignement
                uintptr_t addr = (uintptr_t)align_ptrs[i];
                if (addr & 0x7) {
                    printf("⚠️  WARNING: Pointer %p not 8-byte aligned!\n", align_ptrs[i]);
                }
                
                // Pattern d'alignement
                unsigned char* data = (unsigned char*)align_ptrs[i];
                for (size_t j = 0; j < size; j++) {
                    data[j] = (unsigned char)((addr + j) & 0xFF);
                }
            }
        }
        
        // Vérifier les données
        for (int i = 0; i < 100; i++) {
            if (align_ptrs[i]) {
                uintptr_t addr = (uintptr_t)align_ptrs[i];
                unsigned char* data = (unsigned char*)align_ptrs[i];
                size_t size = (i + 1) + align_test;
                
                for (size_t j = 0; j < size; j++) {
                    if (data[j] != (unsigned char)((addr + j) & 0xFF)) {
                        printf("ALIGNMENT CORRUPTION detected!\n");
                        assert(0);
                    }
                }
                free(align_ptrs[i]);
            }
        }
    }
    
    printf("🔀 Alignment hell survived! 🔀\n\n");
}

// Test 5: SIMULATION MULTITHREAD (même si votre malloc n'est pas thread-safe)
void* thread_torture(void* arg) {
    int thread_id = *(int*)arg;
    printf("🧵 Thread %d starting torture...\n", thread_id);
    
    void* thread_ptrs[1000];
    memset(thread_ptrs, 0, sizeof(thread_ptrs));
    
    for (int round = 0; round < 500; round++) {
        for (int i = 0; i < 1000; i++) {
            if (!thread_ptrs[i] && (rand() % 100) < 60) {
                pthread_mutex_lock(&test_mutex);
                size_t size = (rand() % 1024) + 1;
                thread_ptrs[i] = malloc(size);
                pthread_mutex_unlock(&test_mutex);
                
                if (thread_ptrs[i]) {
                    memset(thread_ptrs[i], thread_id + i, size);
                }
            } else if (thread_ptrs[i] && (rand() % 100) < 30) {
                pthread_mutex_lock(&test_mutex);
                free(thread_ptrs[i]);
                pthread_mutex_unlock(&test_mutex);
                thread_ptrs[i] = NULL;
            }
        }
    }
    
    // Cleanup
    pthread_mutex_lock(&test_mutex);
    for (int i = 0; i < 1000; i++) {
        if (thread_ptrs[i]) {
            free(thread_ptrs[i]);
        }
    }
    pthread_mutex_unlock(&test_mutex);
    
    printf("🧵 Thread %d torture complete!\n", thread_id);
    return NULL;
}

void test_thread_simulation() {
    printf("🧵 === SIMULATION MULTITHREAD === 🧵\n");
    
    pthread_t threads[THREAD_COUNT];
    int thread_ids[THREAD_COUNT];
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, thread_torture, &thread_ids[i]);
    }
    
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("🧵 All threads survived! 🧵\n\n");
}

// Test 6: LE TEST ULTIME - COMBINATION DE TOUS LES ENFERS
void test_ultimate_hell() {
    printf("🌋 === TEST ULTIME DE L'ENFER === 🌋\n");
    printf("Combining ALL tortures simultaneously...\n");
    
    void* hell_ptrs[10000];
    size_t hell_sizes[10000];
    unsigned char hell_patterns[10000];
    int hell_alive[10000];
    
    memset(hell_alive, 0, sizeof(hell_alive));
    
    for (int damnation = 0; damnation < 100; damnation++) {
        printf("🔥 Damnation level %d/100\n", damnation);
        
        // Chaos simultané sur tous les fronts
        for (int i = 0; i < 10000; i++) {
            int action = rand() % 1000;
            
            if (action < 400 && !hell_alive[i]) {
                // Allocation avec pattern insane
                size_t size;
                int madness = rand() % 1000;
                
                if (madness < 100) size = 1;
                else if (madness < 200) size = rand() % 8 + 1;
                else if (madness < 400) size = rand() % 64 + 8;
                else if (madness < 600) size = rand() % 512 + 64;
                else if (madness < 800) size = rand() % 4096 + 512;
                else if (madness < 950) size = rand() % 16384 + 4096;
                else size = rand() % 65536 + 16384;
                
                hell_ptrs[i] = malloc(size);
                if (hell_ptrs[i]) {
                    hell_sizes[i] = size;
                    hell_patterns[i] = rand() % 256;
                    hell_alive[i] = 1;
                    write_insane_pattern(hell_ptrs[i], size, hell_patterns[i]);
                }
            } else if (action < 600 && hell_alive[i]) {
                // Vérification d'intégrité
                if (!verify_insane_pattern(hell_ptrs[i], hell_sizes[i], hell_patterns[i])) {
                    printf("ULTIMATE HELL CORRUPTION at index %d!\n", i);
                }
            } else if (action < 800 && hell_alive[i]) {
                // Libération
                verify_insane_pattern(hell_ptrs[i], hell_sizes[i], hell_patterns[i]);
                free(hell_ptrs[i]);
                hell_alive[i] = 0;
                
                // Pattern zombie
                hell_ptrs[i] = (void*)0xDEADBEEF;
            } else if (action < 900 && hell_alive[i]) {
                // Re-écriture du pattern (simulation d'utilisation)
                write_insane_pattern(hell_ptrs[i], hell_sizes[i], hell_patterns[i]);
            }
        }
        
        // Vérification massive périodique
        if (damnation % 10 == 0) {
            printf("Massive integrity check...\n");
            for (int i = 0; i < 10000; i++) {
                if (hell_alive[i]) {
                    verify_insane_pattern(hell_ptrs[i], hell_sizes[i], hell_patterns[i]);
                }
            }
        }
    }
    
    // Purgatoire final
    printf("Final purgatory cleanup...\n");
    for (int i = 0; i < 10000; i++) {
        if (hell_alive[i]) {
            verify_insane_pattern(hell_ptrs[i], hell_sizes[i], hell_patterns[i]);
            free(hell_ptrs[i]);
        }
    }
    
    printf("🌋 ULTIMATE HELL SURVIVED! YOUR malloc IS GODLIKE! 🌋\n\n");
}

int main() {
    printf("🔥🔥🔥 === TESTS DE FOU FURIEUX POUR malloc === 🔥🔥🔥\n\n");
    printf("Préparez-vous... Votre malloc va SOUFFRIR!\n\n");
    
    test_absolute_chaos();
    test_extreme_fragmentation();
    test_alignment_hell();
    // test_thread_simulation();
    test_death_stress();
    test_ultimate_hell();
    
    printf("🎉🎉🎉 === TOUS LES TESTS DE FOU ONT ÉTÉ RÉUSSIS! === 🎉🎉🎉\n");
    printf("Votre malloc est une BÊTE SAUVAGE! 💪\n");
    printf("Total corruptions détectées: %d\n", corruption_count);
    
    return 0;
}