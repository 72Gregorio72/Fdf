/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 17:56:23 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/16 13:46:38 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

#define MAX_FD 4096

char	*clean(char *line)
{
	char	*rest;
	int		i;

	if (*line == '\0')
		return (free(line), NULL);
	i = 0;
	while (line[i] && line[i] != '\n')
		i++;
	if (line[i] == '\n')
		i++;
	rest = ft_strdup(line + i);
	if (!rest)
		return (NULL);
	free(line);
	return (rest);
}

char	*get_first_line(char *line)
{
	ssize_t	i;
	char	*first_line;

	if (!line || *line == '\0')
		return (NULL);
	i = 0;
	while (line[i] && line[i] != '\n')
	{
		i++;
	}
	first_line = (char *)malloc(i + 2);
	if (!first_line)
		return (NULL);
	ft_strlcpy(first_line, line, i + 2);
	return (first_line);
}

char	*append(int fd, char *line)
{
	ssize_t			rd;
	char			*buffer;
	char			*tmp;

	buffer = malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	rd = 1;
	while (rd > 0 && !ft_strchr(line, '\n'))
	{
		rd = read(fd, buffer, BUFFER_SIZE);
		if (rd < 0)
		{
			free(buffer);
			free(line);
			return (NULL);
		}
		buffer[rd] = '\0';
		tmp = ft_strjoin(line, buffer);
		free(line);
		line = tmp;
	}
	free(buffer);
	return (line);
}

char	*get_next_line(int fd)
{
	static char	*lines[MAX_FD];
	char		*ret;

	if (fd == -42)
	{
		fd = 0;
		while (fd < MAX_FD)
		{
			if (lines[fd])
				free(lines[fd]);
			lines[fd] = NULL;
			fd++;
		}
		return (NULL);
	}
	if (fd < 0 || fd >= MAX_FD || BUFFER_SIZE <= 0)
		return (NULL);
	if (!lines[fd])
		lines[fd] = ft_strdup("");
	lines[fd] = append(fd, lines[fd]);
	if (!lines[fd])
		return (NULL);
	ret = get_first_line(lines[fd]);
	lines[fd] = clean(lines[fd]);
	return (ret);
}
