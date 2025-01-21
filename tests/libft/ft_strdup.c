/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 16:57:43 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/18 17:03:13 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	*ft_strcpy(char *dest, const char *src)
{
	int		i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

char	*ft_strdup(const char *src)
{
	char	*temp;
	int		src_len;

	src_len = ft_strlen(src);
	temp = (char *)malloc((src_len + 1));
	if (temp == NULL)
		return (0);
	return (ft_strcpy(temp, src));
}

/* int main() {
	char str[] = "Hello World!";
	char *p;
	p = ft_strdup(str);
	printf("String after 'Wor' is: %s\n", p);
	char *p1;
	p1 = strdup(str);
	printf("String after 'Wor' is: %s\n", p1);
	return 0;
} */