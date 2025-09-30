#include "../../includes/malloc_internal.h"
#include "rbTree.h"

t_block *minimum(t_block *node, t_block *NIL)
{
    while (node->left != NIL)
        node = node->left;
    return node;
}

void transplant(t_block **root, t_block *u, t_block *v)
{
    if (u == *root)
    {
        *root = v;
    }
    else if (u->parent->left == u)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

void fix_delete(t_block **root, t_block *node, t_block *NIL)
{
    t_block *sibling;

    while (node != *root && (node == NIL || IS_BLOCK_BLACK(node))) {
        if (node == node->parent->left) // node est enfant gauche
        {
            sibling = node->parent->right;

            // Cas 1: Sibling rouge
            if (IS_BLOCK_RED(sibling))
            {
                SET_BLOCK_BLACK(sibling);
                SET_BLOCK_RED(node->parent);
                left_rotation(root, node->parent, NIL);
                sibling = node->parent->right; // mise à jour du sibling
            }

            // Cas 2: Sibling noir avec enfants noirs
            if (sibling == NIL || ((sibling->left == NIL || IS_BLOCK_BLACK(sibling->left)) &&
                                   (sibling->right == NIL || IS_BLOCK_BLACK(sibling->right))))
            {
                SET_BLOCK_RED(sibling);
                node = node->parent;
            }
            else
            {
                // Cas 3: Sibling noir, enfant droit noir, enfant gauche rouge
                if (sibling->right == NIL || IS_BLOCK_BLACK(sibling->right))
                {
                    if (sibling->left != NIL)
                        SET_BLOCK_BLACK(sibling->left);
                    SET_BLOCK_RED(sibling);
                    right_rotation(root, sibling, NIL);
                    sibling = node->parent->right;
                }

                // Cas 4: Sibling noir, enfant droit rouge
                COPY_BLOCK_COLOR(sibling, node->parent);
                SET_BLOCK_BLACK(node->parent);
                if (sibling->right != NIL)
                    SET_BLOCK_BLACK(sibling->right);
                left_rotation(root, node->parent, NIL);
                node = *root; // sortie de la boucle
            }
        }
        else // node est enfant droit (symétrique)
        {
            sibling = node->parent->left;

            // Cas 1: Sibling rouge
            if (IS_BLOCK_RED(sibling))
            {
                SET_BLOCK_BLACK(sibling);
                SET_BLOCK_RED(node->parent);
                right_rotation(root, node->parent, NIL);
                sibling = node->parent->left; // mise à jour du sibling
            }

            // Cas 2: Sibling noir avec enfants noirs
            if (sibling == NIL || ((sibling->left == NIL || IS_BLOCK_BLACK(sibling->left)) &&
                                   (sibling->right == NIL || IS_BLOCK_BLACK(sibling->right))))
            {
                SET_BLOCK_RED(sibling);
                node = node->parent;
            }
            else
            {
                // Cas 3: Sibling noir, enfant gauche noir, enfant droit rouge
                if (sibling->left == NIL || IS_BLOCK_BLACK(sibling->left))
                {
                    if (sibling->right != NIL)
                        SET_BLOCK_BLACK(sibling->right);
                    SET_BLOCK_RED(sibling);
                    left_rotation(root, sibling, NIL);
                    sibling = node->parent->left;
                }

                // Cas 4: Sibling noir, enfant gauche rouge
                COPY_BLOCK_COLOR(sibling, node->parent);
                SET_BLOCK_BLACK(node->parent);
                if (sibling->left != NIL)
                    SET_BLOCK_BLACK(sibling->left);
                right_rotation(root, node->parent, NIL);
                node = *root; // sortie de la boucle
            }
        }
    }

    if (node != NIL)
        SET_BLOCK_BLACK(node);
}


bool check_if_same(t_block **root, t_block *to_del, t_block *NIL)
{
	t_block *prev;
	t_block *next;

	if (to_del->same_prev == NULL && to_del->same_next == NULL)
		return false;

	prev = to_del->same_prev;
	next = to_del->same_next;
	if (prev == NULL)
	{
		COPY_BLOCK_COLOR(next, to_del);
		next->left = to_del->left;
		next->right = to_del->right;
		next->parent = to_del->parent;
		next->same_prev = NULL;
		if (next->left && next->left != NIL)
			next->left->parent = next;
		if (next->right && next->right != NIL)
			next->right->parent = next;
		if (next->parent != NIL)
		{
			if (to_del == next->parent->left)
				next->parent->left = next;
			else
				next->parent->right = next;
		}
		else
			*root = next;
	}
	else
		prev->same_next = next;
	if (next)
	{
		next->same_prev = prev;
	}
	to_del->same_next = NULL;
	to_del->same_prev = NULL;

	to_del->parent = NULL;
	to_del->left = NULL;
	to_del->right = NULL;
	SET_BLOCK_BLACK(NIL);
	return true;
}

void delete_node_tree(t_block **root, t_block *to_del, t_block *NIL)
{
    t_block *replacement;
    t_block *fix_node;
	bool is_black = IS_BLOCK_BLACK(to_del);

	if (check_if_same(root, to_del, NIL))
		return;
    if (to_del->left == NIL)
    {
        fix_node = to_del->right;
        transplant(root, to_del, to_del->right);
    }
    else if (to_del->right == NIL)
    {
        fix_node = to_del->left;
        transplant(root, to_del, to_del->left);
    }
    else
    {
        replacement = minimum(to_del->right, NIL);
		is_black = IS_BLOCK_BLACK(replacement);
        fix_node = replacement->right;
        
        if (replacement->parent == to_del)
                fix_node->parent = replacement;
        else
        {
            transplant(root, replacement, replacement->right);
            replacement->right = to_del->right;
            if (replacement->right != NIL)
                replacement->right->parent = replacement;
        }
        
        transplant(root, to_del, replacement);
        replacement->left = to_del->left;
        if (replacement->left != NIL)
            replacement->left->parent = replacement;
		COPY_BLOCK_COLOR(replacement, to_del);
    }

    if (is_black && *root != NIL)
    {
        fix_delete(root, fix_node, NIL);
    }
    
    if (*root != NIL)
		SET_BLOCK_BLACK(*root);
        
    if (NIL != NULL)
		NIL->parent = NULL;
	
	to_del->same_next = NULL;
	to_del->same_prev = NULL;

	to_del->parent = NULL;
	to_del->left = NULL;
	to_del->right = NULL;
	SET_BLOCK_BLACK(NIL);
}