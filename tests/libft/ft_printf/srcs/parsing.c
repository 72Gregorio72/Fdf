/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/22 13:45:25 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/07 15:06:00 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	parse_str(char *str, va_list args, int *i, int *count)
{
	*i += 1;
	if (str[*i] == 'c')
		*count += ft_putchar_fd((char)va_arg(args, int), 1);
	else if (str[*i] == 'd' || str[*i] == 'i')
		ft_putnbr_fd(va_arg(args, int), 1, count);
	else if (str[*i] == 's')
		*count += ft_putstr_fd(va_arg(args, char *), 1);
	else if (str[*i] == 'p')
	{
		ft_printpointer(va_arg(args, unsigned long long),
			"0123456789abcdef", count);
	}
	else if (str[*i] == 'u')
		ft_putnbr_fd_unsigned((unsigned int)va_arg(args, int), 1, count);
	else if (str[*i] == 'x')
		ft_putnbr_base_long(va_arg(args, unsigned int),
			"0123456789abcdef", count);
	else if (str[*i] == 'X')
		ft_putnbr_base_long(va_arg(args, unsigned int),
			"0123456789ABCDEF", count);
	else if (str[*i] == '%')
		*count += ft_putchar_fd('%', 1);
}
