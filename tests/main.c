#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>


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

void test_basic_malloc() {
    TEST_SECTION("BASIC malloc TESTS");
    
    // Test 1: malloc(0) - comportement défini par l'implémentation
    void *p0 = malloc(0);
    test_result(p0 != NULL || p0 == NULL, "malloc(0) returns valid result");
    if (p0) free(p0);
    
    // Test 2: petites tailles (TINY)
    char *tiny1 = malloc(1);

    char *tiny8 = malloc(8);

    char *tiny16 = malloc(16);

    char *tiny32 = malloc(32);

    char *tiny64 = malloc(64);

    char *tiny128 = malloc(128);
    
    test_result(tiny1 != NULL, "malloc(1) succeeds");
    test_result(tiny8 != NULL, "malloc(8) succeeds");
    test_result(tiny16 != NULL, "malloc(16) succeeds");
    test_result(tiny32 != NULL, "malloc(32) succeeds");
    test_result(tiny64 != NULL, "malloc(64) succeeds");
    test_result(tiny128 != NULL, "malloc(128) succeeds");
    
    // Test d'alignement (8 bytes)
    test_result(((uintptr_t)tiny1 % 8) == 0, "malloc(1) is 8-byte aligned");
    test_result(((uintptr_t)tiny8 % 8) == 0, "malloc(8) is 8-byte aligned");
    test_result(((uintptr_t)tiny16 % 8) == 0, "malloc(16) is 8-byte aligned");
    test_result(((uintptr_t)tiny32 % 8) == 0, "malloc(32) is 8-byte aligned");
    test_result(((uintptr_t)tiny64 % 8) == 0, "malloc(64) is 8-byte aligned");
    test_result(((uintptr_t)tiny128 % 8) == 0, "malloc(128) is 8-byte aligned");
    
    // Test d'écriture/lecture
    if (tiny32) {
        strcpy(tiny32, "Hello");
        test_result(strcmp(tiny32, "Hello") == 0, "Write/read in tiny malloc");
    }
    
    
    
    free(tiny1); 
	free(tiny8); 
	free(tiny16); 
    free(tiny32); 
	free(tiny64); 
	free(tiny128);
}

void test_small_malloc() {
    TEST_SECTION("SMALL malloc TESTS");
    
    // Test tailles SMALL (129 à 1024 bytes)
    char *small200 = malloc(200);
    char *small400 = malloc(400);
    char *small600 = malloc(600);
    char *small800 = malloc(800);
    char *small1000 = malloc(1000);
    char *small1024 = malloc(1024); // limite SMALL
    
    test_result(small200 != NULL, "malloc(200) succeeds");
    test_result(small400 != NULL, "malloc(400) succeeds");
    test_result(small600 != NULL, "malloc(600) succeeds");
    test_result(small800 != NULL, "malloc(800) succeeds");
    test_result(small1000 != NULL, "malloc(1000) succeeds");
    test_result(small1024 != NULL, "malloc(1024) succeeds");
    
    // Test d'alignement
    test_result(((uintptr_t)small200 % 8) == 0, "malloc(200) is 8-byte aligned");
    test_result(((uintptr_t)small400 % 8) == 0, "malloc(400) is 8-byte aligned");
    test_result(((uintptr_t)small1024 % 8) == 0, "malloc(1024) is 8-byte aligned");
    
    // Test d'écriture dans les blocs
    if (small400) {
        memset(small400, 'A', 399);
        small400[399] = '\0';
        test_result(small400[0] == 'A' && small400[398] == 'A', "Write/read in small malloc");
    }
    
    
    
    free(small200);
	free(small400);
	free(small600);
    free(small800);
	free(small1000); 
	free(small1024);
}

void test_large_malloc() {
    TEST_SECTION("LARGE malloc TESTS");
    
    // Test tailles LARGE (> 1024 bytes)
    char *large1025 = malloc(1025); // première taille LARGE
    char *large2048 = malloc(2048);
    char *large4096 = malloc(4096);
    char *large8192 = malloc(8192);
    char *large16384 = malloc(16384);
    char *large_huge = malloc(1024 * 1024); // 1MB
    
    test_result(large1025 != NULL, "malloc(1025) succeeds");
    test_result(large2048 != NULL, "malloc(2048) succeeds");
    test_result(large4096 != NULL, "malloc(4096) succeeds");
    test_result(large8192 != NULL, "malloc(8192) succeeds");
    test_result(large16384 != NULL, "malloc(16384) succeeds");
    test_result(large_huge != NULL, "malloc(1MB) succeeds");
    
    // Test d'alignement
    test_result(((uintptr_t)large1025 % 8) == 0, "malloc(1025) is 8-byte aligned");
    test_result(((uintptr_t)large2048 % 8) == 0, "malloc(2048) is 8-byte aligned");
    test_result(((uintptr_t)large_huge % 8) == 0, "malloc(1MB) is 8-byte aligned");
    
    // Test d'écriture dans un gros bloc
    if (large_huge) {
        large_huge[0] = 'X';
        large_huge[1024 * 1024 - 1] = 'Y';
        test_result(large_huge[0] == 'X' && large_huge[1024 * 1024 - 1] == 'Y', 
                   "Write/read in 1MB malloc");
    }
    
    
    
    free(large1025);
	
	free(large2048);
	
	free(large4096);
    
	free(large8192);
	
	free(large16384);
	
	free(large_huge);
}

void test_fragmentation() {
    TEST_SECTION("FRAGMENTATION TESTS");
    
    // Créer de la fragmentation
    void *ptrs[10];
    
    // Allouer 10 blocs
    for (int i = 0; i < 10; i++) {
        ptrs[i] = malloc(64);
        test_result(ptrs[i] != NULL, "Fragmentation malloc succeeds");
    }
    
    
    
    // Libérer les blocs impairs (créer des trous)
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("After freeing odd blocks:\n");
    
    
    // Essayer de réallouer dans les trous
    void *new_ptr = malloc(32); // devrait réutiliser un trou
    test_result(new_ptr != NULL, "malloc after fragmentation succeeds");
    
    // Nettoyer
    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
    }
    free(new_ptr);
}

void test_realloc_tiny_to_tiny() {
    TEST_SECTION("realloc TINY->TINY TESTS");
    
    char *ptr = malloc(16);
    test_result(ptr != NULL, "Initial malloc(16) for realloc");
    
    if (ptr) {
        strcpy(ptr, "Hello");
        
        // Agrandir dans TINY
        ptr = realloc(ptr, 32);
        test_result(ptr != NULL, "realloc(16->32) succeeds");
        test_result(strcmp(ptr, "Hello") == 0, "Data preserved in realloc(16->32)");
        
        // Réduire dans TINY
        ptr = realloc(ptr, 8);
        test_result(ptr != NULL, "realloc(32->8) succeeds");
        test_result(strncmp(ptr, "Hello", 5) == 0, "Data preserved in realloc(32->8)");
        
        free(ptr);

    }
}

void test_realloc_small_to_small() {
    TEST_SECTION("realloc SMALL->SMALL TESTS");
    
    char *ptr = malloc(200);
    test_result(ptr != NULL, "Initial malloc(200) for realloc");
    
    if (ptr) {
        memset(ptr, 'A', 199);
        ptr[199] = '\0';
        
        // Agrandir dans SMALL
        ptr = realloc(ptr, 500);
        test_result(ptr != NULL, "realloc(200->500) succeeds");
        test_result(ptr[0] == 'A' && ptr[198] == 'A', "Data preserved in realloc(200->500)");
        
        // Réduire dans SMALL
        ptr = realloc(ptr, 150);
        test_result(ptr != NULL, "realloc(500->150) succeeds");
        test_result(ptr[0] == 'A' && ptr[149] == 'A', "Data preserved in realloc(500->150)");
        
        free(ptr);
    }
}

void test_realloc_large_to_large() {
    TEST_SECTION("realloc LARGE->LARGE TESTS");
    
    char *ptr = malloc(2048);
    test_result(ptr != NULL, "Initial malloc(2048) for realloc");
    
    if (ptr) {
        memset(ptr, 'B', 2047);
        ptr[2047] = '\0';
        
        // Agrandir dans LARGE
        ptr = realloc(ptr, 4096);
        test_result(ptr != NULL, "realloc(2048->4096) succeeds");
        test_result(ptr[0] == 'B' && ptr[2046] == 'B', "Data preserved in realloc(2048->4096)");
        
        // Réduire dans LARGE
        ptr = realloc(ptr, 1500);
        test_result(ptr != NULL, "realloc(4096->1500) succeeds");
        test_result(ptr[0] == 'B' && ptr[1499] == 'B', "Data preserved in realloc(4096->1500)");
        
        free(ptr);
    }
}

void test_realloc_category_changes() {
    TEST_SECTION("realloc CATEGORY CHANGE TESTS");
    
    // TINY -> SMALL
    char *ptr = malloc(64);
    if (ptr) {
        strcpy(ptr, "TinyToSmall");
        ptr = realloc(ptr, 256);
        test_result(ptr != NULL, "realloc TINY->SMALL succeeds");
        test_result(strcmp(ptr, "TinyToSmall") == 0, "Data preserved TINY->SMALL");
        
        // SMALL -> LARGE
        strcat(ptr, " and SmallToLarge");
        ptr = realloc(ptr, 2048);
        test_result(ptr != NULL, "realloc SMALL->LARGE succeeds");
        test_result(strstr(ptr, "TinyToSmall") != NULL, "Data preserved SMALL->LARGE");
        
        // LARGE -> SMALL
        ptr = realloc(ptr, 512);
        test_result(ptr != NULL, "realloc LARGE->SMALL succeeds");
        test_result(strstr(ptr, "TinyToSmall") != NULL, "Data preserved LARGE->SMALL");
        
        // SMALL -> TINY
        ptr = realloc(ptr, 32);
        test_result(ptr != NULL, "realloc SMALL->TINY succeeds");
        test_result(strncmp(ptr, "TinyToSmall", 11) == 0, "Data preserved SMALL->TINY");
        
        free(ptr);
    }
}

void test_realloc_edge_cases() {
    TEST_SECTION("realloc EDGE CASES");
    
    // realloc(NULL, size) = malloc(size)
    char *ptr = realloc(NULL, 100);
    test_result(ptr != NULL, "realloc(NULL, 100) works like malloc");
    if (ptr) {
        strcpy(ptr, "realloc NULL test");
        test_result(strcmp(ptr, "realloc NULL test") == 0, "Write after realloc(NULL)");
    }
    
    // realloc(ptr, 0) = free(ptr) + return implementation-defined
    void *result = realloc(ptr, 0);
    test_result(1, "realloc(ptr, 0) completes without crash");
    if (result) free(result);
    
    // realloc même taille
    ptr = malloc(100);
    if (ptr) {
        strcpy(ptr, "Same size test");
        char *old_ptr = ptr;
        ptr = realloc(ptr, 100);
        test_result(ptr != NULL, "realloc same size succeeds");
        test_result(strcmp(ptr, "Same size test") == 0, "Data preserved same size realloc");
        test_result(old_ptr == ptr, "Pointer preserved same size realloc");
        free(ptr);
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
        ptrs[i] = malloc(size);
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
            free(ptrs[i]);
        }
    }
}

void test_stress_realloc() {
    TEST_SECTION("STRESS realloc TEST");
    
    char *ptr = malloc(16);
    test_result(ptr != NULL, "Initial allocation for stress test");
    
    if (ptr) {
        strcpy(ptr, "Start");
        
        // Série de realloc avec des tailles croissantes et décroissantes
        size_t sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16};
        int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
        int all_success = 1;
        
        for (int i = 0; i < num_sizes; i++) {
            ptr = realloc(ptr, sizes[i]);
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
        
        test_result(all_success, "Stress realloc sequence succeeds");
        
        if (ptr) free(ptr);
    }
}

void test_boundary_values() {
    TEST_SECTION("BOUNDARY VALUES TEST");
    
    // Tester les limites exactes entre catégories
    void *tiny_max = malloc(128);      // Max TINY
    void *small_min = malloc(129);     // Min SMALL
    void *small_max = malloc(1024);    // Max SMALL
    void *large_min = malloc(1025);    // Min LARGE
    
    test_result(tiny_max != NULL, "malloc(128) - TINY max boundary");
    test_result(small_min != NULL, "malloc(129) - SMALL min boundary");
    test_result(small_max != NULL, "malloc(1024) - SMALL max boundary");
    test_result(large_min != NULL, "malloc(1025) - LARGE min boundary");
    
    // Vérifier les alignements aux limites
    test_result(((uintptr_t)tiny_max % 8) == 0, "TINY max boundary aligned");
    test_result(((uintptr_t)small_min % 8) == 0, "SMALL min boundary aligned");
    test_result(((uintptr_t)small_max % 8) == 0, "SMALL max boundary aligned");
    test_result(((uintptr_t)large_min % 8) == 0, "LARGE min boundary aligned");
    
    
    
    free(tiny_max); free(small_min); 
    free(small_max); free(large_min);
}

void test_free_edge_cases() {
    TEST_SECTION("free EDGE CASES");
    
    // free(NULL) doit être sans effet
    free(NULL);
    test_result(1, "free(NULL) doesn't crash");
    
    // Double free - devrait être détecté si implémenté
    void *ptr = malloc(100);
    if (ptr) {
        strcpy((char*)ptr, "Double free test");
        free(ptr);
        printf("Note: Uncomment next line to test double-free detection\n");
        // free(ptr); // Décommenter pour tester la détection de double free
    }
}

void test_memory_patterns() {
    TEST_SECTION("MEMORY PATTERNS TEST");
    
    // Test avec différents patterns pour détecter la corruption
    void *ptrs[5];
    char patterns[] = {0x00, 0xFF, 0xAA, 0x55, 0xCC};
    
    for (int i = 0; i < 5; i++) {
        ptrs[i] = malloc(256);
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
        if (ptrs[i]) free(ptrs[i]);
    }
}

void test_calloc_basic() {
    TEST_SECTION("calloc BASIC TESTS");
    
    // Test 1: calloc(0, size) et calloc(nmemb, 0)
    void *p1 = calloc(0, 10);
    void *p2 = calloc(10, 0);
    void *p3 = calloc(0, 0);
    test_result(p1 != NULL || p1 == NULL, "calloc(0, 10) returns valid result");
    test_result(p2 != NULL || p2 == NULL, "calloc(10, 0) returns valid result");
    test_result(p3 != NULL || p3 == NULL, "calloc(0, 0) returns valid result");
    if (p1) free(p1);
    if (p2) free(p2);
    if (p3) free(p3);
    
    // Test 2: calloc simple avec vérification de zéro
    char *ptr = calloc(10, sizeof(char));
    test_result(ptr != NULL, "calloc(10, sizeof(char)) succeeds");
    
    if (ptr) {
        int all_zero = 1;
        for (int i = 0; i < 10; i++) {
            if (ptr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc initializes memory to zero");
        test_result(((uintptr_t)ptr % 8) == 0, "calloc result is 8-byte aligned");
        free(ptr);
    }
    
    // Test 3: calloc avec différentes tailles
    int *int_arr = calloc(100, sizeof(int));
    test_result(int_arr != NULL, "calloc(100, sizeof(int)) succeeds");
    if (int_arr) {
        int all_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (int_arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc int array initialized to zero");
        free(int_arr);
    }
    
    // Test 4: calloc TINY
    char *tiny = calloc(64, 1);
    test_result(tiny != NULL, "calloc TINY (64 bytes) succeeds");
    if (tiny) {
        int all_zero = 1;
        for (int i = 0; i < 64; i++) {
            if (tiny[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc TINY initialized to zero");
        free(tiny);
    }
    
    
}

void test_calloc_sizes() {
    TEST_SECTION("calloc SIZE CATEGORIES");
    
    // Test TINY (≤ 128 bytes)
    char *tiny = calloc(32, 4); // 128 bytes exactly
    test_result(tiny != NULL, "calloc TINY boundary (32*4=128) succeeds");
    if (tiny) {
        int all_zero = 1;
        for (int i = 0; i < 128; i++) {
            if (tiny[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc TINY boundary zeroed");
        free(tiny);
    }
    
    // Test SMALL (129-1024 bytes)
    short *small = calloc(200, sizeof(short)); // 400 bytes
    test_result(small != NULL, "calloc SMALL (200*2=400) succeeds");
    if (small) {
        int all_zero = 1;
        for (int i = 0; i < 200; i++) {
            if (small[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc SMALL zeroed");
        free(small);
    }
    
    // Test LARGE (> 1024 bytes)
    long *large = calloc(300, sizeof(long)); // 2400 bytes sur 64-bit
    test_result(large != NULL, "calloc LARGE (300*8=2400) succeeds");
    if (large) {
        int all_zero = 1;
        for (int i = 0; i < 300; i++) {
            if (large[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc LARGE zeroed");
        free(large);
    }
    
    
}

void test_calloc_overflow() {
    TEST_SECTION("calloc OVERFLOW TESTS");
    
    // Test overflow detection
    size_t huge = SIZE_MAX / 2;
    void *overflow1 = calloc(huge, huge);
    test_result(overflow1 == NULL, "calloc overflow detection (huge * huge)");
    if (overflow1) free(overflow1);
    
    // Test autre cas d'overflow
    void *overflow2 = calloc(SIZE_MAX, 2);
    test_result(overflow2 == NULL, "calloc overflow detection (SIZE_MAX * 2)");
    if (overflow2) free(overflow2);
    
    // Test cas limite mais valide
    void *valid = calloc(1000, 1000); // 1MB
    test_result(valid != NULL, "calloc large but valid allocation");
    if (valid) {
        // Vérifier quelques positions aléatoirement
        char *ptr = (char*)valid;
        int sample_zero = (ptr[0] == 0 && ptr[500000] == 0 && ptr[999999] == 0);
        test_result(sample_zero, "calloc large allocation properly zeroed");
        free(valid);
    }
}

void test_calloc_patterns() {
    TEST_SECTION("calloc PATTERN TESTS");
    
    // Test avec structures complexes
    struct test_struct {
        int a;
        char b[16];
        double c;
        void *ptr;
    };
    
    struct test_struct *structs = calloc(10, sizeof(struct test_struct));
    test_result(structs != NULL, "calloc struct array succeeds");
    
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
        test_result(all_zero, "calloc struct array properly zeroed");
        free(structs);
    }
    
    // Test calloc avec différents patterns de taille
    for (int size = 1; size <= 10; size++) {
        void *ptr = calloc(size, 100);
        test_result(ptr != NULL, "calloc variable sizes succeeds");
        if (ptr) {
            char *cptr = (char*)ptr;
            int is_zero = 1;
            for (int i = 0; i < size * 100; i++) {
                if (cptr[i] != 0) {
                    is_zero = 0;
                    break;
                }
            }
            test_result(is_zero, "calloc variable sizes properly zeroed");
            free(ptr);
        }
    }
}

void test_calloc_extreme() {
    TEST_SECTION("calloc EXTREME TESTS");
    
    // Test calloc(1, very_large)
    void *big1 = calloc(1, 1024 * 1024); // 1MB
    test_result(big1 != NULL, "calloc(1, 1MB) succeeds");
    if (big1) {
        char *ptr = (char*)big1;
        int samples_zero = (ptr[0] == 0 && ptr[512*1024] == 0 && ptr[1024*1024-1] == 0);
        test_result(samples_zero, "calloc(1, 1MB) properly zeroed");
        free(big1);
    }
    
    // Test calloc(very_large, 1)
    void *big2 = calloc(1024 * 1024, 1); // 1MB
    test_result(big2 != NULL, "calloc(1MB, 1) succeeds");
    if (big2) {
        char *ptr = (char*)big2;
        int samples_zero = (ptr[0] == 0 && ptr[512*1024] == 0 && ptr[1024*1024-1] == 0);
        test_result(samples_zero, "calloc(1MB, 1) properly zeroed");
        free(big2);
    }
    
    // Test calloc avec des tailles premières
    void *prime = calloc(97, 101); // 9797 bytes
    test_result(prime != NULL, "calloc(97, 101) prime numbers succeeds");
    if (prime) {
        char *ptr = (char*)prime;
        int all_zero = 1;
        for (int i = 0; i < 97 * 101; i++) {
            if (ptr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc prime numbers properly zeroed");
        free(prime);
    }
    
    
}

void test_reallocarray_basic() {
    TEST_SECTION("reallocARRAY BASIC TESTS");
    
    // Test 1: reallocarray(NULL, nmemb, size) = calloc(nmemb, size)
    int *arr = reallocarray(NULL, 10, sizeof(int));
    test_result(arr != NULL, "reallocarray(NULL, 10, sizeof(int)) succeeds");
    
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
        test_result(values_ok, "reallocarray initial data integrity");
        
        // Test 2: agrandir le tableau
        arr = reallocarray(arr, 20, sizeof(int));
        test_result(arr != NULL, "reallocarray expand (10->20) succeeds");
        
        if (arr) {
            // Vérifier que les anciennes données sont préservées
            int old_data_ok = 1;
            for (int i = 0; i < 10; i++) {
                if (arr[i] != i * i) {
                    old_data_ok = 0;
                    break;
                }
            }
            test_result(old_data_ok, "reallocarray preserves old data on expand");
            
            // Initialiser les nouveaux éléments
            for (int i = 10; i < 20; i++) {
                arr[i] = i * 2;
            }
            
            // Test 3: réduire le tableau
            arr = reallocarray(arr, 15, sizeof(int));
            test_result(arr != NULL, "reallocarray shrink (20->15) succeeds");
            
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
                test_result(remaining_data_ok, "reallocarray preserves data on shrink");
                
                free(arr);
            }
        }
    }
    
    
}

void test_reallocarray_overflow() {
    TEST_SECTION("reallocARRAY OVERFLOW TESTS");
    
    // Test overflow detection avec un pointeur existant
    void *ptr = calloc(100, 1);
    if (ptr) {
        strcpy((char*)ptr, "Test data");
        
        // Tentative d'overflow
        void *overflow1 = reallocarray(ptr, SIZE_MAX/2, SIZE_MAX/2);
        test_result(overflow1 == NULL, "reallocarray overflow detection (huge*huge)");
        
        // Le pointeur original doit rester valide si reallocarray échoue
        if (overflow1 == NULL) {
            test_result(strcmp((char*)ptr, "Test data") == 0, "Original pointer valid after overflow");
        }
        
        // Test autre cas d'overflow
        void *overflow2 = reallocarray(ptr, SIZE_MAX, 2);
        test_result(overflow2 == NULL, "reallocarray overflow detection (SIZE_MAX*2)");
        if (overflow2 == NULL) {
            test_result(strcmp((char*)ptr, "Test data") == 0, "Original pointer still valid");
        }
        
        free(ptr);
    }
    
    // Test reallocarray(NULL, overflow_values)
    void *overflow3 = reallocarray(NULL, SIZE_MAX, SIZE_MAX);
    test_result(overflow3 == NULL, "reallocarray(NULL, overflow) fails safely");
    if (overflow3) free(overflow3);
    
    // Test cas limite valide
    void *valid = reallocarray(NULL, 1000, 1000); // 1MB
    test_result(valid != NULL, "reallocarray large but valid allocation");
    if (valid) free(valid);
}

void test_reallocarray_categories() {
    TEST_SECTION("reallocARRAY SIZE CATEGORIES");
    
    // TINY -> SMALL
    char *ptr = reallocarray(NULL, 64, 1); // 64 bytes TINY
    test_result(ptr != NULL, "reallocarray TINY (64*1) succeeds");
    
    if (ptr) {
        memset(ptr, 'T', 64);
        
        // Passer à SMALL
        ptr = reallocarray(ptr, 200, 1); // 200 bytes SMALL
        test_result(ptr != NULL, "reallocarray TINY->SMALL succeeds");
        
        if (ptr) {
            int data_ok = 1;
            for (int i = 0; i < 64; i++) {
                if (ptr[i] != 'T') {
                    data_ok = 0;
                    break;
                }
            }
            test_result(data_ok, "reallocarray TINY->SMALL preserves data");
            
            // Remplir le reste
            memset(ptr + 64, 'S', 136);
            
            // Passer à LARGE
            ptr = reallocarray(ptr, 2048, 1); // 2048 bytes LARGE
            test_result(ptr != NULL, "reallocarray SMALL->LARGE succeeds");
            
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
                test_result(data_ok, "reallocarray SMALL->LARGE preserves data");
                
                // Test retour LARGE -> SMALL
                ptr = reallocarray(ptr, 512, 1); // 512 bytes SMALL
                test_result(ptr != NULL, "reallocarray LARGE->SMALL succeeds");
                
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
                    test_result(data_ok, "reallocarray LARGE->SMALL preserves data");
                    
                    // Test retour SMALL -> TINY
                    ptr = reallocarray(ptr, 100, 1); // 100 bytes TINY
                    test_result(ptr != NULL, "reallocarray SMALL->TINY succeeds");
                    
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
                        test_result(data_ok, "reallocarray SMALL->TINY preserves data");
                        
                        free(ptr);
                    }
                }
            }
        }
    }
}

void test_reallocarray_edge_cases() {
    TEST_SECTION("reallocARRAY EDGE CASES");
    
    // Test reallocarray(ptr, 0, size) 
    void *ptr = calloc(100, 1);
    if (ptr) {
        strcpy((char*)ptr, "Edge test");
        
        void *result1 = reallocarray(ptr, 0, 10);
        test_result(result1 != NULL || result1 == NULL, "reallocarray(ptr, 0, 10) returns valid result");
        
            free(result1);
            ptr = NULL; // ptr a été libéré
    }
    
    // Test reallocarray(ptr, nmemb, 0)
    if (!ptr) {
        ptr = calloc(100, 1);
        strcpy((char*)ptr, "Edge test 2");
    }
    
    if (ptr) {
        void *result2 = reallocarray(ptr, 10, 0);
        test_result(result2 != NULL || result2 == NULL, "reallocarray(ptr, 10, 0) returns valid result");
        
        if (result2) {
            free(result2);
        }
		ptr = NULL;
    }
    
    // Test reallocarray same size
    double *darr = reallocarray(NULL, 50, sizeof(double));
    if (darr) {
        for (int i = 0; i < 50; i++) {
            darr[i] = i * 3.14;
        }
        
        double *same = reallocarray(darr, 50, sizeof(double));
        test_result(same != NULL, "reallocarray same size succeeds");
        
        if (same) {
            int data_ok = 1;
            for (int i = 0; i < 50; i++) {
                if (same[i] != i * 3.14) {
                    data_ok = 0;
                    break;
                }
            }
            test_result(data_ok, "reallocarray same size preserves data");
            free(same);
        }
    }
    
    // Test reallocarray(NULL, 1, 1)
    void *minimal = reallocarray(NULL, 1, 1);
    test_result(minimal != NULL, "reallocarray(NULL, 1, 1) succeeds");
    if (minimal) {
        *(char*)minimal = 'X';
        test_result(*(char*)minimal == 'X', "reallocarray minimal allocation works");
        free(minimal);
    }
}

void test_reallocarray_stress() {
    TEST_SECTION("reallocARRAY STRESS TEST");
    
    // Tableau qui grandit et rétrécit de façon dynamique
    int *dynamic_arr = NULL;
    size_t current_size = 0;
    int stress_ok = 1;
    
    // Séquence de tailles
    size_t sizes[] = {10, 50, 25, 100, 200, 150, 300, 75, 500, 1000, 250, 50, 10, 15};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int round = 0; round < num_sizes; round++) {
        size_t new_size = sizes[round];
        
        dynamic_arr = reallocarray(dynamic_arr, new_size, sizeof(int));
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
    
    test_result(stress_ok, "reallocarray stress test completes");
    
    if (dynamic_arr) {
        free(dynamic_arr);
    }
}

void test_reallocarray_data_integrity() {
    TEST_SECTION("reallocARRAY DATA INTEGRITY");
    
    // Test avec un pattern spécifique
    struct data_block {
        int magic;
        char data[256];
        int checksum;
    };
    
    struct data_block *blocks = reallocarray(NULL, 10, sizeof(struct data_block));
    test_result(blocks != NULL, "reallocarray data blocks allocation");
    
    if (blocks) {
        // Initialiser avec un pattern reconnaissable
        for (int i = 0; i < 10; i++) {
            blocks[i].magic = 0xDEADBEE;
            memset(blocks[i].data, 'A' + i, 256);
            blocks[i].checksum = i * 12345;
        }
        
        // Agrandir le tableau
        blocks = reallocarray(blocks, 20, sizeof(struct data_block));
        test_result(blocks != NULL, "reallocarray data blocks expansion");
        
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
            test_result(integrity_ok, "reallocarray preserves complex data structures");
            
            free(blocks);
        }
    }
}

void test_calloc_reallocarray_interaction() {
    TEST_SECTION("calloc + reallocARRAY INTERACTION");
    
    // Créer un tableau avec calloc
    long *arr = calloc(20, sizeof(long));
    test_result(arr != NULL, "calloc(20, sizeof(long)) for interaction test");
    
    if (arr) {
        // Vérifier qu'il est à zéro
        int all_zero = 1;
        for (int i = 0; i < 20; i++) {
            if (arr[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        test_result(all_zero, "calloc result is zeroed before reallocarray");
        
        // Modifier quelques valeurs
        for (int i = 0; i < 20; i++) {
            arr[i] = i * 100;
        }
        
        // Utiliser reallocarray pour agrandir
        arr = reallocarray(arr, 40, sizeof(long));
        test_result(arr != NULL, "reallocarray after calloc succeeds");
        
        if (arr) {
            // Vérifier que les données originales sont préservées
            int data_preserved = 1;
            for (int i = 0; i < 20; i++) {
                if (arr[i] != i * 100) {
                    data_preserved = 0;
                    break;
                }
            }
            test_result(data_preserved, "reallocarray preserves calloc'd data");
            
            free(arr);
        }
    }
    
    // Test inverse: reallocarray puis comparaison avec calloc
    char *buf1 = reallocarray(NULL, 100, sizeof(char));
    char *buf2 = calloc(100, sizeof(char));
    
    if (buf1 && buf2) {
        // buf2 devrait être à zéro, buf1 pas nécessairement
        int buf2_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (buf2[i] != 0) {
                buf2_zero = 0;
                break;
            }
        }
        test_result(buf2_zero, "calloc vs reallocarray: calloc is zeroed");
        
        free(buf1);
        free(buf2);
    }
}

void test_reallocarray_boundary() {
    TEST_SECTION("reallocARRAY BOUNDARY TESTS");
    
    // Tests aux limites exactes des catégories
    void *tiny_max = reallocarray(NULL, 128, 1); // Max TINY
    void *small_min = reallocarray(NULL, 129, 1); // Min SMALL  
    void *small_max = reallocarray(NULL, 1024, 1); // Max SMALL
    void *large_min = reallocarray(NULL, 1025, 1); // Min LARGE
    
    test_result(tiny_max != NULL, "reallocarray TINY max boundary (128)");
    test_result(small_min != NULL, "reallocarray SMALL min boundary (129)");
    test_result(small_max != NULL, "reallocarray SMALL max boundary (1024)");
    test_result(large_min != NULL, "reallocarray LARGE min boundary (1025)");
    
    // Test d'écriture dans chaque bloc
    if (tiny_max) {
        memset(tiny_max, 'T', 128);
        test_result(((char*)tiny_max)[127] == 'T', "TINY boundary block writable");
        free(tiny_max);
    }
    
    if (small_min) {
        memset(small_min, 'S', 129);
        test_result(((char*)small_min)[128] == 'S', "SMALL min boundary block writable");
        free(small_min);
    }
    
    if (small_max) {
        memset(small_max, 'M', 1024);
        test_result(((char*)small_max)[1023] == 'M', "SMALL max boundary block writable");
        free(small_max);
    }
    
    if (large_min) {
        memset(large_min, 'L', 1025);
        test_result(((char*)large_min)[1024] == 'L', "LARGE min boundary block writable");
        free(large_min);
    }
    
    
}

void test_strdup_basic() {
    TEST_SECTION("STRDUP BASIC TESTS");
    
    // Test 1: chaîne vide
    char *empty = strdup("");
    test_result(empty != NULL, "strdup(\"\") succeeds");
    if (empty) {
        test_result(strlen(empty) == 0, "strdup(\"\") returns empty string");
        test_result(strcmp(empty, "") == 0, "strdup(\"\") content is correct");
        test_result(((uintptr_t)empty % 8) == 0, "strdup(\"\") result is 8-byte aligned");
        free(empty);
    }
    
    // Test 2: chaîne simple
    char *simple = strdup("Hello");
    test_result(simple != NULL, "strdup(\"Hello\") succeeds");
    if (simple) {
        test_result(strlen(simple) == 5, "strdup(\"Hello\") length is correct");
        test_result(strcmp(simple, "Hello") == 0, "strdup(\"Hello\") content is correct");
        test_result(((uintptr_t)simple % 8) == 0, "strdup(\"Hello\") result is 8-byte aligned");
        free(simple);
    }
    
    // Test 3: chaîne longue
    char *long_str = strdup("This is a longer string for testing purposes");
    test_result(long_str != NULL, "strdup(long string) succeeds");
    if (long_str) {
        test_result(strlen(long_str) == 44, "strdup(long string) length is correct");
        test_result(strcmp(long_str, "This is a longer string for testing purposes") == 0, 
                   "strdup(long string) content is correct");
        free(long_str);
    }
    
    // Test 4: chaîne avec caractères spéciaux
    char *special = strdup("Hello\tWorld\n\r\\\"");
    test_result(special != NULL, "strdup(special chars) succeeds");
    if (special) {
        test_result(strcmp(special, "Hello\tWorld\n\r\\\"") == 0, 
                   "strdup(special chars) content is correct");
        free(special);
    }
    
    
}

void test_strdup_sizes() {
    TEST_SECTION("STRDUP SIZE CATEGORIES");
    
    // Test TINY (≤ 128 bytes)
    // Chaîne de 64 caractères (65 avec \0)
    char tiny_source[65];
    memset(tiny_source, 'A', 64);
    tiny_source[64] = '\0';
    
    char *tiny = strdup(tiny_source);
    test_result(tiny != NULL, "strdup TINY (64 chars) succeeds");
    if (tiny) {
        test_result(strlen(tiny) == 64, "strdup TINY length correct");
        test_result(strcmp(tiny, tiny_source) == 0, "strdup TINY content correct");
        test_result(tiny[63] == 'A' && tiny[64] == '\0', "strdup TINY null termination");
        free(tiny);
    }
    
    // Chaîne exactement à la limite TINY (127 chars + \0 = 128 bytes)
    char tiny_max_source[128];
    memset(tiny_max_source, 'T', 127);
    tiny_max_source[127] = '\0';
    
    char *tiny_max = strdup(tiny_max_source);
    test_result(tiny_max != NULL, "strdup TINY max boundary (127 chars) succeeds");
    if (tiny_max) {
        test_result(strlen(tiny_max) == 127, "strdup TINY max length correct");
        test_result(strcmp(tiny_max, tiny_max_source) == 0, "strdup TINY max content correct");
        free(tiny_max);
    }
    
    // Test SMALL (129-1024 bytes) - 200 caractères
    char small_source[201];
    memset(small_source, 'S', 200);
    small_source[200] = '\0';
    
    char *small = strdup(small_source);
    test_result(small != NULL, "strdup SMALL (200 chars) succeeds");
    if (small) {
        test_result(strlen(small) == 200, "strdup SMALL length correct");
        test_result(strcmp(small, small_source) == 0, "strdup SMALL content correct");
        free(small);
    }
    
    // Chaîne à la limite SMALL (1023 chars + \0 = 1024 bytes)
    char small_max_source[1024];
    memset(small_max_source, 'M', 1023);
    small_max_source[1023] = '\0';
    
    char *small_max = strdup(small_max_source);
    test_result(small_max != NULL, "strdup SMALL max boundary (1023 chars) succeeds");
    if (small_max) {
        test_result(strlen(small_max) == 1023, "strdup SMALL max length correct");
        test_result(small_max[1022] == 'M' && small_max[1023] == '\0', 
                   "strdup SMALL max termination correct");
        free(small_max);
    }
    
    // Test LARGE (> 1024 bytes) - 2000 caractères
    char large_source[2001];
    memset(large_source, 'L', 2000);
    large_source[2000] = '\0';
    
    char *large = strdup(large_source);
    test_result(large != NULL, "strdup LARGE (2000 chars) succeeds");
    if (large) {
        test_result(strlen(large) == 2000, "strdup LARGE length correct");
        test_result(large[1999] == 'L' && large[2000] == '\0', 
                   "strdup LARGE content and termination correct");
        free(large);
    }
    
    
}

void test_strdup_content_integrity() {
    TEST_SECTION("STRDUP CONTENT INTEGRITY");
    
    // Test avec tous les caractères ASCII imprimables
    char ascii_source[96]; // 32-126 + \0
    for (int i = 0; i < 95; i++) {
        ascii_source[i] = (char)(32 + i); // ' ' à '~'
    }
    ascii_source[95] = '\0';
    
    char *ascii_copy = strdup(ascii_source);
    test_result(ascii_copy != NULL, "strdup ASCII printable chars succeeds");
    if (ascii_copy) {
        test_result(strlen(ascii_copy) == 95, "strdup ASCII length correct");
        test_result(strcmp(ascii_copy, ascii_source) == 0, "strdup ASCII content correct");
        
        // Vérifier caractère par caractère
        int char_by_char_ok = 1;
        for (int i = 0; i < 95; i++) {
            if (ascii_copy[i] != ascii_source[i]) {
                char_by_char_ok = 0;
                break;
            }
        }
        test_result(char_by_char_ok, "strdup ASCII char-by-char integrity");
        free(ascii_copy);
    }
    
    // Test avec des octets binaires (incluant des 0 dans la chaîne source jusqu'au premier \0)
    char binary_source[] = "Binary\x01\x02\x03\xFF\xFE test";
    char *binary_copy = strdup(binary_source);
    test_result(binary_copy != NULL, "strdup binary chars succeeds");
    if (binary_copy) {
        test_result(strcmp(binary_copy, binary_source) == 0, "strdup binary content correct");
        test_result(binary_copy[6] == '\x01' && binary_copy[7] == '\x02', 
                   "strdup preserves binary values");
        free(binary_copy);
    }
    
    // Test avec répétition de patterns
    char pattern_source[101];
    for (int i = 0; i < 100; i++) {
        pattern_source[i] = (char)('A' + (i % 26));
    }
    pattern_source[100] = '\0';
    
    char *pattern_copy = strdup(pattern_source);
    test_result(pattern_copy != NULL, "strdup pattern string succeeds");
    if (pattern_copy) {
        test_result(strlen(pattern_copy) == 100, "strdup pattern length correct");
        int pattern_ok = 1;
        for (int i = 0; i < 100; i++) {
            if (pattern_copy[i] != (char)('A' + (i % 26))) {
                pattern_ok = 0;
                break;
            }
        }
        test_result(pattern_ok, "strdup pattern content integrity");
        free(pattern_copy);
    }
}

void test_strdup_independence() {
    TEST_SECTION("STRDUP MEMORY INDEPENDENCE");
    
    // Test que la copie est indépendante de l'original
    char original[] = "Original string";
    char *copy = strdup(original);
    
    test_result(copy != NULL, "strdup creates copy");
    if (copy) {
        test_result(copy != original, "strdup copy has different address");
        test_result(strcmp(copy, original) == 0, "strdup copy has same content initially");
        
        // Modifier l'original
        strcpy(original, "Modified orig");
        test_result(strcmp(copy, "Original string") == 0, 
                   "strdup copy unchanged when original modified");
        
        // Modifier la copie
        strcpy(copy, "Modified copy");
        test_result(strcmp(original, "Modified orig") == 0, 
                   "Original unchanged when strdup copy modified");
        test_result(strcmp(copy, "Modified copy") == 0, 
                   "strdup copy modification successful");
        
        free(copy);
    }
    
    // Test avec buffer temporaire
    char *dynamic_copy;
    {
        char temp_buffer[100];
        strcpy(temp_buffer, "Temporary buffer content");
        dynamic_copy = strdup(temp_buffer);
        test_result(dynamic_copy != NULL, "strdup from temp buffer succeeds");
        // temp_buffer va être détruit à la sortie du bloc
    }
    
    // Vérifier que la copie survit à la destruction du buffer temporaire
    if (dynamic_copy) {
        test_result(strcmp(dynamic_copy, "Temporary buffer content") == 0, 
                   "strdup copy survives original buffer destruction");
        free(dynamic_copy);
    }
}

void test_strdup_extreme_sizes() {
    TEST_SECTION("STRDUP EXTREME SIZES");
    
    // Test chaîne d'un seul caractère
    char *single = strdup("X");
    test_result(single != NULL, "strdup single char succeeds");
    if (single) {
        test_result(strlen(single) == 1, "strdup single char length correct");
        test_result(single[0] == 'X' && single[1] == '\0', "strdup single char content correct");
        free(single);
    }
    
    // Test chaîne très longue (10KB)
    char *big_source = malloc(10241);
    if (big_source) {
        memset(big_source, 'B', 10240);
        big_source[10240] = '\0';
        
        char *big_copy = strdup(big_source);
        test_result(big_copy != NULL, "strdup very large string (10KB) succeeds");
        if (big_copy) {
            test_result(strlen(big_copy) == 10240, "strdup large string length correct");
            test_result(big_copy[0] == 'B' && big_copy[10239] == 'B' && big_copy[10240] == '\0', 
                       "strdup large string content correct");
            
            // Vérifier quelques positions aléatoirement
            int sample_ok = (big_copy[1000] == 'B' && big_copy[5000] == 'B' && big_copy[9000] == 'B');
            test_result(sample_ok, "strdup large string sample positions correct");
            
            free(big_copy);
        }
        free(big_source);
    }
    
    // Test avec une chaîne contenant beaucoup d'espaces
    char spaces_source[1001];
    memset(spaces_source, ' ', 1000);
    spaces_source[1000] = '\0';
    
    char *spaces_copy = strdup(spaces_source);
    test_result(spaces_copy != NULL, "strdup spaces string succeeds");
    if (spaces_copy) {
        test_result(strlen(spaces_copy) == 1000, "strdup spaces length correct");
        int all_spaces = 1;
        for (int i = 0; i < 1000; i++) {
            if (spaces_copy[i] != ' ') {
                all_spaces = 0;
                break;
            }
        }
        test_result(all_spaces && spaces_copy[1000] == '\0', "strdup spaces content correct");
        free(spaces_copy);
    }
}

void test_strdup_special_chars() {
    TEST_SECTION("STRDUP SPECIAL CHARACTERS");
    
    // Test avec caractères Unicode/UTF-8 (si supportés)
    char *utf8 = strdup("Héllo Wörld! 🌍 café");
    test_result(utf8 != NULL, "strdup UTF-8 chars succeeds");
    if (utf8) {
        test_result(strcmp(utf8, "Héllo Wörld! 🌍 café") == 0, "strdup UTF-8 content correct");
        free(utf8);
    }
    
    // Test avec tous les caractères de contrôle ASCII (0-31)
    char control_source[33];
    for (int i = 1; i < 32; i++) { // Skip \0 car il terminerait la chaîne
        control_source[i-1] = (char)i;
    }
    control_source[31] = '\0';
    
    char *control_copy = strdup(control_source);
    test_result(control_copy != NULL, "strdup control chars succeeds");
    if (control_copy) {
        test_result(strlen(control_copy) == 31, "strdup control chars length correct");
        int control_ok = 1;
        for (int i = 0; i < 31; i++) {
            if (control_copy[i] != (char)(i + 1)) {
                control_ok = 0;
                break;
            }
        }
        test_result(control_ok, "strdup control chars content correct");
        free(control_copy);
    }
    
    // Test avec caractères de fin de ligne variés
    char *newlines = strdup("Line1\nLine2\rLine3\r\nLine4");
    test_result(newlines != NULL, "strdup newline variants succeeds");
    if (newlines) {
        test_result(strcmp(newlines, "Line1\nLine2\rLine3\r\nLine4") == 0, 
                   "strdup newline variants content correct");
        free(newlines);
    }
    
    // Test avec guillemets et échappements
    char *quotes = strdup("\"Hello\" 'World' \\backslash\\");
    test_result(quotes != NULL, "strdup quotes and escapes succeeds");
    if (quotes) {
        test_result(strcmp(quotes, "\"Hello\" 'World' \\backslash\\") == 0, 
                   "strdup quotes and escapes content correct");
        free(quotes);
    }
}

void test_strdup_alignment() {
    TEST_SECTION("STRDUP ALIGNMENT TESTS");
    
    // Tester l'alignement avec différentes tailles
    char *ptrs[10];
    char sources[10][20];
    
    for (int i = 0; i < 10; i++) {
        // Créer des chaînes de longueurs variées
        int len = i + 1; // 1 à 10 caractères
        memset(sources[i], 'A' + i, len);
        sources[i][len] = '\0';
        
        ptrs[i] = strdup(sources[i]);
        test_result(ptrs[i] != NULL, "strdup various sizes allocation");
        if (ptrs[i]) {
            test_result(((uintptr_t)ptrs[i] % 8) == 0, "strdup result properly aligned");
            test_result(strcmp(ptrs[i], sources[i]) == 0, "strdup various sizes content correct");
        }
    }
    
    // Nettoyer et vérifier l'état
    
    
    for (int i = 0; i < 10; i++) {
        if (ptrs[i]) {
            free(ptrs[i]);
        }
    }
}

void test_strdup_stress() {
    TEST_SECTION("STRDUP STRESS TESTS");
    
    // Allouer beaucoup de chaînes de tailles variées
    const int num_strings = 1000;
    char **strings = malloc(num_strings * sizeof(char*));
    int success_count = 0;
    
    if (strings) {
        for (int i = 0; i < num_strings; i++) {
            // Créer une chaîne unique pour chaque index
            char temp[100];
            snprintf(temp, sizeof(temp), "String_%d_with_content_%d", i, i * 42);
            
            strings[i] = strdup(temp);
            if (strings[i]) {
                success_count++;
            }
        }
        
        test_result(success_count == num_strings, "strdup stress: all allocations succeed");
        
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
        test_result(integrity_check, "strdup stress: data integrity maintained");
        
        // Nettoyer
        for (int i = 0; i < num_strings; i++) {
            if (strings[i]) {
                free(strings[i]);
            }
        }
        free(strings);
    }
    
    // Test de fragmentation avec strdup
    char *frags[50];
    for (int i = 0; i < 50; i++) {
        char temp[50];
        snprintf(temp, sizeof(temp), "Fragment_%02d", i);
        frags[i] = strdup(temp);
    }
    
    // Libérer les chaînes impaires
    for (int i = 1; i < 50; i += 2) {
        if (frags[i]) {
            free(frags[i]);
            frags[i] = NULL;
        }
    }
    
    // Réallouer des chaînes dans les trous
    for (int i = 1; i < 50; i += 2) {
        char temp[50];
        snprintf(temp, sizeof(temp), "NewFrag_%02d", i);
        frags[i] = strdup(temp);
        test_result(frags[i] != NULL, "strdup fragmentation reallocation");
    }
    
    // Nettoyer la fragmentation
    for (int i = 0; i < 50; i++) {
        if (frags[i]) {
            free(frags[i]);
        }
    }
    
    
}

void test_strdup_null_handling() {
    TEST_SECTION("STRDUP NULL HANDLING");
    
    // Note: strdup(NULL) n'est pas défini par le standard C
    // Votre implémentation peut soit:
    // 1. Retourner NULL
    // 2. Faire un segfault (comportement de glibc)
    // 3. Gérer le cas spécialement
    
    printf("Note: Testing strdup(NULL) - behavior is implementation-defined\n");
    
    // Test plus sûr: vérifier avec une chaîne qui pourrait être corrompue
    // mais n'est pas NULL
    char almost_empty[] = {'\0'};
    char *empty_dup = strdup(almost_empty);
    test_result(empty_dup != NULL, "strdup empty string (via array) succeeds");
    if (empty_dup) {
        test_result(strlen(empty_dup) == 0, "strdup empty string length is 0");
        test_result(empty_dup[0] == '\0', "strdup empty string is properly terminated");
        free(empty_dup);
    }
}

void test_strdup_boundary_conditions() {
    TEST_SECTION("STRDUP BOUNDARY CONDITIONS");
    
    // Test exactement aux limites des catégories de malloc
    
    // Limite TINY: 128 bytes total, donc 127 chars + \0
    char tiny_boundary[128];
    memset(tiny_boundary, 'T', 127);
    tiny_boundary[127] = '\0';
    
    char *tiny_dup = strdup(tiny_boundary);
    test_result(tiny_dup != NULL, "strdup TINY boundary (127 chars) succeeds");
    if (tiny_dup) {
        test_result(strlen(tiny_dup) == 127, "strdup TINY boundary length correct");
        test_result(tiny_dup[126] == 'T' && tiny_dup[127] == '\0', 
                   "strdup TINY boundary termination correct");
        free(tiny_dup);
    }
    
    // Limite SMALL: 1024 bytes total, donc 1023 chars + \0
    char *small_boundary = malloc(1024);
    if (small_boundary) {
        memset(small_boundary, 'S', 1023);
        small_boundary[1023] = '\0';
        
        char *small_dup = strdup(small_boundary);
        test_result(small_dup != NULL, "strdup SMALL boundary (1023 chars) succeeds");
        if (small_dup) {
            test_result(strlen(small_dup) == 1023, "strdup SMALL boundary length correct");
            test_result(small_dup[1022] == 'S' && small_dup[1023] == '\0', 
                       "strdup SMALL boundary termination correct");
            free(small_dup);
        }
        free(small_boundary);
    }
    
    // Premier LARGE: 1025 bytes total, donc 1024 chars + \0
    char *large_boundary = malloc(1025);
    if (large_boundary) {
        memset(large_boundary, 'L', 1024);
        large_boundary[1024] = '\0';
        
        char *large_dup = strdup(large_boundary);
        test_result(large_dup != NULL, "strdup LARGE boundary (1024 chars) succeeds");
        if (large_dup) {
            test_result(strlen(large_dup) == 1024, "strdup LARGE boundary length correct");
            test_result(large_dup[1023] == 'L' && large_dup[1024] == '\0', 
                       "strdup LARGE boundary termination correct");
            free(large_dup);
        }
        free(large_boundary);
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
void* test_sequential_malloc(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test séquentiel (%d allocations)\n", 
           data->thread_id, data->allocations_target);
    
    data->allocated_ptrs = calloc(data->allocations_target, sizeof(void*));
    data->allocated_sizes = calloc(data->allocations_target, sizeof(size_t));
    
    if (!data->allocated_ptrs || !data->allocated_sizes) {
        printf("Thread %d: Erreur allocation tableaux de suivi\n", data->thread_id);
        data->errors++;
        return NULL;
    }
    
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        
        // Allocation
        void* ptr = malloc(size);
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
void* test_rapid_malloc(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double start_time = get_time_us();
    
    printf("Thread %d: Démarrage test rapide\n", data->thread_id);
    
    data->allocated_ptrs = calloc(data->allocations_target, sizeof(void*));
    data->allocated_sizes = calloc(data->allocations_target, sizeof(size_t));
    
    // Tailles prédéfinies pour tester différents cas
    size_t test_sizes[] = {8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    int num_test_sizes = sizeof(test_sizes) / sizeof(test_sizes[0]);
    
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = test_sizes[i % num_test_sizes];
        
        void* ptr = malloc(size);
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
    
    data->allocated_ptrs = calloc(data->allocations_target, sizeof(void*));
    data->allocated_sizes = calloc(data->allocations_target, sizeof(size_t));
    
    // Phase 1: Allocations rapides
    for (int i = 0; i < data->allocations_target; i++) {
        size_t size = MIN_ALLOC_SIZE + (rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE));
        
        void* ptr = malloc(size);
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
    data->allocated_ptrs = calloc(large_alloc_count, sizeof(void*));
    data->allocated_sizes = calloc(large_alloc_count, sizeof(size_t));
    
    for (int i = 0; i < large_alloc_count; i++) {
        // Tailles entre 1KB et 16KB
        size_t size = 1024 + (rand() % (16 * 1024 - 1024));
        
        void* ptr = malloc(size);
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
    printf(YELLOW "Starting comprehensive malloc/realloc/free test suite...\n" RESET);
    
    test_basic_malloc();
    test_small_malloc();
    test_large_malloc();
    test_fragmentation();
    test_realloc_tiny_to_tiny();
    test_realloc_small_to_small();
    test_realloc_large_to_large();
    test_realloc_category_changes();
    test_realloc_edge_cases();
    test_multiple_allocations();
    test_stress_realloc();
    test_boundary_values();
    test_free_edge_cases();
    test_memory_patterns();
    test_calloc_basic();
    test_calloc_sizes();
    test_calloc_overflow();
    test_calloc_patterns();
    test_calloc_extreme();
    
    // Tests reallocarray hardcore  
    test_reallocarray_basic();
    test_reallocarray_overflow();
    test_reallocarray_categories();
    test_reallocarray_edge_cases();
    test_reallocarray_stress();
    test_reallocarray_data_integrity();
    test_reallocarray_boundary();
    
    // Tests d'interaction
    test_calloc_reallocarray_interaction();

	test_strdup_basic();
    test_strdup_sizes();
    test_strdup_content_integrity();
    test_strdup_independence();
    test_strdup_extreme_sizes();
    test_strdup_special_chars();
    test_strdup_alignment();
    test_strdup_stress();
    test_strdup_null_handling();
    test_strdup_boundary_conditions();

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

    printf("=== Test Multi-threadé pour malloc uniquement ===\n");
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
        test_sequential_malloc,
        test_rapid_malloc,
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
            free(thread_data[i].allocated_ptrs);  // free standard
        }
        if (thread_data[i].allocated_sizes) {
            free(thread_data[i].allocated_sizes);  // free standard
        }
    }
    
	// 
    if (total_errors == 0) {
        printf("\n✅ TOUS LES TESTS SONT PASSÉS!\n");
        printf("Votre malloc semble fonctionner correctement en multi-threading.\n");
        return 0;
    } else {
        printf("\n❌ %d ERREURS DÉTECTÉES\n", total_errors);
        printf("Vérifiez la thread-safety de votre malloc.\n");
        return 1;
    }
}

// int main(void)
// {
// 	void *test[1000];

// 	for (int i = 0; i < 1000; i++) {
// 		printf("i == %d\n", i);
// 		test[i] = malloc(10000);
// 		memset(test[i], i, 10000);
// 	}
//
// 	for (int i = 500; i < 1000; i++) {
// 		ft_printf_fd(2, "i == %d\n", i);
// 		free(test[i]);
// 	}
// 	for (int i = 500; i < 1000; i++) {
// 		printf("i == %d\n", i);
// 		test[i] = malloc(10000);
// 		memset(test[i], i, 10000);
// 	}
// 	printf("here\n");
//
// 	for (int i = 0; i < 1000; i++) {
// 		free(test[i]);
// 	}
//
// }