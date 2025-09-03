/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 03:27:14 by madamou           #+#    #+#             */
/*   Updated: 2025/09/03 03:34:59 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/malloc_internal.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

t_malloc	g_malloc = {NULL, NULL, NULL};

void	initialize_blocks(t_page *new_page, int block_size)
{
	void	*begin;
	int		i;

	i = 0;
	begin = (void *)new_page + sizeof(t_page);
	while (i < NB_BLOCK)
	{
		new_page->blocks[i].ptr = begin;
		new_page->blocks[i].size = 0;
		new_page->blocks[i].is_free = true;
		begin += block_size;
		i++;
	}
}

t_page	*create_page(size_t length, int block_size)
{
	t_page	*new_page;

	new_page = mmap(0, length, PROT_READ | PROT_WRITE,
			MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
	if (new_page == MAP_FAILED)
		return (NULL);
	new_page->nb_alloc = 0;
	new_page->next = NULL;
	ft_printf("le mmap a reussi\n");
	initialize_blocks(new_page, block_size);
	return (new_page);
}

t_block	*get_first_free_block(t_page *page)
{
	int	i;

	i = 0;
	while (page->blocks[i].is_free == false)
	{
		i++;
	}
	return (&page->blocks[i]);
}

t_page	*search_page_with_space(t_page *pages)
{
	t_page	*current_page;

	current_page = pages;
	while (current_page)
	{
		if (current_page->nb_alloc < NB_BLOCK)
			return (current_page);
		current_page = current_page->next;
	}
	return (NULL);
}

void	add_back_page_list(t_page **first, t_page *new)
{
	t_page	*current;

	if (*first == NULL)
	{
		*first = new;
	}
	else
	{
		current = *first;
		while (current->next)
		{
			current = current->next;
		}
		current->next = new;
	}
}

void	*optimized_malloc(t_page **malloc_page, size_t block_size, size_t size)
{
	int		page_size;
	t_page	*page;
	t_block	*block;

	page_size = sysconf(_SC_PAGESIZE);
	page = search_page_with_space(*malloc_page);
	if (page)
	{
		block = get_first_free_block(page);
		block->is_free = false;
		block->size = size;
		page->nb_alloc++;
		return (block->ptr);
	}
	page = create_page(sizeof(t_page) + (page_size * block_size), block_size);
	if (page == NULL)
		return (NULL);
	page->blocks[0].size = size;
	page->blocks[0].is_free = false;
	page->nb_alloc++;
	add_back_page_list(malloc_page, page);
	return (page->blocks[0].ptr);
}

void	*large_malloc(size_t size)
{
	ft_printf("je suis dans le large malloc\n");
	ft_printf("size == %u\n", size);
	return (NULL);
}

void	*malloc(size_t size)
{
	if (size <= n)
		return (optimized_malloc(&g_malloc.tiny, n, size));
	else if (size <= m)
		return (optimized_malloc(&g_malloc.small, n, size));
	else
		return (large_malloc(size));
	return (NULL);
}
