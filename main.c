#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

// Assumons que votre malloc/free sont déclarés quelque part
// extern void* malloc(size_t size);
// extern void free(void* ptr);

#define MAX_ALLOCS 100000
#define MIN_SIZE 1
#define MAX_SIZE 100000

// Structure pour tracker les allocations
typedef struct {
    void* ptr;
    size_t size;
    int pattern; // Pattern de données écrit
    int active;  // 1 si l'allocation est active
} alloc_info_t;

alloc_info_t allocations[MAX_ALLOCS];
int num_allocations = 0;

// Fonction pour vérifier l'intégrité des données
void verify_data(void* ptr, size_t size, int pattern) {
    unsigned char* data = (unsigned char*)ptr;
    unsigned char expected = (unsigned char)(pattern & 0xFF);
    
    for (size_t i = 0; i < size; i++) {
        if (data[i] != expected) {
            printf("ERREUR: Corruption détectée à l'offset %zu, attendu %d, trouvé %d\n", 
                   i, expected, data[i]);
            assert(0);
        }
    }
}

// Fonction pour écrire un pattern dans un bloc
void write_pattern(void* ptr, size_t size, int pattern) {
    unsigned char* data = (unsigned char*)ptr;
    unsigned char value = (unsigned char)(pattern & 0xFF);
    memset(data, value, size);
}

// Test 1: Allocations/libérations simples
void test_basic() {
    printf("=== Test basique ===\n");
    
    void* ptr1 = malloc(100);
    void* ptr2 = malloc(200);
    void* ptr3 = malloc(300);
    
    assert(ptr1 != NULL && ptr2 != NULL && ptr3 != NULL);
    assert(ptr1 != ptr2 && ptr2 != ptr3 && ptr1 != ptr3);
    
    write_pattern(ptr1, 100, 0xAA);
    write_pattern(ptr2, 200, 0xBB);
    write_pattern(ptr3, 300, 0xCC);
    
    verify_data(ptr1, 100, 0xAA);
    verify_data(ptr2, 200, 0xBB);
    verify_data(ptr3, 300, 0xCC);
    
    free(ptr2);
    
    // Vérifier que ptr1 et ptr3 sont toujours intacts
    verify_data(ptr1, 100, 0xAA);
    verify_data(ptr3, 300, 0xCC);
    
    void* ptr4 = malloc(150);
    assert(ptr4 != NULL);
    write_pattern(ptr4, 150, 0xDD);
    
    verify_data(ptr1, 100, 0xAA);
    verify_data(ptr3, 300, 0xCC);
    verify_data(ptr4, 150, 0xDD);
    
    free(ptr1);
    free(ptr3);
    free(ptr4);
    
    printf("Test basique: OK\n");
}

// Test 2: Fragmentation et coalescing
void test_fragmentation() {
    printf("=== Test fragmentation ===\n");
    
    void* ptrs[10];
    
    // Allouer 10 blocs
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(100);
        assert(ptrs[i] != NULL);
        write_pattern(ptrs[i], 100, i);
    }
    
    // Libérer les blocs pairs (créer des trous)
    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    // Vérifier que les blocs impairs sont toujours intacts
    for (int i = 1; i < 10; i += 2) {
        verify_data(ptrs[i], 100, i);
    }
    
    // Réallouer dans les trous
    for (int i = 0; i < 10; i += 2) {
        ptrs[i] = malloc(50);
        assert(ptrs[i] != NULL);
        write_pattern(ptrs[i], 50, i + 10);
    }
    
    // Vérifier tout
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            verify_data(ptrs[i], 50, i + 10);
        } else {
            verify_data(ptrs[i], 100, i);
        }
    }
    
    // Tout libérer
    for (int i = 0; i < 10; i++) {
        free(ptrs[i]);
    }
    
    printf("Test fragmentation: OK\n");
}

// Test 3: Stress test avec patterns aléatoires
void test_random_stress() {
    printf("=== Test de stress aléatoire ===\n");
    
    srand((unsigned int)time(NULL));
    num_allocations = 0;
    
    // Initialiser le tableau
    for (int i = 0; i < MAX_ALLOCS; i++) {
        allocations[i].active = 0;
    }
    
    int operations = 5000;
    int alloc_count = 0, free_count = 0;
    
    for (int op = 0; op < operations; op++) {
        int action = rand() % 100;
        
        if (action < 70 && num_allocations < MAX_ALLOCS) {
            // Allocation (70% de chance)
            size_t size = MIN_SIZE + (rand() % (MAX_SIZE - MIN_SIZE + 1));
            void* ptr = malloc(size);
            
            if (ptr != NULL) {
                int slot = -1;
                for (int i = 0; i < MAX_ALLOCS; i++) {
                    if (!allocations[i].active) {
                        slot = i;
                        break;
                    }
                }
                
                if (slot != -1) {
                    allocations[slot].ptr = ptr;
                    allocations[slot].size = size;
                    allocations[slot].pattern = rand() % 256;
                    allocations[slot].active = 1;
                    
                    write_pattern(ptr, size, allocations[slot].pattern);
                    num_allocations++;
                    alloc_count++;
                }
            }
        } else if (num_allocations > 0) {
            // Libération (30% de chance, ou forcée si pas assez de place)
            int active_slots[MAX_ALLOCS];
            int active_count = 0;
            
            for (int i = 0; i < MAX_ALLOCS; i++) {
                if (allocations[i].active) {
                    active_slots[active_count++] = i;
                }
            }
            
            if (active_count > 0) {
                int slot = active_slots[rand() % active_count];
                
                // Vérifier l'intégrité avant de libérer
                verify_data(allocations[slot].ptr, 
                          allocations[slot].size, 
                          allocations[slot].pattern);
                
                free(allocations[slot].ptr);
                allocations[slot].active = 0;
                num_allocations--;
                free_count++;
            }
        }
        
        // Périodiquement, vérifier l'intégrité de toutes les allocations actives
        if (op % 500 == 0) {
            printf("Opération %d: %d allocs actives\n", op, num_allocations);
            for (int i = 0; i < MAX_ALLOCS; i++) {
                if (allocations[i].active) {
                    verify_data(allocations[i].ptr, 
                              allocations[i].size, 
                              allocations[i].pattern);
                }
            }
        }
    }
    
    // Libérer tout ce qui reste
    for (int i = 0; i < MAX_ALLOCS; i++) {
        if (allocations[i].active) {
            verify_data(allocations[i].ptr, 
                      allocations[i].size, 
                      allocations[i].pattern);
            free(allocations[i].ptr);
            free_count++;
        }
    }
    
    printf("Stress test: %d allocations, %d libérations - OK\n", 
           alloc_count, free_count);
}

// Test 4: Cas limites
void test_edge_cases() {
    printf("=== Test des cas limites ===\n");
    
    // Test allocation de taille 0
    void* ptr_zero = malloc(0);
    if (ptr_zero != NULL) {
        free(ptr_zero);
    }
    
    // Test petites allocations
    void* small_ptrs[100];
    for (int i = 0; i < 100; i++) {
        small_ptrs[i] = malloc(1);
        if (small_ptrs[i]) {
            *(char*)small_ptrs[i] = (char)(i & 0xFF);
        }
    }
    
    // Vérifier les données
    for (int i = 0; i < 100; i++) {
        if (small_ptrs[i] && *(char*)small_ptrs[i] != (char)(i & 0xFF)) {
            printf("ERREUR: Corruption dans petite allocation %d\n", i);
            assert(0);
        }
    }
    
    // Libérer de manière alternée
    for (int i = 0; i < 100; i += 2) {
        if (small_ptrs[i]) free(small_ptrs[i]);
    }
    for (int i = 1; i < 100; i += 2) {
        if (small_ptrs[i]) free(small_ptrs[i]);
    }
    
    // Test grosses allocations
    void* big_ptr = malloc(MAX_SIZE);
    if (big_ptr) {
        write_pattern(big_ptr, MAX_SIZE, 0x55);
        verify_data(big_ptr, MAX_SIZE, 0x55);
        free(big_ptr);
    }
    
    printf("Test cas limites: OK\n");
}

// Test 5: Double free detection (optionnel)
void test_double_free() {
    printf("=== Test double free (peut crasher) ===\n");
    
    void* ptr = malloc(100);
    if (ptr) {
        free(ptr);
        // Attention: ceci devrait être détecté par votre implémentation
        // free(ptr); // Décommentez pour tester la détection de double free
    }
    
    printf("Test double free: OK\n");
}

// Test 6: Test de longévité (certaines allocations restent tout le test)
void test_longevity() {
    printf("=== Test de longévité ===\n");
    
    // Allocations qui vont rester tout le test
    void* persistent_ptrs[20];
    for (int i = 0; i < 20; i++) {
        persistent_ptrs[i] = malloc(256);
        assert(persistent_ptrs[i] != NULL);
        write_pattern(persistent_ptrs[i], 256, i);
    }
    
    // Faire beaucoup d'allocations/libérations autour
    for (int round = 0; round < 100; round++) {
        void* temp_ptrs[50];
        
        // Allouer
        for (int i = 0; i < 50; i++) {
			int nb = (rand() % 192);
            temp_ptrs[i] = malloc(64 + nb);
            if (temp_ptrs[i]) {
                write_pattern(temp_ptrs[i], 64 + nb, round + i);
            }
        }
        
        // Vérifier que les allocations persistantes sont toujours OK
        for (int i = 0; i < 20; i++) {
            verify_data(persistent_ptrs[i], 256, i);
        }
        
        // Libérer les temporaires
        for (int i = 0; i < 50; i++) {
            if (temp_ptrs[i]) {
                free(temp_ptrs[i]);
            }
        }
    }
    
    // Vérification finale et libération des persistantes
    for (int i = 0; i < 20; i++) {
        verify_data(persistent_ptrs[i], 256, i);
        free(persistent_ptrs[i]);
    }
    

    printf("Test de longévité: OK\n");
}

int main() {
    printf("=== TESTS COMPLETS MALLOC/FREE MAISON ===\n\n");
    
    test_basic();
    test_fragmentation();
    test_edge_cases();
    test_longevity();
    test_double_free();
    test_random_stress();
    
    printf("\n=== TOUS LES TESTS RÉUSSIS! ===\n");
    return 0;
}