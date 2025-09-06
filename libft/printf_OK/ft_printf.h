/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:26 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 03:26:14 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
# include <unistd.h>
# include "../libft.h"

int		convert_len(long long int nbr, char *base);
void	ft_putnbr_base(long long int nbr, char *base);

int		ft_printf(const char *format, ...);
int		monitoring(va_list args, char *c);

void	print_mem(unsigned long long int nbr);
int		hex_fr(va_list args, int cas);
int		car_fr(va_list args);
int		mem_fr(va_list args);
int		int_fr(va_list args);
int		str_fr(va_list args);
int		uns_fr(va_list args);

#endif