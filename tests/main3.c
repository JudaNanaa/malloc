#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

// Assumons que votre malloc/free sont déclarés quelque part
// extern void* malloc(size_t size);
// extern void free(void* ptr);

#define DEBUG_ALLOCS 10
#define DEBUG_ROUNDS 50
#define MAGIC_PATTERN 0xAB

// Structure pour tracker précisément les allocations
typedef struct {
    void* ptr;
    size_t size;
    int active;
    int id;
} debug_alloc_t;

debug_alloc_t debug_allocations[DEBUG_ALLOCS];

void print_allocation_state() {
    printf("\n--- État des allocations ---\n");
    for (int i = 0; i < DEBUG_ALLOCS; i++) {
        printf("Slot %d: ptr=%p, size=%zu, active=%d, id=%d\n", 
               i, debug_allocations[i].ptr, debug_allocations[i].size, 
               debug_allocations[i].active, debug_allocations[i].id);
    }
    printf("-----------------------------\n\n");
}

void write_debug_pattern(void* ptr, size_t size, int id) {
    if (!ptr) return;
    
    unsigned char* data = (unsigned char*)ptr;
    for (size_t i = 0; i < size; i++) {
        data[i] = MAGIC_PATTERN + (id & 0xFF) + (i & 0xFF);
    }
}

int verify_debug_pattern(void* ptr, size_t size, int id) {
    if (!ptr) return 0;
    
    unsigned char* data = (unsigned char*)ptr;
    for (size_t i = 0; i < size; i++) {
        unsigned char expected = MAGIC_PATTERN + (id & 0xFF) + (i & 0xFF);
        if (data[i] != expected) {
            printf("❌ CORRUPTION! Slot ID %d, offset %zu: attendu 0x%02x, trouvé 0x%02x\n", 
                   id, i, expected, data[i]);
            return 0;
        }
    }
    return 1;
}

void test_debug_death_stress() {
    printf("🔍 === TEST DEBUG - Death Stress Simplifié === 🔍\n\n");
    
    // Initialiser le tracker
    memset(debug_allocations, 0, sizeof(debug_allocations));
    
    // Créer quelques allocations persistantes
    printf("📌 Création d'allocations persistantes...\n");
    for (int i = 0; i < 3; i++) {
        debug_allocations[i].size = 512 + (i * 256);
        debug_allocations[i].ptr = malloc(debug_allocations[i].size);
        debug_allocations[i].active = 1;
        debug_allocations[i].id = 1000 + i;
        
        printf("  Persistante %d: malloc(%zu) = %p\n", 
               i, debug_allocations[i].size, debug_allocations[i].ptr);
        
        if (debug_allocations[i].ptr) {
            write_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id);
        } else {
            printf("❌ ERREUR: malloc a retourné NULL!\n");
            return;
        }
    }
    
    print_allocation_state();
    
    // Boucle de stress simplifiée
    for (int round = 0; round < DEBUG_ROUNDS; round++) {
        printf("🔄 Round %d/%d\n", round + 1, DEBUG_ROUNDS);
        
        // Vérifier les allocations persistantes AVANT toute opération
        printf("  ✓ Vérification persistantes...\n");
        for (int i = 0; i < 3; i++) {
            if (debug_allocations[i].active && debug_allocations[i].ptr) {
                if (!verify_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id)) {
                    printf("❌ ERREUR: Allocation persistante %d corrompue au round %d!\n", i, round);
                    print_allocation_state();
                    return;
                }
            }
        }
        
        // Allocations temporaires (slots 3-9)
        printf("  📦 Allocations temporaires...\n");
        for (int i = 3; i < DEBUG_ALLOCS; i++) {
            if (!debug_allocations[i].active && (rand() % 100) < 70) {
                debug_allocations[i].size = (rand() % 1024) + 64;
                debug_allocations[i].ptr = malloc(debug_allocations[i].size);
                debug_allocations[i].id = round * 100 + i;
                
                printf("    Temp %d: malloc(%zu) = %p (id=%d)\n", 
                       i, debug_allocations[i].size, debug_allocations[i].ptr, debug_allocations[i].id);
                
                if (debug_allocations[i].ptr) {
                    debug_allocations[i].active = 1;
                    write_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id);
                } else {
                    printf("❌ ATTENTION: malloc temporaire a retourné NULL\n");
                }
            }
        }
        
        // Vérifier TOUTES les allocations actives
        printf("  ✓ Vérification complète...\n");
        for (int i = 0; i < DEBUG_ALLOCS; i++) {
            if (debug_allocations[i].active && debug_allocations[i].ptr) {
                if (!verify_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id)) {
                    printf("❌ CORRUPTION détectée dans slot %d au round %d!\n", i, round);
                    print_allocation_state();
                    return;
                }
            }
        }
        
        // Libérer quelques allocations temporaires
        printf("  🗑️  Libérations...\n");
        for (int i = 3; i < DEBUG_ALLOCS; i++) {
            if (debug_allocations[i].active && (rand() % 100) < 40) {
                printf("    free(%p) slot %d (id=%d)\n", 
                       debug_allocations[i].ptr, i, debug_allocations[i].id);
                
                // Vérifier une dernière fois avant de libérer
                if (!verify_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id)) {
                    printf("❌ CORRUPTION juste avant free()!\n");
                    print_allocation_state();
                    return;
                }
                
                free(debug_allocations[i].ptr);
                debug_allocations[i].active = 0;
                debug_allocations[i].ptr = NULL;
            }
        }
        
        // Vérification finale du round
        printf("  ✓ Vérification finale round...\n");
        for (int i = 0; i < 3; i++) {
            if (debug_allocations[i].active && debug_allocations[i].ptr) {
                if (!verify_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id)) {
                    printf("❌ ERREUR: Persistante %d corrompue à la fin du round %d!\n", i, round);
                    print_allocation_state();
                    return;
                }
            }
        }
        
        if (round % 10 == 0) {
            print_allocation_state();
        }
        
        printf("  ✅ Round %d OK\n\n", round + 1);
    }
    
    printf("🧹 Nettoyage final...\n");
    print_allocation_state();
    
    // Libérer toutes les allocations restantes
    for (int i = 0; i < DEBUG_ALLOCS; i++) {
        if (debug_allocations[i].active && debug_allocations[i].ptr) {
            printf("  Final free(%p) slot %d\n", debug_allocations[i].ptr, i);
            
            // Vérification avant libération finale
            if (!verify_debug_pattern(debug_allocations[i].ptr, debug_allocations[i].size, debug_allocations[i].id)) {
                printf("❌ CORRUPTION dans le nettoyage final!\n");
            }
            
            free(debug_allocations[i].ptr);
            debug_allocations[i].active = 0;
            debug_allocations[i].ptr = NULL;
        }
    }
    
    printf("🔍 Test debug terminé avec succès!\n");
}

// Test encore plus minimal pour isoler le problème
void test_minimal_stress() {
    printf("🔬 === TEST MINIMAL === 🔬\n");
    
    void* persistent = malloc(1024);
    printf("Persistent: %p\n", persistent);
    if (!persistent) {
        printf("❌ Échec allocation persistante\n");
        return;
    }
    memset(persistent, 0xAA, 1024);
    
    for (int i = 0; i < 20; i++) {
        printf("Round %d: ", i);
        
        // Vérifier la persistante
        unsigned char* check = (unsigned char*)persistent;
        for (int j = 0; j < 1024; j++) {
            if (check[j] != 0xAA) {
                printf("❌ Corruption persistante à l'offset %d\n", j);
                free(persistent);
                return;
            }
        }
        
        // Faire quelques allocations temporaires
        void* temp1 = malloc(256);
        void* temp2 = malloc(512);
        
        printf("temp1=%p temp2=%p ", temp1, temp2);
        
        if (temp1) {
            memset(temp1, 0xBB, 256);
            free(temp1);
        }
        if (temp2) {
            memset(temp2, 0xCC, 512);
            free(temp2);
        }
        
        printf("OK\n");
    }
    
    free(persistent);
    printf("🔬 Test minimal OK\n");
}

int main() {
    printf("🐛 === TESTS DE DÉBOGAGE malloc === 🐛\n\n");
    
    srand((unsigned int)time(NULL));
    
    printf("Commençons par le test minimal...\n\n");
    test_minimal_stress();
    
    printf("\nMaintenant le test debug complet...\n\n");
    test_debug_death_stress();
    
    printf("\n🎉 Tous les tests de débogage réussis!\n");
    return 0;
}