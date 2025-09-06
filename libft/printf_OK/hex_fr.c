/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hex_fr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 16:30:22 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:17:37 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	hex_fr(va_list args, int cas, int fd)
{
	unsigned int	arg;

	arg = va_arg(args, unsigned int);
	if (cas == 1)
		ft_putnbr_base(arg, "0123456789abcdef", fd);
	else
		ft_putnbr_base(arg, "0123456789ABCDEF", fd);
	return (convert_len(arg, "0123456789abcdef"));
}
