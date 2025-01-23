/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 15:55:20 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/19 15:16:48 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	char	*last;

	last = NULL;
	while (*s)
	{
		if (*s == (unsigned char)c)
			last = (char *)s;
		s++;
	}
	if ((unsigned char)c == '\0')
		return ((char *)s);
	return (last);
}
/* int main()
{
	char str[] = "Hello Worold!";
	char *p;
	p = ft_strrchr(str, 'o');
	printf("String after 'o' is: %s\n", p);
	char str1[] = "Hello Worold!";
	char *p1;
	p1 = strrchr(str1, 'o');
	printf("String after 'o' is: %s\n", p1);
	return 0;
} */