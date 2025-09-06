/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:22:25 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:12:58 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	monitoring(va_list args, char *c, int fd)
{
	c++;
	if (*c == 'c')
		return (car_fr(args, fd));
	else if (*c == 's')
		return (str_fr(args, fd));
	else if (*c == 'd' || *c == 'i')
		return (int_fr(args, fd));
	else if (*c == 'p')
		return (mem_fr(args, fd));
	else if (*c == 'u')
		return (uns_fr(args, fd));
	else if (*c == 'x')
		return (hex_fr(args, 1, fd));
	else if (*c == 'X')
		return (hex_fr(args, 2, fd));
	else if (*c == '%')
		return (write(fd, "%", 1), 1);
	return (0);
}
