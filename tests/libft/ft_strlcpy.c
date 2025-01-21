/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcpy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 15:12:26 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 12:44:05 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <unistd.h>
#include <bsd/string.h>
#include <stdio.h>
#include <stdlib.h>

size_t	ft_strlcpy(char *dst, const char *src, size_t size)
{
	size_t	i;

	i = 0;
	if (size == 0)
		return (ft_strlen(src));
	while (src[i] && i < size - 1)
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return (ft_strlen(src));
}
/* 
int main()
{
	char *src = "Hello";
	char *dst = malloc(1);
	char *src1 = "Hello";
	char *dst1 = malloc(1);
	ft_strlcpy(dst, src, 5);
	printf("%s\n", dst);
	strlcpy(dst1, src1, 5);
	printf("%s\n", dst1);
	return (0);
} */
