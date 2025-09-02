/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_file.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/05 20:03:40 by madamou           #+#    #+#             */
/*   Updated: 2024/12/02 02:49:33 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*read_file(int fd)
{
	char	*temp;
	char	*str;

	temp = get_next_line(fd);
	if (temp == NULL)
		return (NULL);
	str = NULL;
	while (temp != NULL)
	{
		str = ft_re_strjoin(str, temp);
		free(temp);
		if (str == NULL)
			return (NULL);
		temp = get_next_line(fd);
	}
	return (str);
}
