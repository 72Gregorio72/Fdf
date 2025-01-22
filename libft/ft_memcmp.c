/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memcmp.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:16:42 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 12:50:56 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	ft_memcmp(const void *s1, const void *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (((unsigned char *)s1)[i] != ((unsigned char *)s2)[i])
			return (((unsigned char *)s1)[i] - ((unsigned char *)s2)[i]);
		i++;
	}
	return (0);
}
/* 
int main()
{
	char str1[] = "Helsdlo World!";
	char str2[] = "Heldslo World!";
	int n = 5;
	int ret;
	
	ret = ft_memcmp(str1, str2, n);
	printf("Return value is: %d\n", ret);
	ret = memcmp(str1, str2, 10);
	printf("Return value is: %d\n", ret);
	return 0;
} */