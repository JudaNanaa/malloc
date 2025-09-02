/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   add_string_char_2d.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/16 02:05:39 by madamou           #+#    #+#             */
/*   Updated: 2024/12/16 02:07:07 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	add_string_char_2d(char ***tabb, char *str)
{
	char	**new;
	int		i;
	char	**buff;

	buff = *tabb;
	new = malloc(sizeof(char *) * (ft_strlen_2d(buff) + 1 + 1));
	if (!new)
		return (-1);
	i = 0;
	while (buff && buff[i])
	{
		new[i] = buff[i];
		i++;
	}
	new[i] = str;
	new[++i] = NULL;
	free(buff);
	*tabb = new;
	return (0);
}
