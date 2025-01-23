/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 12:20:47 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 16:25:24 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libft.h"

void	*ft_memmove(void *dest, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	if (!dest && !src)
		return (NULL);
	d = (unsigned char *)dest;
	s = (unsigned char *)src;
	if (d < s)
	{
		ft_memcpy(dest, src, n);
	}
	else
	{
		while (n--)
		{
			d[n] = s[n];
		}
	}
	return (dest);
}
/* 
 int main()
{
	char *str  = malloc(10);
	char *str2 = malloc(10);

	strcpy(str, "Hello");
	strcpy(str2, "123");
	ft_memmove(str + 2, str, 2);
	ft_memmove(str2 + 2, str2, 2);
	printf("%s\n", str);
	printf("%s\n", str2);
	return (0);
} */