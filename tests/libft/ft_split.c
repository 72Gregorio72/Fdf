/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 10:55:40 by gpicchio          #+#    #+#             */
/*   Updated: 2024/11/20 14:08:34 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_count_words(char const *s, char c)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] != c)
		{
			count++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	return (count);
}

static char	**ft_free(char **splitted, int i)
{
	while (i >= 0)
	{
		free(splitted[i]);
		i--;
	}
	free(splitted);
	return (NULL);
}

static char	*ft_dupto(const char *s, char c)
{
	char	*dup;
	int		i;
	int		len;

	len = 0;
	while (s[len] && s[len] != c)
		len++;
	dup = (char *)malloc(len + 1);
	if (!dup)
		return (NULL);
	i = 0;
	while (s[i] && s[i] != c)
	{
		dup[i] = s[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

char	**ft_split(char const *s, char c)
{
	char	**splitted;
	int		i;

	i = 0;
	if (!s)
		return (NULL);
	splitted = (char **)malloc(sizeof(char *) * (ft_count_words(s, c) + 1));
	if (!splitted)
		return (NULL);
	while (*s)
	{
		while (*s && *s == c)
			s++;
		if (*s)
		{
			splitted[i] = ft_dupto(s, c);
			if (!splitted[i])
				return (ft_free(splitted, i));
			i++;
			while (*s && *s != c)
				s++;
		}
	}
	splitted[i] = NULL;
	return (splitted);
}

/* 
int main()
{
	char **p;
	p = ft_split("Hello!", ' ');
	for (int j = 0; p[j] != NULL; j++)
	{
		printf("%s\n", p[j]);
	}
	for (int j = 0; p[j] != NULL; j++)
	{
		free(p[j]);
	}
	free(p);
	return 0;
} */
