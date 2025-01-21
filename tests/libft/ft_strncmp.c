/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strncmp.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 15:58:49 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/16 12:06:29 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t		i;

	i = 0;
	while (i < n && (s1[i] || s2[i]))
	{
		if (s1[i] != s2[i])
		{
			if ((unsigned char)s1[i] > (unsigned char)s2[i])
				return (1);
			else
				return (-1);
		}
		i++;
	}
	return (0);
}
/* 

#include <stdio.h>

int main(void)
{
	//char str1[] = "test";
	//har str2[] = "teste";
	//int n = 10;
	int ret;
	
	ret = ft_strncmp("", "", 5);
	printf("Return value is: %d\n", ret);
	ret = strncmp("", "", 5);
	printf("Return value is: %d\n", ret);
    return 0;
} */
