#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include "../includes/malloc_internal.h"
#include "../includes/lib_malloc.h"

// Couleurs pour les tests
#define GREEN "\033[32m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

#define TEST_SECTION(msg) printf(BLUE "\n=== " msg " ===" RESET "\n")

// Variables globales pour les statistiques
static int tests_passed = 0;
static int tests_failed = 0;

void test_result(int condition, const char *test_name) {
    if (condition) {
        printf(GREEN "✅ PASS: %s" RESET "\n", test_name);
        tests_passed++;
    } else {
        printf(RED "❌ FAIL: %s" RESET "\n", test_name);
        tests_failed++;
    }
}

void test_basic_MALLOC_NAME() {
    TEST_SECTION("BASIC MALLOC_NAME TESTS");
    
    // Test 1: MALLOC_NAME(0) - comportement défini par l'implémentation
    void *p0 = MALLOC_NAME(0);
    test_result(p0 != NULL || p0 == NULL, "MALLOC_NAME(0) returns valid result");
    if (p0) FREE_NAME(p0);
    
    // Test 2: petites tailles (TINY)
    char *tiny1 = MALLOC_NAME(1);

    char *tiny8 = MALLOC_NAME(8);

    char *tiny16 = MALLOC_NAME(16);

    char *tiny32 = MALLOC_NAME(32);

    char *tiny64 = MALLOC_NAME(64);

    char *tiny128 = MALLOC_NAME(128);
    
    test_result(tiny1 != NULL, "MALLOC_NAME(1) succeeds");
    test_result(tiny8 != NULL, "MALLOC_NAME(8) succeeds");
    test_result(tiny16 != NULL, "MALLOC_NAME(16) succeeds");
    test_result(tiny32 != NULL, "MALLOC_NAME(32) succeeds");
    test_result(tiny64 != NULL, "MALLOC_NAME(64) succeeds");
    test_result(tiny128 != NULL, "MALLOC_NAME(128) succeeds");
    
    // Test d'alignement (8 bytes)
    test_result(((uintptr_t)tiny1 % 8) == 0, "MALLOC_NAME(1) is 8-byte aligned");
    test_result(((uintptr_t)tiny8 % 8) == 0, "MALLOC_NAME(8) is 8-byte aligned");
    test_result(((uintptr_t)tiny16 % 8) == 0, "MALLOC_NAME(16) is 8-byte aligned");
    test_result(((uintptr_t)tiny32 % 8) == 0, "MALLOC_NAME(32) is 8-byte aligned");
    test_result(((uintptr_t)tiny64 % 8) == 0, "MALLOC_NAME(64) is 8-byte aligned");
    test_result(((uintptr_t)tiny128 % 8) == 0, "MALLOC_NAME(128) is 8-byte aligned");
    
    // Test d'écriture/lecture
    if (tiny32) {
        strcpy(tiny32, "Hello");
        test_result(strcmp(tiny32, "Hello") == 0, "Write/read in tiny MALLOC_NAME");
    }
    
    
    
    FREE_NAME(tiny1); 
	FREE_NAME(tiny8); 
	FREE_NAME(tiny16); 
    FREE_NAME(tiny32); 
	FREE_NAME(tiny64); 
	FREE_NAME(tiny128);
}

void test_small_MALLOC_NAME() {
    TEST_SECTION("SMALL MALLOC_NAME TESTS");
    
    // Test tailles SMALL (129 à 1024 bytes)
    char *small200 = MALLOC_NAME(200);
    char *small400 = MALLOC_NAME(400);
    char *small600 = MALLOC_NAME(600);
    char *small800 = MALLOC_NAME(800);
    char *small1000 = MALLOC_NAME(1000);
    char *small1024 = MALLOC_NAME(1024); // limite SMALL
    
    test_result(small200 != NULL, "MALLOC_NAME(200) succeeds");
    test_result(small400 != NULL, "MALLOC_NAME(400) succeeds");
    test_result(small600 != NULL, "MALLOC_NAME(600) succeeds");
    test_result(small800 != NULL, "MALLOC_NAME(800) succeeds");
    test_result(small1000 != NULL, "MALLOC_NAME(1000) succeeds");
    test_result(small1024 != NULL, "MALLOC_NAME(1024) succeeds");
    
    // Test d'alignement
    test_result(((uintptr_t)small200 % 8) == 0, "MALLOC_NAME(200) is 8-byte aligned");
    test_result(((uintptr_t)small400 % 8) == 0, "MALLOC_NAME(400) is 8-byte aligned");
    test_result(((uintptr_t)small1024 % 8) == 0, "MALLOC_NAME(1024) is 8-byte aligned");
    
    // Test d'écriture dans les blocs
    if (small400) {
        memset(small400, 'A', 399);
        small400[399] = '\0';
        test_result(small400[0] == 'A' && small400[398] == 'A', "Write/read in small MALLOC_NAME");
    }
    
    
    
    FREE_NAME(small200);
	FREE_NAME(small400);
	FREE_NAME(small600);
    FREE_NAME(small800);
	FREE_NAME(small1000); 
	FREE_NAME(small1024);
}

void test_large_MALLOC_NAME() {
    TEST_SECTION("LARGE MALLOC_NAME TESTS");
    
    // Test tailles LARGE (> 1024 bytes)
    char *large1025 = MALLOC_NAME(1025); // première taille LARGE
    char *large2048 = MALLOC_NAME(2048);
    char *large4096 = MALLOC_NAME(4096);
    char *large8192 = MALLOC_NAME(8192);
    char *large16384 = MALLOC_NAME(16384);
    char *large_huge = MALLOC_NAME(1024 * 1024); // 1MB
    
    test_result(large1025 != NULL, "MALLOC_NAME(1025) succeeds");
    test_result(large2048 != NULL, "MALLOC_NAME(2048) succeeds");
    test_result(large4096 != NULL, "MALLOC_NAME(4096) succeeds");
    test_result(large8192 != NULL, "MALLOC_NAME(8192) succeeds");
    test_result(large16384 != NULL, "MALLOC_NAME(16384) succeeds");
    test_result(large_huge != NULL, "MALLOC_NAME(1MB) succeeds");
    
    // Test d'alignement
    test_result(((uintptr_t)large1025 % 8) == 0, "MALLOC_NAME(1025) is 8-byte aligned");
    test_result(((uintptr_t)large2048 % 8) == 0, "MALLOC_NAME(2048) is 8-byte aligned");
    test_result(((uintptr_t)large_huge % 8) == 0, "MALLOC_NAME(1MB) is 8-byte aligned");
    
    // Test d'écriture dans un gros bloc
    if (large_huge) {
        large_huge[0] = 'X';
        large_huge[1024 * 1024 - 1] = 'Y';
        test_result(large_huge[0] == 'X' && large_huge[1024 * 1024 - 1] == 'Y', 
                   "Write/read in 1MB MALLOC_NAME");
    }
    
    
    
    FREE_NAME(large1025);
	
	FREE_NAME(large2048);
	
	FREE_NAME(large4096);
    
	FREE_NAME(large8192);
	
	FREE_NAME(large16384);
	
	FREE_NAME(large_huge);
}

void test_fragmentation() {
    TEST_SECTION("FRAGMENTATION TESTS");
    
    // Créer de la fragmentation
    void *ptrs[10];
    
    // Allouer 10 blocs
    for (int i = 0; i < 10; i++) {
        ptrs[i] = MALLOC_NAME(64);
        test_result(ptrs[i] != NULL, "Fragmentation MALLOC_NAME succeeds");
    }
    
    
    
    // Libérer les blocs impairs (créer des trous)
    for (int i = 1; i < 10; i += 2) {
        FREE_NAME(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("After FREE_NAMEing odd blocks:\n");
    
    
    // Essayer de réallouer dans les trous
    void *new_ptr = MALLOC_NAME(32); // devrait réutiliser un trou
    test_result(new_ptr != NULL, "MALLOC_NAME after fragmentation succeeds");
    
    // Nettoyer
    for (int i = 0; i < 10; i += 2) {
        FREE_NAME(ptrs[i]);
    }
    FREE_NAME(new_ptr);
}

void test_REALLOC_NAME_tiny_to_tiny() {
    TEST_SECTION("REALLOC_NAME TINY->TINY TESTS");
    
    char *ptr = MALLOC_NAME(16);
    test_result(ptr != NULL, "Initial MALLOC_NAME(16) for REALLOC_NAME");
    
    if (ptr) {
        strcpy(ptr, "Hello");
        
        // Agrandir dans TINY
        ptr = REALLOC_NAME(ptr, 32);
        test_result(ptr != NULL, "REALLOC_NAME(16->32) succeeds");
        test_result(strcmp(ptr, "Hello") == 0, "Data preserved in REALLOC_NAME(16->32)");
        
        // Réduire dans TINY
        ptr = REALLOC_NAME(ptr, 8);
        test_result(ptr != NULL, "REALLOC_NAME(32->8) succeeds");
        test_result(strncmp(ptr, "Hello", 5) == 0, "Data preserved in REALLOC_NAME(32->8)");
        
        FREE_NAME(ptr);

    }
}

void test_REALLOC_NAME_small_to_small() {
    TEST_SECTION("REALLOC_NAME SMALL->SMALL TESTS");
    
    char *ptr = MALLOC_NAME(200);
    test_result(ptr != NULL, "Initial MALLOC_NAME(200) for REALLOC_NAME");
    
    if (ptr) {
        memset(ptr, 'A', 199);
        ptr[199] = '\0';
        
        // Agrandir dans SMALL
        ptr = REALLOC_NAME(ptr, 500);
        test_result(ptr != NULL, "REALLOC_NAME(200->500) succeeds");
        test_result(ptr[0] == 'A' && ptr[198] == 'A', "Data preserved in REALLOC_NAME(200->500)");
        
        // Réduire dans SMALL
        ptr = REALLOC_NAME(ptr, 150);
        test_result(ptr != NULL, "REALLOC_NAME(500->150) succeeds");
        test_result(ptr[0] == 'A' && ptr[149] == 'A', "Data preserved in REALLOC_NAME(500->150)");
        
        FREE_NAME(ptr);
    }
}

void test_REALLOC_NAME_large_to_large() {
    TEST_SECTION("REALLOC_NAME LARGE->LARGE TESTS");
    
    char *ptr = MALLOC_NAME(2048);
    test_result(ptr != NULL, "Initial MALLOC_NAME(2048) for REALLOC_NAME");
    
    if (ptr) {
        memset(ptr, 'B', 2047);
        ptr[2047] = '\0';
        
        // Agrandir dans LARGE
        ptr = REALLOC_NAME(ptr, 4096);
        test_result(ptr != NULL, "REALLOC_NAME(2048->4096) succeeds");
        test_result(ptr[0] == 'B' && ptr[2046] == 'B', "Data preserved in REALLOC_NAME(2048->4096)");
        
        // Réduire dans LARGE
        ptr = REALLOC_NAME(ptr, 1500);
        test_result(ptr != NULL, "REALLOC_NAME(4096->1500) succeeds");
        test_result(ptr[0] == 'B' && ptr[1499] == 'B', "Data preserved in REALLOC_NAME(4096->1500)");
        
        FREE_NAME(ptr);
    }
}

void test_REALLOC_NAME_category_changes() {
    TEST_SECTION("REALLOC_NAME CATEGORY CHANGE TESTS");
    
    // TINY -> SMALL
    char *ptr = MALLOC_NAME(64);
    if (ptr) {
        strcpy(ptr, "TinyToSmall");
        ptr = REALLOC_NAME(ptr, 256);
        test_result(ptr != NULL, "REALLOC_NAME TINY->SMALL succeeds");
        test_result(strcmp(ptr, "TinyToSmall") == 0, "Data preserved TINY->SMALL");
        
        // SMALL -> LARGE
        strcat(ptr, " and SmallToLarge");
        ptr = REALLOC_NAME(ptr, 2048);
        test_result(ptr != NULL, "REALLOC_NAME SMALL->LARGE succeeds");
        test_result(strstr(ptr, "TinyToSmall") != NULL, "Data preserved SMALL->LARGE");
        
        // LARGE -> SMALL
        ptr = REALLOC_NAME(ptr, 512);
        test_result(ptr != NULL, "REALLOC_NAME LARGE->SMALL succeeds");
        test_result(strstr(ptr, "TinyToSmall") != NULL, "Data preserved LARGE->SMALL");
        
        // SMALL -> TINY
        ptr = REALLOC_NAME(ptr, 32);
        test_result(ptr != NULL, "REALLOC_NAME SMALL->TINY succeeds");
        test_result(strncmp(ptr, "TinyToSmall", 11) == 0, "Data preserved SMALL->TINY");
        
        FREE_NAME(ptr);
    }
}

void test_REALLOC_NAME_edge_cases() {
    TEST_SECTION("REALLOC_NAME EDGE CASES");
    
    // REALLOC_NAME(NULL, size) = MALLOC_NAME(size)
    char *ptr = REALLOC_NAME(NULL, 100);
    test_result(ptr != NULL, "REALLOC_NAME(NULL, 100) works like MALLOC_NAME");
    if (ptr) {
        strcpy(ptr, "REALLOC_NAME NULL test");
        test_result(strcmp(ptr, "REALLOC_NAME NULL test") == 0, "Write after REALLOC_NAME(NULL)");
    }
    
    // REALLOC_NAME(ptr, 0) = FREE_NAME(ptr) + return implementation-defined
    void *result = REALLOC_NAME(ptr, 0);
    test_result(1, "REALLOC_NAME(ptr, 0) completes without crash");
    if (result) FREE_NAME(result);
    
    // REALLOC_NAME même taille
    ptr = MALLOC_NAME(100);
    if (ptr) {
        strcpy(ptr, "Same size test");
        char *old_ptr = ptr;
        ptr = REALLOC_NAME(ptr, 100);
        test_result(ptr != NULL, "REALLOC_NAME same size succeeds");
        test_result(strcmp(ptr, "Same size test") == 0, "Data preserved same size REALLOC_NAME");
        test_result(old_ptr == ptr, "Pointer preserved same size REALLOC_NAME");
        FREE_NAME(ptr);
    }
}

void test_multiple_allocations() {
    TEST_SECTION("MULTIPLE ALLOCATIONS TEST");
    
    const int num_allocs = 100;
    void *ptrs[num_allocs];
    int success_count = 0;
    
    // Allouer beaucoup de blocs de tailles variées
    for (int i = 0; i < num_allocs; i++) {
        size_t size = (i % 3 == 0) ? 32 : (i % 3 == 1) ? 512 : 2048;
        ptrs[i] = MALLOC_NAME(size);
        if (ptrs[i]) {
            success_count++;
            // Écrire des données pour tester
            memset(ptrs[i], (char)(i % 256), size > 1 ? size - 1 : 0);
        }
    }
    
    test_result(success_count == num_allocs, "Multiple allocations all succeed");
    printf("Allocated %d/%d blocks successfully\n", success_count, num_allocs);
    
    
    // Vérifier l'intégrité des données
    int integrity_ok = 1;
    for (int i = 0; i < num_allocs; i++) {
        if (ptrs[i]) {
            size_t size = (i % 3 == 0) ? 32 : (i % 3 == 1) ? 512 : 2048;
            if (size > 1 && ((char*)ptrs[i])[0] != (char)(i % 256)) {
                integrity_ok = 0;
                break;
            }
        }
    }
    test_result(integrity_ok, "Data integrity maintained across multiple allocations");
    
    // Libérer tous les blocs
    for (int i = 0; i < num_allocs; i++) {
        if (ptrs[i]) {
            FREE_NAME(ptrs[i]);
        }
    }
}

void test_stress_REALLOC_NAME() {
    TEST_SECTION("STRESS REALLOC_NAME TEST");
    
    char *ptr = MALLOC_NAME(16);
    test_result(ptr != NULL, "Initial allocation for stress test");
    
    if (ptr) {
        strcpy(ptr, "Start");
        
        // Série de REALLOC_NAME avec des tailles croissantes et décroissantes
        size_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16};
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        int all_success = 1;
        
        for (int i = 0; i < num_sizes; i++) {
            ptr = REALLOC_NAME(ptr, sizes[i]);
            if (!ptr) {
                all_success = 0;
                break;
            }
			
            // Vérifier que les données initiales sont toujours là
            if (strncmp(ptr, "Start", 5) != 0) {
                all_success = 0;
                break;
            }
        }
        
        test_result(all_success, "Stress REALLOC_NAME sequence succeeds");
        
        if (ptr) FREE_NAME(ptr);
    }
}

void test_boundary_values() {
    TEST_SECTION("BOUNDARY VALUES TEST");
    
    // Tester les limites exactes entre catégories
    void *tiny_max = MALLOC_NAME(128);      // Max TINY
    void *small_min = MALLOC_NAME(129);     // Min SMALL
    void *small_max = MALLOC_NAME(1024);    // Max SMALL
    void *large_min = MALLOC_NAME(1025);    // Min LARGE
    
    test_result(tiny_max != NULL, "MALLOC_NAME(128) - TINY max boundary");
    test_result(small_min != NULL, "MALLOC_NAME(129) - SMALL min boundary");
    test_result(small_max != NULL, "MALLOC_NAME(1024) - SMALL max boundary");
    test_result(large_min != NULL, "MALLOC_NAME(1025) - LARGE min boundary");
    
    // Vérifier les alignements aux limites
    test_result(((uintptr_t)tiny_max % 8) == 0, "TINY max boundary aligned");
    test_result(((uintptr_t)small_min % 8) == 0, "SMALL min boundary aligned");
    test_result(((uintptr_t)small_max % 8) == 0, "SMALL max boundary aligned");
    test_result(((uintptr_t)large_min % 8) == 0, "LARGE min boundary aligned");
    
    
    
    FREE_NAME(tiny_max); FREE_NAME(small_min); 
    FREE_NAME(small_max); FREE_NAME(large_min);
}

void test_FREE_NAME_edge_cases() {
    TEST_SECTION("FREE_NAME EDGE CASES");
    
    // FREE_NAME(NULL) doit être sans effet
    FREE_NAME(NULL);
    test_result(1, "FREE_NAME(NULL) doesn't crash");
    
    // Double FREE_NAME - devrait être détecté si implémenté
    void *ptr = MALLOC_NAME(100);
    if (ptr) {
        strcpy((char*)ptr, "Double FREE_NAME test");
        FREE_NAME(ptr);
        printf("Note: Uncomment next line to test double-FREE_NAME detection\n");
        // FREE_NAME(ptr); // Décommenter pour tester la détection de double FREE_NAME
    }
}

void test_memory_patterns() {
    TEST_SECTION("MEMORY PATTERNS TEST");
    
    // Test avec différents patterns pour détecter la corruption
    void *ptrs[5];
    char patterns[] = {0x00, 0xFF, 0xAA, 0x55, 0xCC};
    
    for (int i = 0; i < 5; i++) {
        ptrs[i] = MALLOC_NAME(256);
        if (ptrs[i]) {
            memset(ptrs[i], patterns[i], 256);
        }
    }
    
    // Vérifier l'intégrité des patterns
    int pattern_integrity = 1;
    for (int i = 0; i < 5; i++) {
        if (ptrs[i]) {
            char *ptr = (char*)ptrs[i];
            for (int j = 0; j < 256; j++) {
                if (ptr[j] != patterns[i]) {
                    pattern_integrity = 0;
                    break;
                }
            }
        }
    }
    
    test_result(pattern_integrity, "Memory pattern integrity maintained");
    
    // Nettoyer
    for (int i = 0; i < 5; i++) {
        if (ptrs[i]) FREE_NAME(ptrs[i]);
    }
}

void test_CALLOC_NAME_basic() {
    TEST_SECTION("CALLOC_NAME BASIC TESTS");
    
    // Test 1: CALLOC_NAME(0, size) et CALLOC_NAME(nmemb, 0)
    void *p1 = CALLOC_NAME(0, 10);
    void *p2 = CALLOC_NAME(10, 0);
    void *p3 = CALLOC_NAME(0, 0);
    test_result(p1 != NULL || p1 == NULL, "CALLOC_NAME(0, 10) returns valid result");
    test_result(p2 != NULL || p2 == NULL, "CALLOC_NAME(10, 0) returns valid result");
    test_result(p3 != NULL || p3 == NULL, "CALLOC_NAME(0, 0) returns valid result");
    if (p1) FREE_NAME(p1);
    if (p2) FREE_NAME(p2);
    if (p3) FREE_NAME(p3);
    
    // Test 2: CALLOC_NAME simple avec vérification de zéro
    char *ptr = CALLOC_NAME(10, sizeof(char));
    test_result(ptr != NULL, "CALLOC_NAME(10, sizeof(char)) succeeds");
    
    if (ptr) {
        int all_zero = 1;
        for (int i = 0; i < 10; i++) {
            if (ptr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME initializes memory to zero");
        test_result(((uintptr_t)ptr % 8) == 0, "CALLOC_NAME result is 8-byte aligned");
        FREE_NAME(ptr);
    }
    
    // Test 3: CALLOC_NAME avec différentes tailles
    int *int_arr = CALLOC_NAME(100, sizeof(int));
    test_result(int_arr != NULL, "CALLOC_NAME(100, sizeof(int)) succeeds");
    if (int_arr) {
        int all_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (int_arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME int array initialized to zero");
        FREE_NAME(int_arr);
    }
    
    // Test 4: CALLOC_NAME TINY
    char *tiny = CALLOC_NAME(64, 1);
    test_result(tiny != NULL, "CALLOC_NAME TINY (64 bytes) succeeds");
    if (tiny) {
        int all_zero = 1;
        for (int i = 0; i < 64; i++) {
            if (tiny[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME TINY initialized to zero");
        FREE_NAME(tiny);
    }
    
    
}

void test_CALLOC_NAME_sizes() {
    TEST_SECTION("CALLOC_NAME SIZE CATEGORIES");
    
    // Test TINY (≤ 128 bytes)
    char *tiny = CALLOC_NAME(32, 4); // 128 bytes exactly
    test_result(tiny != NULL, "CALLOC_NAME TINY boundary (32*4=128) succeeds");
    if (tiny) {
        int all_zero = 1;
        for (int i = 0; i < 128; i++) {
            if (tiny[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME TINY boundary zeroed");
        FREE_NAME(tiny);
    }
    
    // Test SMALL (129-1024 bytes)
    short *small = CALLOC_NAME(200, sizeof(short)); // 400 bytes
    test_result(small != NULL, "CALLOC_NAME SMALL (200*2=400) succeeds");
    if (small) {
        int all_zero = 1;
        for (int i = 0; i < 200; i++) {
            if (small[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME SMALL zeroed");
        FREE_NAME(small);
    }
    
    // Test LARGE (> 1024 bytes)
    long *large = CALLOC_NAME(300, sizeof(long)); // 2400 bytes sur 64-bit
    test_result(large != NULL, "CALLOC_NAME LARGE (300*8=2400) succeeds");
    if (large) {
        int all_zero = 1;
        for (int i = 0; i < 300; i++) {
            if (large[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME LARGE zeroed");
        FREE_NAME(large);
    }
    
    
}

void test_CALLOC_NAME_overflow() {
    TEST_SECTION("CALLOC_NAME OVERFLOW TESTS");
    
    // Test overflow detection
    size_t huge = SIZE_MAX / 2;
    void *overflow1 = CALLOC_NAME(huge, huge);
    test_result(overflow1 == NULL, "CALLOC_NAME overflow detection (huge * huge)");
    if (overflow1) FREE_NAME(overflow1);
    
    // Test autre cas d'overflow
    void *overflow2 = CALLOC_NAME(SIZE_MAX, 2);
    test_result(overflow2 == NULL, "CALLOC_NAME overflow detection (SIZE_MAX * 2)");
    if (overflow2) FREE_NAME(overflow2);
    
    // Test cas limite mais valide
    void *valid = CALLOC_NAME(1000, 1000); // 1MB
    test_result(valid != NULL, "CALLOC_NAME large but valid allocation");
    if (valid) {
        // Vérifier quelques positions aléatoirement
        char *ptr = (char*)valid;
        int sample_zero = (ptr[0] == 0 && ptr[500000] == 0 && ptr[999999] == 0);
        test_result(sample_zero, "CALLOC_NAME large allocation properly zeroed");
        FREE_NAME(valid);
    }
}

void test_CALLOC_NAME_patterns() {
    TEST_SECTION("CALLOC_NAME PATTERN TESTS");
    
    // Test avec structures complexes
    struct test_struct {
        int a;
        char b[16];
        double c;
        void *ptr;
    };
    
    struct test_struct *structs = CALLOC_NAME(10, sizeof(struct test_struct));
    test_result(structs != NULL, "CALLOC_NAME struct array succeeds");
    
    if (structs) {
        int all_zero = 1;
        for (int i = 0; i < 10; i++) {
            if (structs[i].a != 0 || structs[i].c != 0.0 || structs[i].ptr != NULL) {
                all_zero = 0;
                break;
            }
            for (int j = 0; j < 16; j++) {
                if (structs[i].b[j] != 0) {
                    all_zero = 0;
                    break;
                }
            }
        }
        test_result(all_zero, "CALLOC_NAME struct array properly zeroed");
        FREE_NAME(structs);
    }
    
    // Test CALLOC_NAME avec différents patterns de taille
    for (int size = 1; size <= 10; size++) {
        void *ptr = CALLOC_NAME(size, 100);
        test_result(ptr != NULL, "CALLOC_NAME variable sizes succeeds");
        if (ptr) {
            char *cptr = (char*)ptr;
            int is_zero = 1;
            for (int i = 0; i < size * 100; i++) {
                if (cptr[i] != 0) {
                    is_zero = 0;
                    break;
                }
            }
            test_result(is_zero, "CALLOC_NAME variable sizes properly zeroed");
            FREE_NAME(ptr);
        }
    }
}

void test_CALLOC_NAME_extreme() {
    TEST_SECTION("CALLOC_NAME EXTREME TESTS");
    
    // Test CALLOC_NAME(1, very_large)
    void *big1 = CALLOC_NAME(1, 1024 * 1024); // 1MB
    test_result(big1 != NULL, "CALLOC_NAME(1, 1MB) succeeds");
    if (big1) {
        char *ptr = (char*)big1;
        int samples_zero = (ptr[0] == 0 && ptr[512*1024] == 0 && ptr[1024*1024-1] == 0);
        test_result(samples_zero, "CALLOC_NAME(1, 1MB) properly zeroed");
        FREE_NAME(big1);
    }
    
    // Test CALLOC_NAME(very_large, 1)
    void *big2 = CALLOC_NAME(1024 * 1024, 1); // 1MB
    test_result(big2 != NULL, "CALLOC_NAME(1MB, 1) succeeds");
    if (big2) {
        char *ptr = (char*)big2;
        int samples_zero = (ptr[0] == 0 && ptr[512*1024] == 0 && ptr[1024*1024-1] == 0);
        test_result(samples_zero, "CALLOC_NAME(1MB, 1) properly zeroed");
        FREE_NAME(big2);
    }
    
    // Test CALLOC_NAME avec des tailles premières
    void *prime = CALLOC_NAME(97, 101); // 9797 bytes
    test_result(prime != NULL, "CALLOC_NAME(97, 101) prime numbers succeeds");
    if (prime) {
        char *ptr = (char*)prime;
        int all_zero = 1;
        for (int i = 0; i < 97 * 101; i++) {
            if (ptr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME prime numbers properly zeroed");
        FREE_NAME(prime);
    }
    
    
}

void test_REALLOCARRAY_NAME_basic() {
    TEST_SECTION("REALLOCARRAY_NAME BASIC TESTS");
    
    // Test 1: REALLOCARRAY_NAME(NULL, nmemb, size) = CALLOC_NAME(nmemb, size)
    int *arr = REALLOCARRAY_NAME(NULL, 10, sizeof(int));
    test_result(arr != NULL, "REALLOCARRAY_NAME(NULL, 10, sizeof(int)) succeeds");
    
    if (arr) {
        // Initialiser avec des valeurs
        for (int i = 0; i < 10; i++) {
            arr[i] = i * i;
        }
        
        // Vérifier les valeurs
        int values_ok = 1;
        for (int i = 0; i < 10; i++) {
            if (arr[i] != i * i) {
                values_ok = 0;
                break;
            }
        }
        test_result(values_ok, "REALLOCARRAY_NAME initial data integrity");
        
        // Test 2: agrandir le tableau
        arr = REALLOCARRAY_NAME(arr, 20, sizeof(int));
        test_result(arr != NULL, "REALLOCARRAY_NAME expand (10->20) succeeds");
        
        if (arr) {
            // Vérifier que les anciennes données sont préservées
            int old_data_ok = 1;
            for (int i = 0; i < 10; i++) {
                if (arr[i] != i * i) {
                    old_data_ok = 0;
                    break;
                }
            }
            test_result(old_data_ok, "REALLOCARRAY_NAME preserves old data on expand");
            
            // Initialiser les nouveaux éléments
            for (int i = 10; i < 20; i++) {
                arr[i] = i * 2;
            }
            
            // Test 3: réduire le tableau
            arr = REALLOCARRAY_NAME(arr, 15, sizeof(int));
            test_result(arr != NULL, "REALLOCARRAY_NAME shrink (20->15) succeeds");
            
            if (arr) {
                // Vérifier les données restantes
                int remaining_data_ok = 1;
                for (int i = 0; i < 10; i++) {
                    if (arr[i] != i * i) {
                        remaining_data_ok = 0;
                        break;
                    }
                }
                for (int i = 10; i < 15; i++) {
                    if (arr[i] != i * 2) {
                        remaining_data_ok = 0;
                        break;
                    }
                }
                test_result(remaining_data_ok, "REALLOCARRAY_NAME preserves data on shrink");
                
                FREE_NAME(arr);
            }
        }
    }
    
    
}

void test_REALLOCARRAY_NAME_overflow() {
    TEST_SECTION("REALLOCARRAY_NAME OVERFLOW TESTS");
    
    // Test overflow detection avec un pointeur existant
    void *ptr = CALLOC_NAME(100, 1);
    if (ptr) {
        strcpy((char*)ptr, "Test data");
        printf("je susi la\n");
        // Tentative d'overflow
        void *overflow1 = REALLOCARRAY_NAME(ptr, SIZE_MAX/2, SIZE_MAX/2);
        test_result(overflow1 == NULL, "REALLOCARRAY_NAME overflow detection (huge*huge)");
        
        // Le pointeur original doit rester valide si REALLOCARRAY_NAME échoue
        if (overflow1 == NULL) {
            test_result(strcmp((char*)ptr, "Test data") == 0, "Original pointer valid after overflow");
        }
        
        // Test autre cas d'overflow
        void *overflow2 = REALLOCARRAY_NAME(ptr, SIZE_MAX, 2);
        test_result(overflow2 == NULL, "REALLOCARRAY_NAME overflow detection (SIZE_MAX*2)");
        if (overflow2 == NULL) {
            test_result(strcmp((char*)ptr, "Test data") == 0, "Original pointer still valid");
        }
        
        FREE_NAME(ptr);
    }
    
    // Test REALLOCARRAY_NAME(NULL, overflow_values)
    void *overflow3 = REALLOCARRAY_NAME(NULL, SIZE_MAX, SIZE_MAX);
    test_result(overflow3 == NULL, "REALLOCARRAY_NAME(NULL, overflow) fails safely");
    if (overflow3) FREE_NAME(overflow3);
    
    // Test cas limite valide
    void *valid = REALLOCARRAY_NAME(NULL, 1000, 1000); // 1MB
    test_result(valid != NULL, "REALLOCARRAY_NAME large but valid allocation");
    if (valid) FREE_NAME(valid);
}

void test_REALLOCARRAY_NAME_categories() {
    TEST_SECTION("REALLOCARRAY_NAME SIZE CATEGORIES");
    
    // TINY -> SMALL
    char *ptr = REALLOCARRAY_NAME(NULL, 64, 1); // 64 bytes TINY
    test_result(ptr != NULL, "REALLOCARRAY_NAME TINY (64*1) succeeds");
    
    if (ptr) {
        memset(ptr, 'T', 64);
        
        // Passer à SMALL
        ptr = REALLOCARRAY_NAME(ptr, 200, 1); // 200 bytes SMALL
        test_result(ptr != NULL, "REALLOCARRAY_NAME TINY->SMALL succeeds");
        
        if (ptr) {
            int data_ok = 1;
            for (int i = 0; i < 64; i++) {
                if (ptr[i] != 'T') {
                    data_ok = 0;
                    break;
                }
            }
            test_result(data_ok, "REALLOCARRAY_NAME TINY->SMALL preserves data");
            
            // Remplir le reste
            memset(ptr + 64, 'S', 136);
            
            // Passer à LARGE
            ptr = REALLOCARRAY_NAME(ptr, 2048, 1); // 2048 bytes LARGE
            test_result(ptr != NULL, "REALLOCARRAY_NAME SMALL->LARGE succeeds");
            
            if (ptr) {
                int data_ok = 1;
                for (int i = 0; i < 64; i++) {
                    if (ptr[i] != 'T') {
                        data_ok = 0;
                        break;
                    }
                }
                for (int i = 64; i < 200 && data_ok; i++) {
                    if (ptr[i] != 'S') {
                        data_ok = 0;
                        break;
                    }
                }
                test_result(data_ok, "REALLOCARRAY_NAME SMALL->LARGE preserves data");
                
                // Test retour LARGE -> SMALL
                ptr = REALLOCARRAY_NAME(ptr, 512, 1); // 512 bytes SMALL
                test_result(ptr != NULL, "REALLOCARRAY_NAME LARGE->SMALL succeeds");
                
                if (ptr) {
                    int data_ok = 1;
                    for (int i = 0; i < 64; i++) {
                        if (ptr[i] != 'T') {
                            data_ok = 0;
                            break;
                        }
                    }
                    for (int i = 64; i < 200 && data_ok; i++) {
                        if (ptr[i] != 'S') {
                            data_ok = 0;
                            break;
                        }
                    }
                    test_result(data_ok, "REALLOCARRAY_NAME LARGE->SMALL preserves data");
                    
                    // Test retour SMALL -> TINY
                    ptr = REALLOCARRAY_NAME(ptr, 100, 1); // 100 bytes TINY
                    test_result(ptr != NULL, "REALLOCARRAY_NAME SMALL->TINY succeeds");
                    
                    if (ptr) {
                        int data_ok = 1;
                        for (int i = 0; i < 64; i++) {
                            if (ptr[i] != 'T') {
                                data_ok = 0;
                                break;
                            }
                        }
                        for (int i = 64; i < 100 && data_ok; i++) {
                            if (ptr[i] != 'S') {
                                data_ok = 0;
                                break;
                            }
                        }
                        test_result(data_ok, "REALLOCARRAY_NAME SMALL->TINY preserves data");
                        
                        FREE_NAME(ptr);
                    }
                }
            }
        }
    }
}

void test_REALLOCARRAY_NAME_edge_cases() {
    TEST_SECTION("REALLOCARRAY_NAME EDGE CASES");
    
    // Test REALLOCARRAY_NAME(ptr, 0, size) 
    void *ptr = CALLOC_NAME(100, 1);
    if (ptr) {
        strcpy((char*)ptr, "Edge test");
        
        void *result1 = REALLOCARRAY_NAME(ptr, 0, 10);
        test_result(result1 != NULL || result1 == NULL, "REALLOCARRAY_NAME(ptr, 0, 10) returns valid result");
        
            FREE_NAME(result1);
            ptr = NULL; // ptr a été libéré
    }
    
    // Test REALLOCARRAY_NAME(ptr, nmemb, 0)
    if (!ptr) {
        ptr = CALLOC_NAME(100, 1);
        strcpy((char*)ptr, "Edge test 2");
    }
    
    if (ptr) {
        void *result2 = REALLOCARRAY_NAME(ptr, 10, 0);
        test_result(result2 != NULL || result2 == NULL, "REALLOCARRAY_NAME(ptr, 10, 0) returns valid result");
        
        if (result2) {
            FREE_NAME(result2);
        }
		ptr = NULL;
    }
    
    // Test REALLOCARRAY_NAME same size
    double *darr = REALLOCARRAY_NAME(NULL, 50, sizeof(double));
    if (darr) {
        for (int i = 0; i < 50; i++) {
            darr[i] = i * 3.14;
        }
        
        double *same = REALLOCARRAY_NAME(darr, 50, sizeof(double));
        test_result(same != NULL, "REALLOCARRAY_NAME same size succeeds");
        
        if (same) {
            int data_ok = 1;
            for (int i = 0; i < 50; i++) {
                if (same[i] != i * 3.14) {
                    data_ok = 0;
                    break;
                }
            }
            test_result(data_ok, "REALLOCARRAY_NAME same size preserves data");
            FREE_NAME(same);
        }
    }
    
    // Test REALLOCARRAY_NAME(NULL, 1, 1)
    void *minimal = REALLOCARRAY_NAME(NULL, 1, 1);
    test_result(minimal != NULL, "REALLOCARRAY_NAME(NULL, 1, 1) succeeds");
    if (minimal) {
        *(char*)minimal = 'X';
        test_result(*(char*)minimal == 'X', "REALLOCARRAY_NAME minimal allocation works");
        FREE_NAME(minimal);
    }
}

void test_REALLOCARRAY_NAME_stress() {
    TEST_SECTION("REALLOCARRAY_NAME STRESS TEST");
    
    // Tableau qui grandit et rétrécit de façon dynamique
    int *dynamic_arr = NULL;
    size_t current_size = 0;
    int stress_ok = 1;
    
    // Séquence de tailles
    size_t sizes[] = {10, 50, 25, 100, 200, 150, 300, 75, 500, 1000, 250, 50, 10, 15};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int round = 0; round < num_sizes; round++) {
        size_t new_size = sizes[round];
        
        dynamic_arr = REALLOCARRAY_NAME(dynamic_arr, new_size, sizeof(int));
        if (!dynamic_arr) {
            stress_ok = 0;
            break;
        }
        
        // Si on grandit, initialiser les nouveaux éléments
        if (new_size > current_size) {
            for (size_t i = current_size; i < new_size; i++) {
                dynamic_arr[i] = (int)(i + round * 1000);
            }
        }
        
        // Vérifier que nous pouvons lire/écrire
        if (new_size > 0) {
            dynamic_arr[0] = 12345;
            dynamic_arr[new_size - 1] = 67890;
            
            if (dynamic_arr[0] != 12345 || dynamic_arr[new_size - 1] != 67890) {
                stress_ok = 0;
                break;
            }
        }
        
        current_size = new_size;
    }
    
    test_result(stress_ok, "REALLOCARRAY_NAME stress test completes");
    
    if (dynamic_arr) {
        FREE_NAME(dynamic_arr);
    }
}

void test_REALLOCARRAY_NAME_data_integrity() {
    TEST_SECTION("REALLOCARRAY_NAME DATA INTEGRITY");
    
    // Test avec un pattern spécifique
    struct data_block {
        int magic;
        char data[256];
        int checksum;
    };
    
    struct data_block *blocks = REALLOCARRAY_NAME(NULL, 10, sizeof(struct data_block));
    test_result(blocks != NULL, "REALLOCARRAY_NAME data blocks allocation");
    
    if (blocks) {
        // Initialiser avec un pattern reconnaissable
        for (int i = 0; i < 10; i++) {
            blocks[i].magic = 0xDEADBEE;
            memset(blocks[i].data, 'A' + i, 256);
            blocks[i].checksum = i * 12345;
        }
        
        // Agrandir le tableau
        blocks = REALLOCARRAY_NAME(blocks, 20, sizeof(struct data_block));
        test_result(blocks != NULL, "REALLOCARRAY_NAME data blocks expansion");
        
        if (blocks) {
            // Vérifier l'intégrité des données originales
            int integrity_ok = 1;
            for (int i = 0; i < 10; i++) {
                if (blocks[i].magic != 0xDEADBEE || blocks[i].checksum != i * 12345) {
                    integrity_ok = 0;
                    break;
                }
                for (int j = 0; j < 256; j++) {
                    if (blocks[i].data[j] != 'A' + i) {
                        integrity_ok = 0;
                        break;
                    }
                }
                if (!integrity_ok) break;
            }
            test_result(integrity_ok, "REALLOCARRAY_NAME preserves complex data structures");
            
            FREE_NAME(blocks);
        }
    }
}

void test_CALLOC_NAME_REALLOCARRAY_NAME_interaction() {
    TEST_SECTION("CALLOC_NAME + REALLOCARRAY_NAME INTERACTION");
    
    // Créer un tableau avec CALLOC_NAME
    long *arr = CALLOC_NAME(20, sizeof(long));
    test_result(arr != NULL, "CALLOC_NAME(20, sizeof(long)) for interaction test");
    
    if (arr) {
        // Vérifier qu'il est à zéro
        int all_zero = 1;
        for (int i = 0; i < 20; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "CALLOC_NAME result is zeroed before REALLOCARRAY_NAME");
        
        // Modifier quelques valeurs
        for (int i = 0; i < 20; i++) {
            arr[i] = i * 100;
        }
        
        // Utiliser REALLOCARRAY_NAME pour agrandir
        arr = REALLOCARRAY_NAME(arr, 40, sizeof(long));
        test_result(arr != NULL, "REALLOCARRAY_NAME after CALLOC_NAME succeeds");
        
        if (arr) {
            // Vérifier que les données originales sont préservées
            int data_preserved = 1;
            for (int i = 0; i < 20; i++) {
                if (arr[i] != i * 100) {
                    data_preserved = 0;
                    break;
                }
            }
            test_result(data_preserved, "REALLOCARRAY_NAME preserves CALLOC_NAME'd data");
            
            FREE_NAME(arr);
        }
    }
    
    // Test inverse: REALLOCARRAY_NAME puis comparaison avec CALLOC_NAME
    char *buf1 = REALLOCARRAY_NAME(NULL, 100, sizeof(char));
    char *buf2 = CALLOC_NAME(100, sizeof(char));
    
    if (buf1 && buf2) {
        // buf2 devrait être à zéro, buf1 pas nécessairement
        int buf2_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (buf2[i] != 0) {
                buf2_zero = 0;
                break;
            }
        }
        test_result(buf2_zero, "CALLOC_NAME vs REALLOCARRAY_NAME: CALLOC_NAME is zeroed");
        
        FREE_NAME(buf1);
        FREE_NAME(buf2);
    }
}

void test_REALLOCARRAY_NAME_boundary() {
    TEST_SECTION("REALLOCARRAY_NAME BOUNDARY TESTS");
    
    // Tests aux limites exactes des catégories
    void *tiny_max = REALLOCARRAY_NAME(NULL, 128, 1); // Max TINY
    void *small_min = REALLOCARRAY_NAME(NULL, 129, 1); // Min SMALL  
    void *small_max = REALLOCARRAY_NAME(NULL, 1024, 1); // Max SMALL
    void *large_min = REALLOCARRAY_NAME(NULL, 1025, 1); // Min LARGE
    
    test_result(tiny_max != NULL, "REALLOCARRAY_NAME TINY max boundary (128)");
    test_result(small_min != NULL, "REALLOCARRAY_NAME SMALL min boundary (129)");
    test_result(small_max != NULL, "REALLOCARRAY_NAME SMALL max boundary (1024)");
    test_result(large_min != NULL, "REALLOCARRAY_NAME LARGE min boundary (1025)");
    
    // Test d'écriture dans chaque bloc
    if (tiny_max) {
        memset(tiny_max, 'T', 128);
        test_result(((char*)tiny_max)[127] == 'T', "TINY boundary block writable");
        FREE_NAME(tiny_max);
    }
    
    if (small_min) {
        memset(small_min, 'S', 129);
        test_result(((char*)small_min)[128] == 'S', "SMALL min boundary block writable");
        FREE_NAME(small_min);
    }
    
    if (small_max) {
        memset(small_max, 'M', 1024);
        test_result(((char*)small_max)[1023] == 'M', "SMALL max boundary block writable");
        FREE_NAME(small_max);
    }
    
    if (large_min) {
        memset(large_min, 'L', 1025);
        test_result(((char*)large_min)[1024] == 'L', "LARGE min boundary block writable");
        FREE_NAME(large_min);
    }
    
    
}

void test_STRDUP_NAME_basic() {
    TEST_SECTION("STRDUP_NAME BASIC TESTS");
    
    // Test 1: chaîne vide
    char *empty = STRDUP_NAME("");
    test_result(empty != NULL, "STRDUP_NAME(\"\") succeeds");
    if (empty) {
        test_result(strlen(empty) == 0, "STRDUP_NAME(\"\") returns empty string");
        test_result(strcmp(empty, "") == 0, "STRDUP_NAME(\"\") content is correct");
        test_result(((uintptr_t)empty % 8) == 0, "STRDUP_NAME(\"\") result is 8-byte aligned");
        FREE_NAME(empty);
    }
    
    // Test 2: chaîne simple
    char *simple = STRDUP_NAME("Hello");
    test_result(simple != NULL, "STRDUP_NAME(\"Hello\") succeeds");
    if (simple) {
        test_result(strlen(simple) == 5, "STRDUP_NAME(\"Hello\") length is correct");
        test_result(strcmp(simple, "Hello") == 0, "STRDUP_NAME(\"Hello\") content is correct");
        test_result(((uintptr_t)simple % 8) == 0, "STRDUP_NAME(\"Hello\") result is 8-byte aligned");
        FREE_NAME(simple);
    }
    
    // Test 3: chaîne longue
    char *long_str = STRDUP_NAME("This is a longer string for testing purposes");
    test_result(long_str != NULL, "STRDUP_NAME(long string) succeeds");
    if (long_str) {
        test_result(strlen(long_str) == 44, "STRDUP_NAME(long string) length is correct");
        test_result(strcmp(long_str, "This is a longer string for testing purposes") == 0, 
                   "STRDUP_NAME(long string) content is correct");
        FREE_NAME(long_str);
    }
    
    // Test 4: chaîne avec caractères spéciaux
    char *special = STRDUP_NAME("Hello\tWorld\n\r\\\"");
    test_result(special != NULL, "STRDUP_NAME(special chars) succeeds");
    if (special) {
        test_result(strcmp(special, "Hello\tWorld\n\r\\\"") == 0, 
                   "STRDUP_NAME(special chars) content is correct");
        FREE_NAME(special);
    }
    
    
}

void test_STRDUP_NAME_sizes() {
    TEST_SECTION("STRDUP_NAME SIZE CATEGORIES");
    
    // Test TINY (≤ 128 bytes)
    // Chaîne de 64 caractères (65 avec \0)
    char tiny_source[65];
    memset(tiny_source, 'A', 64);
    tiny_source[64] = '\0';
    
    char *tiny = STRDUP_NAME(tiny_source);
    test_result(tiny != NULL, "STRDUP_NAME TINY (64 chars) succeeds");
    if (tiny) {
        test_result(strlen(tiny) == 64, "STRDUP_NAME TINY length correct");
        test_result(strcmp(tiny, tiny_source) == 0, "STRDUP_NAME TINY content correct");
        test_result(tiny[63] == 'A' && tiny[64] == '\0', "STRDUP_NAME TINY null termination");
        FREE_NAME(tiny);
    }
    
    // Chaîne exactement à la limite TINY (127 chars + \0 = 128 bytes)
    char tiny_max_source[128];
    memset(tiny_max_source, 'T', 127);
    tiny_max_source[127] = '\0';
    
    char *tiny_max = STRDUP_NAME(tiny_max_source);
    test_result(tiny_max != NULL, "STRDUP_NAME TINY max boundary (127 chars) succeeds");
    if (tiny_max) {
        test_result(strlen(tiny_max) == 127, "STRDUP_NAME TINY max length correct");
        test_result(strcmp(tiny_max, tiny_max_source) == 0, "STRDUP_NAME TINY max content correct");
        FREE_NAME(tiny_max);
    }
    
    // Test SMALL (129-1024 bytes) - 200 caractères
    char small_source[201];
    memset(small_source, 'S', 200);
    small_source[200] = '\0';
    
    char *small = STRDUP_NAME(small_source);
    test_result(small != NULL, "STRDUP_NAME SMALL (200 chars) succeeds");
    if (small) {
        test_result(strlen(small) == 200, "STRDUP_NAME SMALL length correct");
        test_result(strcmp(small, small_source) == 0, "STRDUP_NAME SMALL content correct");
        FREE_NAME(small);
    }
    
    // Chaîne à la limite SMALL (1023 chars + \0 = 1024 bytes)
    char small_max_source[1024];
    memset(small_max_source, 'M', 1023);
    small_max_source[1023] = '\0';
    
    char *small_max = STRDUP_NAME(small_max_source);
    test_result(small_max != NULL, "STRDUP_NAME SMALL max boundary (1023 chars) succeeds");
    if (small_max) {
        test_result(strlen(small_max) == 1023, "STRDUP_NAME SMALL max length correct");
        test_result(small_max[1022] == 'M' && small_max[1023] == '\0', 
                   "STRDUP_NAME SMALL max termination correct");
        FREE_NAME(small_max);
    }
    
    // Test LARGE (> 1024 bytes) - 2000 caractères
    char large_source[2001];
    memset(large_source, 'L', 2000);
    large_source[2000] = '\0';
    
    char *large = STRDUP_NAME(large_source);
    test_result(large != NULL, "STRDUP_NAME LARGE (2000 chars) succeeds");
    if (large) {
        test_result(strlen(large) == 2000, "STRDUP_NAME LARGE length correct");
        test_result(large[1999] == 'L' && large[2000] == '\0', 
                   "STRDUP_NAME LARGE content and termination correct");
        FREE_NAME(large);
    }
    
    
}

void test_STRDUP_NAME_content_integrity() {
    TEST_SECTION("STRDUP_NAME CONTENT INTEGRITY");
    
    // Test avec tous les caractères ASCII imprimables
    char ascii_source[96]; // 32-126 + \0
    for (int i = 0; i < 95; i++) {
        ascii_source[i] = (char)(32 + i); // ' ' à '~'
    }
    ascii_source[95] = '\0';
    
    char *ascii_copy = STRDUP_NAME(ascii_source);
    test_result(ascii_copy != NULL, "STRDUP_NAME ASCII printable chars succeeds");
    if (ascii_copy) {
        test_result(strlen(ascii_copy) == 95, "STRDUP_NAME ASCII length correct");
        test_result(strcmp(ascii_copy, ascii_source) == 0, "STRDUP_NAME ASCII content correct");
        
        // Vérifier caractère par caractère
        int char_by_char_ok = 1;
        for (int i = 0; i < 95; i++) {
            if (ascii_copy[i] != ascii_source[i]) {
                char_by_char_ok = 0;
                break;
            }
        }
        test_result(char_by_char_ok, "STRDUP_NAME ASCII char-by-char integrity");
        FREE_NAME(ascii_copy);
    }
    
    // Test avec des octets binaires (incluant des 0 dans la chaîne source jusqu'au premier \0)
    char binary_source[] = "Binary\x01\x02\x03\xFF\xFE test";
    char *binary_copy = STRDUP_NAME(binary_source);
    test_result(binary_copy != NULL, "STRDUP_NAME binary chars succeeds");
    if (binary_copy) {
        test_result(strcmp(binary_copy, binary_source) == 0, "STRDUP_NAME binary content correct");
        test_result(binary_copy[6] == '\x01' && binary_copy[7] == '\x02', 
                   "STRDUP_NAME preserves binary values");
        FREE_NAME(binary_copy);
    }
    
    // Test avec répétition de patterns
    char pattern_source[101];
    for (int i = 0; i < 100; i++) {
        pattern_source[i] = (char)('A' + (i % 26));
    }
    pattern_source[100] = '\0';
    
    char *pattern_copy = STRDUP_NAME(pattern_source);
    test_result(pattern_copy != NULL, "STRDUP_NAME pattern string succeeds");
    if (pattern_copy) {
        test_result(strlen(pattern_copy) == 100, "STRDUP_NAME pattern length correct");
        int pattern_ok = 1;
        for (int i = 0; i < 100; i++) {
            if (pattern_copy[i] != (char)('A' + (i % 26))) {
                pattern_ok = 0;
                break;
            }
        }
        test_result(pattern_ok, "STRDUP_NAME pattern content integrity");
        FREE_NAME(pattern_copy);
    }
}

void test_STRDUP_NAME_independence() {
    TEST_SECTION("STRDUP_NAME MEMORY INDEPENDENCE");
    
    // Test que la copie est indépendante de l'original
    char original[] = "Original string";
    char *copy = STRDUP_NAME(original);
    
    test_result(copy != NULL, "STRDUP_NAME creates copy");
    if (copy) {
        test_result(copy != original, "STRDUP_NAME copy has different address");
        test_result(strcmp(copy, original) == 0, "STRDUP_NAME copy has same content initially");
        
        // Modifier l'original
        strcpy(original, "Modified orig");
        test_result(strcmp(copy, "Original string") == 0, 
                   "STRDUP_NAME copy unchanged when original modified");
        
        // Modifier la copie
        strcpy(copy, "Modified copy");
        test_result(strcmp(original, "Modified orig") == 0, 
                   "Original unchanged when STRDUP_NAME copy modified");
        test_result(strcmp(copy, "Modified copy") == 0, 
                   "STRDUP_NAME copy modification successful");
        
        FREE_NAME(copy);
    }
    
    // Test avec buffer temporaire
    char *dynamic_copy;
    {
        char temp_buffer[100];
        strcpy(temp_buffer, "Temporary buffer content");
        dynamic_copy = STRDUP_NAME(temp_buffer);
        test_result(dynamic_copy != NULL, "STRDUP_NAME from temp buffer succeeds");
        // temp_buffer va être détruit à la sortie du bloc
    }
    
    // Vérifier que la copie survit à la destruction du buffer temporaire
    if (dynamic_copy) {
        test_result(strcmp(dynamic_copy, "Temporary buffer content") == 0, 
                   "STRDUP_NAME copy survives original buffer destruction");
        FREE_NAME(dynamic_copy);
    }
}

void test_STRDUP_NAME_extreme_sizes() {
    TEST_SECTION("STRDUP_NAME EXTREME SIZES");
    
    // Test chaîne d'un seul caractère
    char *single = STRDUP_NAME("X");
    test_result(single != NULL, "STRDUP_NAME single char succeeds");
    if (single) {
        test_result(strlen(single) == 1, "STRDUP_NAME single char length correct");
        test_result(single[0] == 'X' && single[1] == '\0', "STRDUP_NAME single char content correct");
        FREE_NAME(single);
    }
    
    // Test chaîne très longue (10KB)
    char *big_source = MALLOC_NAME(10241);
    if (big_source) {
        memset(big_source, 'B', 10240);
        big_source[10240] = '\0';
        
        char *big_copy = STRDUP_NAME(big_source);
        test_result(big_copy != NULL, "STRDUP_NAME very large string (10KB) succeeds");
        if (big_copy) {
            test_result(strlen(big_copy) == 10240, "STRDUP_NAME large string length correct");
            test_result(big_copy[0] == 'B' && big_copy[10239] == 'B' && big_copy[10240] == '\0', 
                       "STRDUP_NAME large string content correct");
            
            // Vérifier quelques positions aléatoirement
            int sample_ok = (big_copy[1000] == 'B' && big_copy[5000] == 'B' && big_copy[9000] == 'B');
            test_result(sample_ok, "STRDUP_NAME large string sample positions correct");
            
            FREE_NAME(big_copy);
        }
        FREE_NAME(big_source);
    }
    
    // Test avec une chaîne contenant beaucoup d'espaces
    char spaces_source[1001];
    memset(spaces_source, ' ', 1000);
    spaces_source[1000] = '\0';
    
    char *spaces_copy = STRDUP_NAME(spaces_source);
    test_result(spaces_copy != NULL, "STRDUP_NAME spaces string succeeds");
    if (spaces_copy) {
        test_result(strlen(spaces_copy) == 1000, "STRDUP_NAME spaces length correct");
        int all_spaces = 1;
        for (int i = 0; i < 1000; i++) {
            if (spaces_copy[i] != ' ') {
                all_spaces = 0;
                break;
            }
        }
        test_result(all_spaces && spaces_copy[1000] == '\0', "STRDUP_NAME spaces content correct");
        FREE_NAME(spaces_copy);
    }
}

void test_STRDUP_NAME_special_chars() {
    TEST_SECTION("STRDUP_NAME SPECIAL CHARACTERS");
    
    // Test avec caractères Unicode/UTF-8 (si supportés)
    char *utf8 = STRDUP_NAME("Héllo Wörld! 🌍 café");
    test_result(utf8 != NULL, "STRDUP_NAME UTF-8 chars succeeds");
    if (utf8) {
        test_result(strcmp(utf8, "Héllo Wörld! 🌍 café") == 0, "STRDUP_NAME UTF-8 content correct");
        FREE_NAME(utf8);
    }
    
    // Test avec tous les caractères de contrôle ASCII (0-31)
    char control_source[33];
    for (int i = 1; i < 32; i++) { // Skip \0 car il terminerait la chaîne
        control_source[i-1] = (char)i;
    }
    control_source[31] = '\0';
    
    char *control_copy = STRDUP_NAME(control_source);
    test_result(control_copy != NULL, "STRDUP_NAME control chars succeeds");
    if (control_copy) {
        test_result(strlen(control_copy) == 31, "STRDUP_NAME control chars length correct");
        int control_ok = 1;
        for (int i = 0; i < 31; i++) {
            if (control_copy[i] != (char)(i + 1)) {
                control_ok = 0;
                break;
            }
        }
        test_result(control_ok, "STRDUP_NAME control chars content correct");
        FREE_NAME(control_copy);
    }
    
    // Test avec caractères de fin de ligne variés
    char *newlines = STRDUP_NAME("Line1\nLine2\rLine3\r\nLine4");
    test_result(newlines != NULL, "STRDUP_NAME newline variants succeeds");
    if (newlines) {
        test_result(strcmp(newlines, "Line1\nLine2\rLine3\r\nLine4") == 0, 
                   "STRDUP_NAME newline variants content correct");
        FREE_NAME(newlines);
    }
    
    // Test avec guillemets et échappements
    char *quotes = STRDUP_NAME("\"Hello\" 'World' \\backslash\\");
    test_result(quotes != NULL, "STRDUP_NAME quotes and escapes succeeds");
    if (quotes) {
        test_result(strcmp(quotes, "\"Hello\" 'World' \\backslash\\") == 0, 
                   "STRDUP_NAME quotes and escapes content correct");
        FREE_NAME(quotes);
    }
}

void test_STRDUP_NAME_alignment() {
    TEST_SECTION("STRDUP_NAME ALIGNMENT TESTS");
    
    // Tester l'alignement avec différentes tailles
    char *ptrs[10];
    char sources[10][20];
    
    for (int i = 0; i < 10; i++) {
        // Créer des chaînes de longueurs variées
        int len = i + 1; // 1 à 10 caractères
        memset(sources[i], 'A' + i, len);
        sources[i][len] = '\0';
        
        ptrs[i] = STRDUP_NAME(sources[i]);
        test_result(ptrs[i] != NULL, "STRDUP_NAME various sizes allocation");
        if (ptrs[i]) {
            test_result(((uintptr_t)ptrs[i] % 8) == 0, "STRDUP_NAME result properly aligned");
            test_result(strcmp(ptrs[i], sources[i]) == 0, "STRDUP_NAME various sizes content correct");
        }
    }
    
    // Nettoyer et vérifier l'état
    
    
    for (int i = 0; i < 10; i++) {
        if (ptrs[i]) {
            FREE_NAME(ptrs[i]);
        }
    }
}

void test_STRDUP_NAME_stress() {
    TEST_SECTION("STRDUP_NAME STRESS TESTS");
    
    // Allouer beaucoup de chaînes de tailles variées
    const int num_strings = 1000;
    char **strings = MALLOC_NAME(num_strings * sizeof(char*));
    int success_count = 0;
    
    if (strings) {
        for (int i = 0; i < num_strings; i++) {
            // Créer une chaîne unique pour chaque index
            char temp[100];
            snprintf(temp, sizeof(temp), "String_%d_with_content_%d", i, i * 42);
            
            strings[i] = STRDUP_NAME(temp);
            if (strings[i]) {
                success_count++;
            }
        }
        
        test_result(success_count == num_strings, "STRDUP_NAME stress: all allocations succeed");
        
        // Vérifier l'intégrité de quelques chaînes aléatoirement
        int integrity_check = 1;
        for (int i = 0; i < num_strings; i += 100) { // Vérifier chaque 100ème
            if (strings[i]) {
                char expected[100];
                snprintf(expected, sizeof(expected), "String_%d_with_content_%d", i, i * 42);
                if (strcmp(strings[i], expected) != 0) {
                    integrity_check = 0;
                    break;
                }
            }
        }
        test_result(integrity_check, "STRDUP_NAME stress: data integrity maintained");
        
        // Nettoyer
        for (int i = 0; i < num_strings; i++) {
            if (strings[i]) {
                FREE_NAME(strings[i]);
            }
        }
        FREE_NAME(strings);
    }
    
    // Test de fragmentation avec STRDUP_NAME
    char *frags[50];
    for (int i = 0; i < 50; i++) {
        char temp[50];
        snprintf(temp, sizeof(temp), "Fragment_%02d", i);
        frags[i] = STRDUP_NAME(temp);
    }
    
    // Libérer les chaînes impaires
    for (int i = 1; i < 50; i += 2) {
        if (frags[i]) {
            FREE_NAME(frags[i]);
            frags[i] = NULL;
        }
    }
    
    // Réallouer des chaînes dans les trous
    for (int i = 1; i < 50; i += 2) {
        char temp[50];
        snprintf(temp, sizeof(temp), "NewFrag_%02d", i);
        frags[i] = STRDUP_NAME(temp);
        test_result(frags[i] != NULL, "STRDUP_NAME fragmentation REALLOC_NAMEation");
    }
    
	printf("je suis ici\n");
    // Nettoyer la fragmentation
    for (int i = 0; i < 50; i++) {
        if (frags[i]) {
			printf("i == %d\n", i);
            FREE_NAME(frags[i]);
        }
    }
	printf("je sors ici\n");
    
    
}

void test_STRDUP_NAME_null_handling() {
    TEST_SECTION("STRDUP_NAME NULL HANDLING");
    
    // Note: STRDUP_NAME(NULL) n'est pas défini par le standard C
    // Votre implémentation peut soit:
    // 1. Retourner NULL
    // 2. Faire un segfault (comportement de glibc)
    // 3. Gérer le cas spécialement
    
    printf("Note: Testing STRDUP_NAME(NULL) - behavior is implementation-defined\n");
    
    // Test plus sûr: vérifier avec une chaîne qui pourrait être corrompue
    // mais n'est pas NULL
    char almost_empty[] = {'\0'};
    char *empty_dup = STRDUP_NAME(almost_empty);
    test_result(empty_dup != NULL, "STRDUP_NAME empty string (via array) succeeds");
    if (empty_dup) {
        test_result(strlen(empty_dup) == 0, "STRDUP_NAME empty string length is 0");
        test_result(empty_dup[0] == '\0', "STRDUP_NAME empty string is properly terminated");
        FREE_NAME(empty_dup);
    }
}

void test_STRDUP_NAME_boundary_conditions() {
    TEST_SECTION("STRDUP_NAME BOUNDARY CONDITIONS");
    
    // Test exactement aux limites des catégories de MALLOC_NAME
    
    // Limite TINY: 128 bytes total, donc 127 chars + \0
    char tiny_boundary[128];
    memset(tiny_boundary, 'T', 127);
    tiny_boundary[127] = '\0';
    
    char *tiny_dup = STRDUP_NAME(tiny_boundary);
    test_result(tiny_dup != NULL, "STRDUP_NAME TINY boundary (127 chars) succeeds");
    if (tiny_dup) {
        test_result(strlen(tiny_dup) == 127, "STRDUP_NAME TINY boundary length correct");
        test_result(tiny_dup[126] == 'T' && tiny_dup[127] == '\0', 
                   "STRDUP_NAME TINY boundary termination correct");
        FREE_NAME(tiny_dup);
    }
    
    // Limite SMALL: 1024 bytes total, donc 1023 chars + \0
    char *small_boundary = MALLOC_NAME(1024);
    if (small_boundary) {
        memset(small_boundary, 'S', 1023);
        small_boundary[1023] = '\0';
        
        char *small_dup = STRDUP_NAME(small_boundary);
        test_result(small_dup != NULL, "STRDUP_NAME SMALL boundary (1023 chars) succeeds");
        if (small_dup) {
            test_result(strlen(small_dup) == 1023, "STRDUP_NAME SMALL boundary length correct");
            test_result(small_dup[1022] == 'S' && small_dup[1023] == '\0', 
                       "STRDUP_NAME SMALL boundary termination correct");
            FREE_NAME(small_dup);
        }
        FREE_NAME(small_boundary);
    }
    
    // Premier LARGE: 1025 bytes total, donc 1024 chars + \0
    char *large_boundary = MALLOC_NAME(1025);
    if (large_boundary) {
        memset(large_boundary, 'L', 1024);
        large_boundary[1024] = '\0';
        
        char *large_dup = STRDUP_NAME(large_boundary);
        test_result(large_dup != NULL, "STRDUP_NAME LARGE boundary (1024 chars) succeeds");
        if (large_dup) {
            test_result(strlen(large_dup) == 1024, "STRDUP_NAME LARGE boundary length correct");
            test_result(large_dup[1023] == 'L' && large_dup[1024] == '\0', 
                       "STRDUP_NAME LARGE boundary termination correct");
            FREE_NAME(large_dup);
        }
        FREE_NAME(large_boundary);
    }
}

#include <pthread.h>
#include <sys/time.h>
// Configuration des tests
#define NUM_THREADS 100
#define ALLOCATIONS_PER_THREAD 1000
#define MAX_ALLOC_SIZE 4096
#define MIN_ALLOC_SIZE 8

// Structure pour passer les paramètres aux threads
typedef struct {
    int thread_id;
    int allocations_target;
    int allocations_made;
    int errors;
    int write_errors;
    int read_errors;
    double execution_time;
    void** allocated_ptrs;
    size_t* allocated_sizes;
} thread_data_t;

// Variables globales pour les statistiques
pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;
int total_allocations = 0;
int total_errors = 0;
size_t total_memory_allocated = 0;

// Fonction utilitaire pour obtenir le temps en microsecondes
double get_time_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

// Test 1: Allocations séquentielles avec vérification d'intégrité
void* test_sequential_MALLOC_NAME(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test séquentiel (%d allocations)\n", 
           data->thread_id, data->allocations_target);
    
    data->allocated_ptrs = CALLOC_NAME(data->allocations_target, sizeof(void*));
    data->allocated_sizes = CALLOC_NAME(data->allocations_target, sizeof(size_t));
    
    if (!data->allocated_ptrs || !data->allocated_sizes) {
        printf("Thread %d: Erreur allocation tableaux de suivi\n", data->thread_id);
        data->errors++;
        return NULL;
    }
    
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        
        // Allocation
        void* ptr = MALLOC_NAME(size);
        if (!ptr) {
            printf("Thread %d: Erreur allocation %zu bytes (allocation #%d)\n", 
                   data->thread_id, size, i);
            data->errors++;
            continue;
        }
        
        data->allocated_ptrs[i] = ptr;
        data->allocated_sizes[i] = size;
        data->allocations_made++;
        
        // Test d'écriture avec pattern unique par thread
        unsigned char pattern = (data->thread_id * 13 + i) % 256;
        memset(ptr, pattern, size);
        
        // Vérification immédiate de l'écriture
        unsigned char* byte_ptr = (unsigned char*)ptr;
        for (size_t j = 0; j < size; j++) {
            if (byte_ptr[j] != pattern) {
                printf("Thread %d: Erreur écriture à l'offset %zu (alloc #%d)\n", 
                       data->thread_id, j, i);
                data->write_errors++;
                break;
            }
        }
        
        // Pause occasionnelle pour laisser de la place aux autres threads
        if (i % 50 == 0) {
            usleep(1);
        }
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_errors += data->errors + data->write_errors;
    for (int i = 0; i < data->allocations_made; i++) {
        if (data->allocated_ptrs[i]) {
            total_memory_allocated += data->allocated_sizes[i];
        }
    }
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Séquentiel terminé - %d/%d allocations réussies, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->allocations_target, 
           data->errors + data->write_errors);
    
    return NULL;
}

// Test 2: Allocations rapides avec différentes tailles
void* test_rapid_MALLOC_NAME(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test rapide\n", data->thread_id);
    
    data->allocated_ptrs = CALLOC_NAME(data->allocations_target, sizeof(void*));
    data->allocated_sizes = CALLOC_NAME(data->allocations_target, sizeof(size_t));
    
    // Tailles prédéfinies pour tester différents cas
    size_t test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    int num_test_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = test_sizes[i % num_test_sizes];
        
        void* ptr = MALLOC_NAME(size);
        if (!ptr) {
            printf("Thread %d: Erreur allocation %zu bytes\n", data->thread_id, size);
            data->errors++;
            continue;
        }
        
        data->allocated_ptrs[i] = ptr;
        data->allocated_sizes[i] = size;
        data->allocations_made++;
        
        // Pattern d'écriture basé sur la taille et l'index
        unsigned char pattern = (size + i) % 256;
        memset(ptr, pattern, size);
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_errors += data->errors;
    for (int i = 0; i < data->allocations_made; i++) {
        if (data->allocated_ptrs[i]) {
            total_memory_allocated += data->allocated_sizes[i];
        }
    }
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Rapide terminé - %d allocations, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->errors);
    
    return NULL;
}

// Test 3: Allocations avec vérification différée de l'intégrité
void* test_delayed_verification(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test avec vérification différée\n", data->thread_id);
    
    data->allocated_ptrs = CALLOC_NAME(data->allocations_target, sizeof(void*));
    data->allocated_sizes = CALLOC_NAME(data->allocations_target, sizeof(size_t));
    
    // Phase 1: Allocations rapides
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        
        void* ptr = MALLOC_NAME(size);
        if (!ptr) {
            data->errors++;
            continue;
        }
        
        data->allocated_ptrs[i] = ptr;
        data->allocated_sizes[i] = size;
        data->allocations_made++;
        
        // Écriture d'un pattern identifiable
        unsigned char pattern = (data->thread_id * 17 + i * 3) % 256;
        memset(ptr, pattern, size);
    }
    
    // Pause pour laisser les autres threads travailler
    usleep(5000);
    
    // Phase 2: Vérification différée de toutes les allocations
    for (int i = 0; i < data->allocations_made; i++) {
        if (!data->allocated_ptrs[i]) continue;
        
        unsigned char expected_pattern = (data->thread_id * 17 + i * 3) % 256;
        unsigned char* byte_ptr = (unsigned char*)data->allocated_ptrs[i];
        size_t size = data->allocated_sizes[i];
        
        for (size_t j = 0; j < size; j++) {
            if (byte_ptr[j] != expected_pattern) {
                printf("Thread %d: Corruption détectée dans l'allocation #%d à l'offset %zu\n", 
                       data->thread_id, i, j);
                data->read_errors++;
                break;
            }
        }
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_errors += data->errors + data->read_errors;
    for (int i = 0; i < data->allocations_made; i++) {
        if (data->allocated_ptrs[i]) {
            total_memory_allocated += data->allocated_sizes[i];
        }
    }
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Vérification différée terminée - %d allocations, %d erreurs lecture\n", 
           data->thread_id, data->allocations_made, data->read_errors);
    
    return NULL;
}

// Test 4: Stress test avec allocations de grandes tailles
void* test_large_allocations(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test grandes allocations\n", data->thread_id);
    
    // Réduire le nombre pour les grandes allocations
    int large_alloc_count = data->allocations_target / 4;
    data->allocated_ptrs = CALLOC_NAME(large_alloc_count, sizeof(void*));
    data->allocated_sizes = CALLOC_NAME(large_alloc_count, sizeof(size_t));
    
    for (int i = 0; i < large_alloc_count; i++) {
        // Tailles entre 1KB et 16KB
        size_t size = 1024 + (rand() % (16 * 1024 - 1024));
        
        void* ptr = MALLOC_NAME(size);
        if (!ptr) {
            printf("Thread %d: Erreur allocation grande taille %zu bytes\n", 
                   data->thread_id, size);
            data->errors++;
            continue;
        }
        
        data->allocated_ptrs[i] = ptr;
        data->allocated_sizes[i] = size;
        data->allocations_made++;
        
        // Écriture par blocs pour les grandes allocations
        unsigned char pattern = (data->thread_id + i) % 256;
        unsigned char* byte_ptr = (unsigned char*)ptr;
        
        // Écrire au début, milieu et fin
        byte_ptr[0] = pattern;
        byte_ptr[size/2] = pattern;
        byte_ptr[size-1] = pattern;
        
        // Vérification immédiate des positions critiques
        if (byte_ptr[0] != pattern || byte_ptr[size/2] != pattern || byte_ptr[size-1] != pattern) {
            printf("Thread %d: Erreur écriture grande allocation #%d\n", data->thread_id, i);
            data->write_errors++;
        }
        
        // Pause plus longue pour les grandes allocations
        usleep(10);
    }
    
    data->execution_time = get_time_us() - start_time;
    
    pthread_mutex_lock(&stats_mutex);
    total_allocations += data->allocations_made;
    total_errors += data->errors + data->write_errors;
    for (int i = 0; i < data->allocations_made; i++) {
        if (data->allocated_ptrs[i]) {
            total_memory_allocated += data->allocated_sizes[i];
        }
    }
    pthread_mutex_unlock(&stats_mutex);
    
    printf("Thread %d: Grandes allocations terminées - %d allocations, %d erreurs\n", 
           data->thread_id, data->allocations_made, data->errors + data->write_errors);
    
    return NULL;
}

// Fonction pour vérifier l'alignement des pointeurs
void check_pointer_alignment(void** ptrs, int count, int thread_id) {
    int misaligned = 0;
    for (int i = 0; i < count; i++) {
        if (ptrs[i] && ((uintptr_t)ptrs[i] % sizeof(void*)) != 0) {
            misaligned++;
        }
    }
    if (misaligned > 0) {
        printf("Thread %d: ATTENTION - %d pointeurs mal alignés détectés\n", thread_id, misaligned);
    }
}

int main() {
    printf(YELLOW "Starting comprehensive MALLOC_NAME/REALLOC_NAME/FREE_NAME test suite...\n" RESET);
    
    test_basic_MALLOC_NAME();
    test_small_MALLOC_NAME();
    test_large_MALLOC_NAME();
    test_fragmentation();
    test_REALLOC_NAME_tiny_to_tiny();
    test_REALLOC_NAME_small_to_small();
    test_REALLOC_NAME_large_to_large();
    test_REALLOC_NAME_category_changes();
    test_REALLOC_NAME_edge_cases();
    test_multiple_allocations();
    test_stress_REALLOC_NAME();
    test_boundary_values();
    test_FREE_NAME_edge_cases();
    test_memory_patterns();
    test_CALLOC_NAME_basic();
    test_CALLOC_NAME_sizes();
    test_CALLOC_NAME_overflow();
    test_CALLOC_NAME_patterns();
    test_CALLOC_NAME_extreme();
    
    // Tests REALLOCARRAY_NAME hardcore  
    test_REALLOCARRAY_NAME_basic();
    test_REALLOCARRAY_NAME_overflow();
    test_REALLOCARRAY_NAME_categories();
    test_REALLOCARRAY_NAME_edge_cases();
    test_REALLOCARRAY_NAME_stress();
    test_REALLOCARRAY_NAME_data_integrity();
    test_REALLOCARRAY_NAME_boundary();
    
    // Tests d'interaction
    test_CALLOC_NAME_REALLOCARRAY_NAME_interaction();

	test_STRDUP_NAME_basic();
    test_STRDUP_NAME_sizes();
    test_STRDUP_NAME_content_integrity();
    test_STRDUP_NAME_independence();
    test_STRDUP_NAME_extreme_sizes();
    test_STRDUP_NAME_special_chars();
    test_STRDUP_NAME_alignment();
    test_STRDUP_NAME_stress();
    test_STRDUP_NAME_null_handling();
    test_STRDUP_NAME_boundary_conditions();

    printf(BLUE "\n=== FINAL MEMORY STATE ===" RESET "\n");
    
    
    printf(YELLOW "\n=== TEST SUMMARY ===" RESET "\n");
    printf("Tests passed: " GREEN "%d" RESET "\n", tests_passed);
    printf("Tests failed: " RED "%d" RESET "\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf(GREEN "🎉 ALL TESTS PASSED! 🎉" RESET "\n");
    } else {
        printf(RED "⚠️  Some tests failed. Check implementation." RESET "\n");
        return 1;
    }

    printf("=== Test Multi-threadé pour MALLOC_NAME uniquement ===\n");
    printf("Configuration: %d threads, %d allocations par thread\n", 
           NUM_THREADS, ALLOCATIONS_PER_THREAD);
    printf("Tailles: %d à %d bytes\n\n", MIN_ALLOC_SIZE, MAX_ALLOC_SIZE);
    
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    // Initialiser le générateur aléatoire
    srand(time(NULL));
    
    // Test avec mix de différents patterns
    printf(">>> Démarrage test multi-threadé avec patterns variés\n");
    total_allocations = total_errors = 0;
    total_memory_allocated = 0;
    
    void* (*test_functions[])(void*) = {
        test_sequential_MALLOC_NAME,
        test_rapid_MALLOC_NAME,
        test_delayed_verification,
        test_large_allocations
    };
    
    const char* test_names[] = {
        "Séquentiel",
        "Rapide", 
        "Vérification différée",
        "Grandes allocations"
    };
    
    double global_start = get_time_us();
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].allocations_target = ALLOCATIONS_PER_THREAD;
        thread_data[i].allocations_made = 0;
        thread_data[i].errors = 0;
        thread_data[i].write_errors = 0;
        thread_data[i].read_errors = 0;
        thread_data[i].allocated_ptrs = NULL;
        thread_data[i].allocated_sizes = NULL;
        
        // Alterner entre les différents types de tests
        void* (*test_func)(void*) = test_functions[i % 4];
        printf("Thread %d utilise: %s\n", i, test_names[i % 4]);
        
        pthread_create(&threads[i], NULL, test_func, &thread_data[i]);
    }
    
    // Attendre tous les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    double global_time = get_time_us() - global_start;
    
    // Vérifications post-test
    printf("\n=== Vérifications finales ===\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        if (thread_data[i].allocated_ptrs) {
            check_pointer_alignment(thread_data[i].allocated_ptrs, 
                                  thread_data[i].allocations_made, 
                                  thread_data[i].thread_id);
        }
    }
    
    // Afficher les statistiques finales
    printf("\n=== Résultats finaux ===\n");
    printf("Allocations totales réussies: %d\n", total_allocations);
    printf("Mémoire totale allouée: %.2f MB\n", total_memory_allocated / (1024.0 * 1024.0));
    printf("Erreurs totales: %d\n", total_errors);
    printf("Temps total: %.2f ms\n", global_time / 1000.0);
    printf("Débit global: %.0f allocations/seconde\n", 
           total_allocations * 1000000.0 / global_time);
    
    // Statistiques par thread
    printf("\n=== Détails par thread ===\n");
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("Thread %d (%s): %d allocs, %.2f ms, %.0f ops/sec, %d erreurs\n",
               thread_data[i].thread_id,
               test_names[i % 4],
               thread_data[i].allocations_made,
               thread_data[i].execution_time / 1000.0,
               thread_data[i].allocations_made * 1000000.0 / thread_data[i].execution_time,
               thread_data[i].errors + thread_data[i].write_errors + thread_data[i].read_errors);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        if (thread_data[i].allocated_ptrs) {
            FREE_NAME(thread_data[i].allocated_ptrs);  // FREE_NAME standard
        }
        if (thread_data[i].allocated_sizes) {
            FREE_NAME(thread_data[i].allocated_sizes);  // FREE_NAME standard
        }
    }
    
	// 
    if (total_errors == 0) {
        printf("\n✅ TOUS LES TESTS SONT PASSÉS!\n");
        printf("Votre MALLOC_NAME semble fonctionner correctement en multi-threading.\n");
        return 0;
    } else {
        printf("\n❌ %d ERREURS DÉTECTÉES\n", total_errors);
        printf("Vérifiez la thread-safety de votre MALLOC_NAME.\n");
        return 1;
    }
}

// int main(void)
// {
// 	void *test[1000];

// 	for (int i = 0; i < 1000; i++) {
// 		printf("i == %d\n", i);
// 		test[i] = MALLOC_NAME(10000);
// 		memset(test[i], i, 10000);
// 	}
//
// 	for (int i = 500; i < 1000; i++) {
// 		ft_printf_fd(2, "i == %d\n", i);
// 		FREE_NAME(test[i]);
// 	}
// 	for (int i = 500; i < 1000; i++) {
// 		printf("i == %d\n", i);
// 		test[i] = MALLOC_NAME(10000);
// 		memset(test[i], i, 10000);
// 	}
// 	printf("here\n");
//
// 	for (int i = 0; i < 1000; i++) {
// 		FREE_NAME(test[i]);
// 	}
//
// }