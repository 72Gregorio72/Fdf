/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:04:45 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 12:49:59 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	const unsigned char	*str;
	int					i;

	str = (const unsigned char *)s;
	i = 0;
	while (n--)
	{
		if (str[i] == (unsigned char)c)
			return ((void *)str + i);
		i++;
	}
	return (NULL);
}
/* 
int main()
{
	char str[] = "Hello World!";
	char *p;
	p = ft_memchr(str, 'W', 12);
	printf("String after 'W' is: %s\n", p);
	char str1[] = "Hello World!";
	char *p1;
	p1 = memchr(str1, 'W', 12);
	printf("String after 'W' is: %s\n", p1);
	return 0;
} */