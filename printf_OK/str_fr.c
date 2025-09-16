/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   str_fr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:44:32 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:18:44 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_putstr_f(char *str, int fd)
{
	int	i;

	i = 0;
	while (str[i])
	{
		write(fd, &str[i], 1);
		i++;
	}
	return (i);
}

int	str_fr(va_list args, int fd)
{
	char	*str;

	str = va_arg(args, char *);
	if (!str)
		return (ft_putstr_f("(null)", fd));
	return (ft_putstr_f(str, fd));
}
