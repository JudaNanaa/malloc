/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lib_malloc.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 22:09:42 by madamou           #+#    #+#             */
/*   Updated: 2025/09/14 01:01:29 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIB_MALLOC_H
# define LIB_MALLOC_H

# include <stddef.h>
# include <sys/mman.h>
# include <unistd.h>

void	*my_malloc(size_t size);

void	my_free(void *ptr);

void	*realloc(void *ptr, size_t size);

void	*reallocarray(void *ptr, size_t nmemb, size_t size);

void	*calloc(size_t nmemb, size_t size);

void	show_alloc_mem(void);

void	show_alloc_mem_ex(void);

char	*strdup(const char *s);

#endif