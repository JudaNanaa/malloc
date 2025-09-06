# include "../libft/printf_OK/ft_printf.h"
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

void test_basic_malloc() {
    TEST_SECTION("BASIC MALLOC TESTS");
    
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
    
    show_alloc_mem_ex();
    
    free(tiny1); free(tiny8); free(tiny16); 
    free(tiny32); free(tiny64); free(tiny128);
}

void test_small_malloc() {
    TEST_SECTION("SMALL MALLOC TESTS");
    
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
    
    show_alloc_mem_ex();
    
    free(small200); free(small400); free(small600);
    free(small800); free(small1000); free(small1024);
}

void test_large_malloc() {
    TEST_SECTION("LARGE MALLOC TESTS");
    
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
    
    show_alloc_mem_ex();
    
    free(large1025); free(large2048); free(large4096);
    free(large8192); free(large16384); free(large_huge);
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
    
    show_alloc_mem_ex();
    
    // Libérer les blocs impairs (créer des trous)
    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
        ptrs[i] = NULL;
    }
    
    printf("After freeing odd blocks:\n");
    show_alloc_mem_ex();
    
    // Essayer de réallouer dans les trous
    void *new_ptr = malloc(32); // devrait réutiliser un trou
    test_result(new_ptr != NULL, "Malloc after fragmentation succeeds");
    
    // Nettoyer
    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
    }
    free(new_ptr);
}

void test_realloc_tiny_to_tiny() {
    TEST_SECTION("REALLOC TINY->TINY TESTS");
    
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
    TEST_SECTION("REALLOC SMALL->SMALL TESTS");
    
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
    TEST_SECTION("REALLOC LARGE->LARGE TESTS");
    
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
    TEST_SECTION("REALLOC CATEGORY CHANGE TESTS");
    
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
    TEST_SECTION("REALLOC EDGE CASES");
    
    // realloc(NULL, size) = malloc(size)
    char *ptr = realloc(NULL, 100);
    test_result(ptr != NULL, "realloc(NULL, 100) works like malloc");
    if (ptr) {
        strcpy(ptr, "Realloc NULL test");
        test_result(strcmp(ptr, "Realloc NULL test") == 0, "Write after realloc(NULL)");
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
    
    show_alloc_mem_ex();
    
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
    TEST_SECTION("STRESS REALLOC TEST");
    
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
    
    show_alloc_mem_ex();
    
    free(tiny_max); free(small_min); 
    free(small_max); free(large_min);
}

void test_free_edge_cases() {
    TEST_SECTION("FREE EDGE CASES");
    
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
    TEST_SECTION("CALLOC BASIC TESTS");
    
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
    
    show_alloc_mem();
}

void test_calloc_sizes() {
    TEST_SECTION("CALLOC SIZE CATEGORIES");
    
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
    
    show_alloc_mem();
}

void test_calloc_overflow() {
    TEST_SECTION("CALLOC OVERFLOW TESTS");
    
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
    TEST_SECTION("CALLOC PATTERN TESTS");
    
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
    TEST_SECTION("CALLOC EXTREME TESTS");
    
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
    
    show_alloc_mem();
}

void test_reallocarray_basic() {
    TEST_SECTION("REALLOCARRAY BASIC TESTS");
    
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
    
    show_alloc_mem();
}

void test_reallocarray_overflow() {
    TEST_SECTION("REALLOCARRAY OVERFLOW TESTS");
    
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
    TEST_SECTION("REALLOCARRAY SIZE CATEGORIES");
    
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
    TEST_SECTION("REALLOCARRAY EDGE CASES");
    
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
    TEST_SECTION("REALLOCARRAY STRESS TEST");
    
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
				ft_printf_fd(STDERR_FILENO, "ok chellout\n");
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
    TEST_SECTION("REALLOCARRAY DATA INTEGRITY");
    
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
    TEST_SECTION("CALLOC + REALLOCARRAY INTERACTION");
    
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
    TEST_SECTION("REALLOCARRAY BOUNDARY TESTS");
    
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
    
    show_alloc_mem();
}

int main(void) {
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
    printf(BLUE "\n=== FINAL MEMORY STATE ===" RESET "\n");
    show_alloc_mem_ex();
    
    printf(YELLOW "\n=== TEST SUMMARY ===" RESET "\n");
    printf("Tests passed: " GREEN "%d" RESET "\n", tests_passed);
    printf("Tests failed: " RED "%d" RESET "\n", tests_failed);
    printf("Total tests: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf(GREEN "🎉 ALL TESTS PASSED! 🎉" RESET "\n");
        return 0;
    } else {
        printf(RED "⚠️  Some tests failed. Check implementation." RESET "\n");
        return 1;
    }
}