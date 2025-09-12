#include "../includes/lib_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

// Configuration des tests
#define NUM_THREADS 8
#define OPERATIONS_PER_THREAD 1000
#define MAX_ALLOC_SIZE 4096
#define MIN_ALLOC_SIZE 8

// Structure pour passer les paramètres aux threads
typedef struct {
    int thread_id;
    int operations_count;
    int allocations_made;
    int deallocations_made;
    int errors;
    double execution_time;
} thread_data_t;

// Variables globales pour les statistiques
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
int total_allocations = 0;
int total_deallocations = 0;
int total_errors = 0;

// Fonction utilitaire pour obtenir le temps en microsecondes
double get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// Test 1: Allocations/libérations séquentielles
void* test_sequential_alloc_free(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test séquentiel\n", data->thread_id);
    
    for (int i = 0; i < data->operations_count; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        
        // Allocation
        void* ptr = malloc(size);
        if (!ptr) {
            printf("Thread %d: Erreur allocation %zu bytes\n", data->thread_id, size);
            data->errors++;
            continue;
        }
        
        data->allocations_made++;
        
        // Écriture pour vérifier l'accès mémoire
        memset(ptr, 0xAA, size);
        
        // Vérification
        if (((char*)ptr)[0] != (char)0xAA || ((char*)ptr)[size-1] != (char)0xAA) {
            printf("Thread %d: Erreur corruption mémoire\n", data->thread_id);
            data->errors++;
        }
        
        // Libération immédiate
        free(ptr);
        data->deallocations_made++;
        
        // Petite pause occasionnelle
        if (i % 100 == 0) {
            usleep(1);
        }
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_deallocations += data->deallocations_made;
    total_errors += data->errors;
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Terminé - %d alloc, %d free, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->deallocations_made, data->errors);
    
    return NULL;
}

// Test 2: Accumulation puis libération en bloc
void* test_batch_alloc_free(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    void** ptrs = malloc(data->operations_count * sizeof(void*));
    
    printf("Thread %d: Démarrage test par batch\n", data->thread_id);
    
    // Phase d'allocation
    for (int i = 0; i < data->operations_count; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        ptrs[i] = malloc(size);
        
        if (!ptrs[i]) {
            printf("Thread %d: Erreur allocation %zu bytes (op %d)\n", data->thread_id, size, i);
            data->errors++;
            continue;
        }
        
        data->allocations_made++;
        memset(ptrs[i], 0xBB, size);
    }
    
    // Petite pause entre allocation et libération
    usleep(1000);
    
    // Phase de libération
    for (int i = 0; i < data->operations_count; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
            data->deallocations_made++;
        }
    }
    
    free(ptrs);
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_deallocations += data->deallocations_made;
    total_errors += data->errors;
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Batch terminé - %d alloc, %d free, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->deallocations_made, data->errors);
    
    return NULL;
}

// Test 3: Allocations/libérations aléatoires
void* test_random_alloc_free(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    void** ptrs = calloc(data->operations_count, sizeof(void*));
    int active_ptrs = 0;
    
    printf("Thread %d: Démarrage test aléatoire\n", data->thread_id);
    
    for (int i = 0; i < data->operations_count * 2; i++) {
        if (active_ptrs == 0 || (active_ptrs < data->operations_count && rand() % 2)) {
            // Allocation
            size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
            int slot = -1;
            
            // Trouver un slot libre
            for (int j = 0; j < data->operations_count; j++) {
                if (!ptrs[j]) {
                    slot = j;
                    break;
                }
            }
            
            if (slot != -1) {
                ptrs[slot] = malloc(size);
                if (ptrs[slot]) {
                    data->allocations_made++;
                    active_ptrs++;
                    memset(ptrs[slot], 0xCC, size);
                } else {
                    data->errors++;
                }
            }
        } else {
            // Libération
            int slot = rand() % data->operations_count;
            if (ptrs[slot]) {
                free(ptrs[slot]);
                ptrs[slot] = NULL;
                data->deallocations_made++;
                active_ptrs--;
            }
        }
        
        if (i % 200 == 0) {
            usleep(1);
        }
    }
    
    // Nettoyer les allocations restantes
    for (int i = 0; i < data->operations_count; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
            data->deallocations_made++;
        }
    }
    
    free(ptrs);
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_deallocations += data->deallocations_made;
    total_errors += data->errors;
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Aléatoire terminé - %d alloc, %d free, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->deallocations_made, data->errors);
    
    return NULL;
}

// Test de stress avec différentes tailles
void* test_stress_different_sizes(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test de stress\n", data->thread_id);
    
    size_t sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < data->operations_count; i++) {
        size_t size = sizes[i % num_sizes];
        
        void* ptr = malloc(size);
        if (!ptr) {
            data->errors++;
            continue;
        }
        
        data->allocations_made++;
        
        // Test d'écriture/lecture
        memset(ptr, (i % 256), size);
        for (size_t j = 0; j < size; j++) {
            if (((char*)ptr)[j] != (char)(i % 256)) {
                printf("Thread %d: Corruption détectée à l'offset %zu\n", data->thread_id, j);
                data->errors++;
                break;
            }
        }
        
        free(ptr);
        data->deallocations_made++;
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_deallocations += data->deallocations_made;
    total_errors += data->errors;
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Stress terminé - %d alloc, %d free, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->deallocations_made, data->errors);
    
    return NULL;
}

int main() {
    printf("=== Test Multi-threadé pour malloc/free personnalisé ===\n\n");
    
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    // Initialiser le générateur aléatoire
    srand(time(NULL));
    
    // Test 1: Allocations/libérations séquentielles
    printf(">>> Test 1: Allocations/libérations séquentielles (%d threads)\n", NUM_THREADS/2);
    total_allocations = total_deallocations = total_errors = 0;
    
    for (int i = 0; i < NUM_THREADS/2; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].operations_count = OPERATIONS_PER_THREAD;
        thread_data[i].allocations_made = 0;
        thread_data[i].deallocations_made = 0;
        thread_data[i].errors = 0;
        
        pthread_create(&threads[i], NULL, test_sequential_alloc_free, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS/2; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Test 1 Résultats: %d allocations, %d libérations, %d erreurs\n\n", 
           total_allocations, total_deallocations, total_errors);
    
    // Test 2: Mix de différents patterns
    printf(">>> Test 2: Mix de patterns différents (%d threads)\n", NUM_THREADS);
    total_allocations = total_deallocations = total_errors = 0;
    
    void* (*test_functions[])(void*) = {
        test_sequential_alloc_free,
        test_batch_alloc_free,
        test_random_alloc_free,
        test_stress_different_sizes
    };
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].operations_count = OPERATIONS_PER_THREAD / 2; // Réduire pour ce test
        thread_data[i].allocations_made = 0;
        thread_data[i].deallocations_made = 0;
        thread_data[i].errors = 0;
        
        // Choisir un pattern de test différent pour chaque thread
        void* (*test_func)(void*) = test_functions[i % 4];
        pthread_create(&threads[i], NULL, test_func, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printf("Test 2 Résultats: %d allocations, %d libérations, %d erreurs\n\n", 
           total_allocations, total_deallocations, total_errors);
    
    // Afficher les statistiques détaillées
    printf("=== Statistiques détaillées ===\n");
    double total_time = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d: %.2f ms, %d ops/sec\n", 
               thread_data[i].thread_id, 
               thread_data[i].execution_time / 1000.0,
               (int)((thread_data[i].allocations_made + thread_data[i].deallocations_made) * 1000000.0 / thread_data[i].execution_time));
        total_time += thread_data[i].execution_time;
    }
    
    printf("Temps total cumulé: %.2f ms\n", total_time / 1000.0);
    printf("Erreurs totales: %d\n", total_errors);
    
    if (total_errors == 0) {
        printf("\n✅ TOUS LES TESTS SONT PASSÉS!\n");
        return 0;
    } else {
        printf("\n❌ %d ERREURS DÉTECTÉES\n", total_errors);
        return 1;
    }
}