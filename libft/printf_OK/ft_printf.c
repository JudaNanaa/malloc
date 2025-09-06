/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: madamou <madamou@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 15:11:59 by itahri            #+#    #+#             */
/*   Updated: 2025/09/06 19:12:17 by madamou          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

int	ft_printf_fd(int fd, const char *format, ...)
{
	va_list	args;
	int		i;
	int		count;

	if (!format)
		return (-1);
	i = 0;
	count = 0;
	va_start(args, format);
	while (format[i])
	{
		if (format[i] == '%')
		{
			count += monitoring(args, (char *)&format[i], fd);
			format += 2;
		}
		else
		{
			write(fd, &format[i], 1);
			i++;
		}
	}
	va_end(args);
	return (i + count);
}
/*
#include <stdio.h>
#include <limits.h>
int main(void)
{
	printf("ft : \n");
	printf("%d\n", ft_printf(NULL));
	printf("original : \n");
	printf("%d\n", printf(NULL));
}
*/
