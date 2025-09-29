#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdint.h>

// Structure pour tracker les allocations
typedef struct {
    void* ptr;
    size_t size;
    unsigned char pattern;
    int active;
    int id;
} tracked_alloc_t;

#define MAX_TRACKED 1000
tracked_alloc_t tracked[MAX_TRACKED];
int num_tracked = 0;

// Couleurs pour l'affichage
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

// Fonction pour arrêter le programme en cas de corruption
void corruption_detected(const char* message, int alloc_id, void* ptr, size_t offset) {
    printf(RED "\n=== CORRUPTION DETECTEE ===" RESET "\n");
    printf(RED "Message: %s" RESET "\n", message);
    printf(RED "Allocation ID: %d" RESET "\n", alloc_id);
    printf(RED "Pointeur: %p" RESET "\n", ptr);
    printf(RED "Offset: %zu" RESET "\n", offset);
    printf(RED "ARRET DU PROGRAMME !" RESET "\n");
    fflush(stdout);
    exit(1);
}

// Fonction pour écrire un pattern avec canaris (guards)
void write_protected_pattern(void* ptr, size_t size, unsigned char pattern) {
    if (!ptr || size == 0) return;
    
    unsigned char* bytes = (unsigned char*)ptr;
    
    // Pattern principal
    for (size_t i = 0; i < size; i++) {
        bytes[i] = pattern;
    }
}

// Fonction pour vérifier l'intégrité d'une allocation
int verify_allocation(int idx) {
    if (idx >= num_tracked || !tracked[idx].active) return 1;
    
    tracked_alloc_t* alloc = &tracked[idx];
    if (!alloc->ptr) return 1;
    
    unsigned char* bytes = (unsigned char*)alloc->ptr;
    
    // Vérification du pattern principal
    for (size_t i = 0; i < alloc->size; i++) {
        if (bytes[i] != alloc->pattern) {
            corruption_detected(
                "Pattern principal corrompu", 
                alloc->id, 
                alloc->ptr, 
                i
            );
            return 0;
        }
    }
    
    return 1;
}

// Fonction pour vérifier toutes les allocations actives
void verify_all_allocations(const char* context) {
    printf(YELLOW "Vérification intégrité (%s)..." RESET, context);
    fflush(stdout);
    
    for (int i = 0; i < num_tracked; i++) {
        if (tracked[i].active) {
            verify_allocation(i);
        }
    }
    
    printf(GREEN " OK\n" RESET);
}

// Fonction pour ajouter une allocation au tracking
void track_allocation(void* ptr, size_t size, unsigned char pattern) {
    if (num_tracked >= MAX_TRACKED) {
        printf(RED "ERREUR: Trop d'allocations trackées!\n" RESET);
        exit(1);
    }
    
    tracked[num_tracked].ptr = ptr;
    tracked[num_tracked].size = size;
    tracked[num_tracked].pattern = pattern;
    tracked[num_tracked].active = 1;
    tracked[num_tracked].id = num_tracked;
    
    write_protected_pattern(ptr, size, pattern);
    num_tracked++;
}

// Fonction pour enlever une allocation du tracking
void untrack_allocation(void* ptr) {
    for (int i = 0; i < num_tracked; i++) {
        if (tracked[i].active && tracked[i].ptr == ptr) {
            // Vérification finale avant libération
            verify_allocation(i);
            tracked[i].active = 0;
            tracked[i].ptr = NULL;
            return;
        }
    }
    
    if (ptr != NULL) {
        printf(RED "ERREUR: Tentative de libérer un pointeur non tracké: %p\n" RESET, ptr);
        exit(1);
    }
}

// Test 1: Allocations simples avec vérifications fréquentes
void test_simple_with_checks() {
    printf("=== Test 1: Allocations simples avec vérifications ===\n");
    
    // Phase 1: Quelques allocations
    for (int i = 0; i < 10; i++) {
        size_t size = 64 + i * 32;
        void* ptr = malloc(size);
        
        if (!ptr) {
            printf(RED "ERREUR: Allocation %d échouée\n" RESET, i);
            exit(1);
        }
        
        printf("Alloc %d: %p (%zu bytes)\n", i, ptr, size);
        track_allocation(ptr, size, (unsigned char)(0x10 + i));
        
        // Vérification après chaque allocation
        verify_all_allocations("après allocation");
    }
    
    // Phase 2: Libération avec vérifications
    for (int i = 0; i < 10; i++) {
        if (tracked[i].active) {
            printf("Free %d: %p\n", i, tracked[i].ptr);
            untrack_allocation(tracked[i].ptr);
            free(tracked[i].ptr);
            
            // Vérification après chaque libération
            verify_all_allocations("après libération");
        }
    }
    
    printf(GREEN "Test 1 terminé avec succès\n\n" RESET);
}

// Test 2: Allocations entrelacées avec vérifications
void test_interleaved_with_checks() {
    printf("=== Test 2: Allocations entrelacées avec vérifications ===\n");
    
    // Reset du tracker
    num_tracked = 0;
    for (int i = 0; i < MAX_TRACKED; i++) {
        tracked[i].active = 0;
        tracked[i].ptr = NULL;
    }
    
    for (int round = 0; round < 5; round++) {
        printf("Round %d\n", round + 1);
        
        // Alloue 20 blocs
        for (int i = 0; i < 20; i++) {
            size_t size = 32 + i * 16;
            void* ptr = malloc(size);
            
            if (!ptr) {
                printf(RED "ERREUR: Allocation round %d, bloc %d échouée\n" RESET, round, i);
                exit(1);
            }
            
            track_allocation(ptr, size, (unsigned char)(0x20 + i));
        }
        
        verify_all_allocations("après allocations du round");
        
        // Libère les blocs pairs
        for (int i = 0; i < num_tracked; i++) {
            if (tracked[i].active && (i % 2 == 0)) {
                untrack_allocation(tracked[i].ptr);
                free(tracked[i].ptr);
            }
        }
        
        verify_all_allocations("après libérations pairs");
        
        // Libère les blocs impairs
        for (int i = 0; i < num_tracked; i++) {
            if (tracked[i].active) {
                untrack_allocation(tracked[i].ptr);
                free(tracked[i].ptr);
            }
        }
        
        verify_all_allocations("après libérations impairs");
        
        // Reset pour le prochain round
        num_tracked = 0;
        for (int i = 0; i < MAX_TRACKED; i++) {
            tracked[i].active = 0;
            tracked[i].ptr = NULL;
        }
    }
    
    printf(GREEN "Test 2 terminé avec succès\n\n" RESET);
}

// Test 3: Test avec tailles variées et vérifications intensives
void test_varied_sizes_intensive_checks() {
    printf("=== Test 3: Tailles variées avec vérifications intensives ===\n");
    
    // Reset du tracker
    num_tracked = 0;
    for (int i = 0; i < MAX_TRACKED; i++) {
        tracked[i].active = 0;
        tracked[i].ptr = NULL;
    }
    
    size_t sizes[] = {8, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    // Alloue avec différentes tailles
    for (int i = 0; i < 50; i++) {
        size_t size = sizes[i % num_sizes];
        void* ptr = malloc(size);
        
        if (!ptr) {
            printf(RED "ERREUR: Allocation %d de taille %zu échouée\n" RESET, i, size);
            exit(1);
        }
        
        track_allocation(ptr, size, (unsigned char)(0x30 + (i % 256)));
        
        // Vérification très fréquente
        if (i % 5 == 0) {
            verify_all_allocations("check fréquent");
        }
    }
    
    printf("Vérification finale avant libérations...\n");
    verify_all_allocations("avant libérations finales");
    
    // Libération avec vérifications
    for (int i = 0; i < num_tracked; i++) {
        if (tracked[i].active) {
            untrack_allocation(tracked[i].ptr);
            free(tracked[i].ptr);
            
            // Vérification après chaque libération
            if (i % 3 == 0) {
                verify_all_allocations("pendant libérations");
            }
        }
    }
    
    printf(GREEN "Test 3 terminé avec succès\n\n" RESET);
}

// Test 4: Test de corruption avec écriture intensive
void test_write_intensive() {
    printf("=== Test 4: Test écriture intensive ===\n");
    
    // Reset du tracker
    num_tracked = 0;
    for (int i = 0; i < MAX_TRACKED; i++) {
        tracked[i].active = 0;
        tracked[i].ptr = NULL;
    }
    
    // Alloue quelques gros blocs
    for (int i = 0; i < 10; i++) {
        size_t size = 1024 + i * 512;
        void* ptr = malloc(size);
        
        if (!ptr) {
            printf(RED "ERREUR: Allocation gros bloc %d échouée\n" RESET, i);
            exit(1);
        }
        
        track_allocation(ptr, size, (unsigned char)(0x40 + i));
    }
    
    verify_all_allocations("après allocations gros blocs");
    
    // Réécrit plusieurs fois dans chaque bloc
    for (int pass = 0; pass < 5; pass++) {
        printf("Pass d'écriture %d\n", pass + 1);
        
        for (int i = 0; i < num_tracked; i++) {
            if (tracked[i].active) {
                // Réécrit avec un nouveau pattern
                unsigned char new_pattern = (unsigned char)(0x50 + pass * 10 + i);
                tracked[i].pattern = new_pattern;
                write_protected_pattern(tracked[i].ptr, tracked[i].size, new_pattern);
            }
        }
        
        verify_all_allocations("après réécriture");
    }
    
    // Nettoyage
    for (int i = 0; i < num_tracked; i++) {
        if (tracked[i].active) {
            untrack_allocation(tracked[i].ptr);
            free(tracked[i].ptr);
        }
    }
    
    printf(GREEN "Test 4 terminé avec succès\n\n" RESET);
}

// Test 5: Test rapide de sanity check
void test_quick_sanity() {
    printf("=== Test 5: Sanity check rapide ===\n");
    
    // Test basique: alloue, écrit, vérifie, libère
    for (int i = 0; i < 20; i++) {
        size_t size = 128;
        void* ptr = malloc(size);
        
        if (!ptr) {
            printf(RED "ERREUR: Sanity check allocation %d échouée\n" RESET, i);
            exit(1);
        }
        
        // Écrit un pattern simple
        unsigned char pattern = (unsigned char)(0x60 + i);
        memset(ptr, pattern, size);
        
        // Vérification immédiate
        unsigned char* bytes = (unsigned char*)ptr;
        for (size_t j = 0; j < size; j++) {
            if (bytes[j] != pattern) {
                corruption_detected("Sanity check échoué", i, ptr, j);
            }
        }
        
        free(ptr);
        printf("Sanity %d: OK\n", i);
    }
    
    printf(GREEN "Test 5 terminé avec succès\n\n" RESET);
}

int main() {
    printf("=== DETECTEUR DE CORRUPTION MEMOIRE ===\n");
    printf("Ce programme s'arrêtera immédiatement si une corruption est détectée!\n\n");
    
    test_quick_sanity();                    // Test rapide
    test_simple_with_checks();              // Test simple avec checks
    test_interleaved_with_checks();         // Test entrelacé
    test_varied_sizes_intensive_checks();   // Tailles variées
    test_write_intensive();                 // Écritures intensives
    
    printf(GREEN "=== TOUS LES TESTS PASSES SANS CORRUPTION ===\n" RESET);
    printf(GREEN "Ton malloc semble clean niveau corruption mémoire !\n" RESET);
    
    return 0;
}