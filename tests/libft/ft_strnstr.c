/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:23:41 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 12:41:52 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *big, const char *little, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (!*little)
		return ((char *)big);
	while (big[i] && i < len)
	{
		j = 0;
		while (big[i + j] == little[j] && little[j] && i + j < len)
			j++;
		if (!little[j])
			return ((char *)big + i);
		i++;
	}
	return (NULL);
}
/* 
int main()
{
	char str[] = "Hello World!";
	char *p;
	p = ft_strnstr(str, "Wor", 12);
	printf("String after 'Wor' is: %s\n", p);
	char str1[] = "Hello World!";
	char *p1;
	p1 = strnstr(str1, "Wor", 12);
	printf("String after 'Wor' is: %s\n", p1);
	return 0;
} */