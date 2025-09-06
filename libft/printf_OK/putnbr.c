/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   putnbr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:56:20 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:18:02 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

void	ft_putnbr_base(long long int nbr, char *base, int fd)
{
	char	char_nbr;
	int		base_len;

	base_len = ft_strlen(base);
	if (nbr < 0)
	{
		write(fd, "-", 1);
		nbr = -nbr;
	}
	if (nbr >= base_len)
		ft_putnbr_base(nbr / base_len, base, fd);
	char_nbr = base[nbr % base_len];
	write(fd, &char_nbr, 1);
}
