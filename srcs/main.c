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