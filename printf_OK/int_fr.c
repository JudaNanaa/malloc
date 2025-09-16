/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   int_fr.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:50:29 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:17:48 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	int_fr(va_list args, int fd)
{
	int	arg;

	arg = va_arg(args, int);
	ft_putnbr_base(arg, "0123456789", fd);
	return (convert_len(arg, "0123456789"));
}
