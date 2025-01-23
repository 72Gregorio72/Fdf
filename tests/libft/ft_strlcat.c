/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 15:33:17 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 12:42:57 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <bsd/string.h>

static int	lungo(char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

static int	lungo1(const char *str)
{
	int	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

size_t	ft_strlcat(char *dest, const char *src, unsigned int size)
{
	unsigned int	i;
	unsigned int	j;
	unsigned int	res_d;
	unsigned int	res_s;

	i = lungo(dest);
	j = 0;
	res_d = lungo(dest);
	res_s = lungo1(src);
	if (size < 1)
		return (res_s + size);
	while (src[j] && i < size - 1)
	{
		dest[i] = src[j];
		i++;
		j++;
	}
	dest[i] = '\0';
	if (size < res_d)
		return (res_s + size);
	else
		return (res_d + res_s);
}
/* 
#include <stdio.h>

int main(void)
{
	char dest[5] = "Hello";
	char src[10] = "World";
	printf("%lu\n", ft_strlcat(dest, src, 2));
	
	char dest1[5] = "Hello";
	char src1[10] = "World";
	printf("%lu\n", strlcat(dest1, src1, 2));
    return 0;
} */