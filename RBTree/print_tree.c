#include "RBTree.h"
#include <stdio.h>

// couleurs ANSI
#define RED_TEXT   "\033[31m"
#define BLACK_TEXT "\033[30m"
#define RESET_TEXT "\033[0m"

// affichage style arbre (rotated tree print)
static void print_rbtree(const t_rbtree *node, int depth)
{
    if (node == NULL)
        return;

    // d'abord la branche droite
    print_rbtree(node->right, depth + 1);

    // indentation
    for (int i = 0; i < depth; i++)
        printf("    ");

    // couleur + valeur
    if (node->color == RED)
        printf(RED_TEXT "[%zu:R]" RESET_TEXT "\n", node->size);
    else
        printf(BLACK_TEXT "[%zu:B]" RESET_TEXT "\n", node->size);

    // puis la branche gauche
    print_rbtree(node->left, depth + 1);
}

void print_rbtree_root(const t_rbtree *root)
{
    printf("Red-Black Tree:\n");
    print_rbtree(root, 0);
}