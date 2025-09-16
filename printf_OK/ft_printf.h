/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:12:26 by itahri            #+#    #+#             */
/*   Updated: 2025/09/16 17:30:12 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
# include <unistd.h>

int		convert_len(long long int nbr, char *base);
void	ft_putnbr_base(long long int nbr, char *base, int fd);

int		ft_printf_fd(int fd, const char *format, ...);
int		monitoring(va_list args, char *c, int fd);

void	print_mem(unsigned long long int nbr, int fd);
int		hex_fr(va_list args, int cas, int fd);
int		car_fr(va_list args, int fd);
int		mem_fr(va_list args, int fd);
int		int_fr(va_list args, int fd);
int		str_fr(va_list args, int fd);
int		uns_fr(va_list args, int fd);
size_t	ft_strlen(const char *s);

#endif