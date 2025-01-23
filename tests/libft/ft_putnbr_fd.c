/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 12:28:59 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/28 11:06:01 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd, int *count)
{
	unsigned int	nb;

	if (n == -2147483648)
	{
		ft_putstr_fd("-2147483648", fd);
		*count += 11;
		return ;
	}
	if (n < 0)
	{
		*count += ft_putchar_fd('-', fd);
		nb = -n;
	}
	else
		nb = n;
	if (nb >= 10)
	{
		(*count)++;
		ft_putnbr_fd(nb / 10, fd, count);
		ft_putchar_fd(nb % 10 + '0', fd);
	}
	else
		*count += ft_putchar_fd(nb + '0', fd);
}

void	ft_putnbr_fd_unsigned(unsigned int n, int fd, int *count)
{
	char	c;

	if (n >= 10)
		ft_putnbr_fd(n / 10, fd, count);
	c = (n % 10) + '0';
	write(fd, &c, 1);
	(*count)++;
}

void	ft_putnbr_base_long(unsigned long n, const char *base, int *count)
{
	if (n >= 16)
	{
		ft_putnbr_base_long(n / 16, base, count);
	}
	write(1, &base[n % 16], 1);
	(*count)++;
}

void	ft_printpointer(unsigned long long n, const char *base, int *count)
{
	if (n == 0)
	{
		*count += ft_putstr_fd("(nil)", 1);
		return ;
	}
	*count += ft_putstr_fd("0x", 1);
	if (n >= 16)
	{
		ft_putnbr_base_long(n / 16, base, count);
	}
	write(1, &base[n % 16], 1);
	(*count)++;
}
