/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prova.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:36:16 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/21 16:15:31 by gpicchio         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <mlx.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <stdio.h>
#include <X11/X.h>
#include <X11/keysym.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "fdf.h"
#define SCREENX 1920
#define SCREENY 1080

typedef struct	s_data {
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	void	*mlx;
	void	*win;
}				t_data;

typedef struct s_object
{
    t_data img;
    int color;
    char **map;
    int **new_map;
    int angle_x;
    int angle_y;
	int	size;
}		t_object;


void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    if (x < 0 || x >= SCREENX || y < 0 || y >= SCREENY) {
        printf("Pixel fuori dai limiti: x=%d, y=%d\n", x, y);
        return;
    }
    char *dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}

void	draw_circle(t_data *img, int x, int y, int radius, int color)
{
	static const double PI = 3.1415926535;
	double i, angle, x1, y1;

	for(i = 0; i < 360; i += 0.1)
	{
		angle = i;
		x1 = radius * cos(angle * PI / 180);
		y1 = radius * sin(angle * PI / 180);
		my_mlx_pixel_put(img, x + x1, y + y1, color);
	}
}

char **read_map(char **av)
{
	int fd = open(av[1], O_RDONLY);
	char *line;
	if (fd < 0)
	{
		perror("Error opening file");
		return (NULL);
	}
	int i = 0;
	int map_size = 100;
	char **map = malloc(sizeof(char *) * map_size);
	if (!map)
		return (perror("Error allocating memory"), close(fd), NULL);
	while ((line = get_next_line(fd)) != NULL)
	{
		if (i >= map_size)
		{
			map_size *= 2;
			char **new_map = realloc(map, sizeof(char *) * map_size);
			if (!new_map)
				return (perror("Error reallocating memory"), close(fd), NULL);
			map = new_map;
		}
		map[i] = malloc(sizeof(char) * (ft_strlen(line) + 1));
		if (!map[i])
			return (perror("Error allocating memory"), close(fd), NULL);
		line[ft_strlen(line) - 1] = '\0';
		map[i++] = line;
	}
	map[i] = NULL;
	close(fd);
	return (map);
}

int	num_count(char *str)
{
	int count = 0;
	int i = 0;
	while (str[i])
	{
		while (str[i] && (str[i] < '0' || str[i] > '9'))
			i++;
		if (str[i] && (str[i] >= '0' && str[i] <= '9'))
		{
			count++;
			while (str[i] && (str[i] >= '0' && str[i] <= '9'))
				i++;
		}
	}
	return count;
}

int **transform_map(char **map)
{
    if (!map)
        return NULL;
    int rows = 0;
    while (map[rows])
        rows++;
    if (rows == 0)
        return NULL;
    int **new_map = (int **)malloc(sizeof(int *) * (rows + 1)); // +1 for NULL termination
    if (!new_map)
    {
        perror("Error allocating memory for new_map");
        return NULL;
    }
    for (int i = 0; i < rows; i++)
    {
        int count = num_count(map[i]);
        if (count == 0)
        {
            new_map[i] = (int *)malloc(sizeof(int));
            if (!new_map[i])
            {
                perror("Error allocating memory for new_map row");
                for (int k = 0; k < i; k++)
                    free(new_map[k]);
                free(new_map);
                return NULL;
            }
            new_map[i][0] = 0;
            continue;
        }
        new_map[i] = (int *)malloc(sizeof(int) * count);
        if (!new_map[i])
        {
            perror("Error allocating memory for new_map row");
            for (int k = 0; k < i; k++)
                free(new_map[k]);
            free(new_map);
            return NULL;
        }
        int j = 0;
        int start = 0;
        int in_number = 0;
        while (map[i][start] != '\0' && j < count)
        {
            while (map[i][start] && (map[i][start] != '-' && (map[i][start] < '0' || map[i][start] > '9')))
                start++;
            if (map[i][start] == '\0')
                break;
            int num_start = start;
            if (map[i][start] == '-')
                start++;
            while (map[i][start] && (map[i][start] >= '0' && map[i][start] <= '9'))
                start++;
            int length = start - num_start;
            char *num_str = (char *)malloc(sizeof(char) * (length + 1));
            if (!num_str)
            {
                perror("Error allocating memory for num_str");
                for (int m = 0; m <= i; m++)
                    free(new_map[m]);
                free(new_map);
                return NULL;
            }
            strncpy(num_str, &map[i][num_start], length);
            num_str[length] = '\0';
            new_map[i][j++] = ft_atoi(num_str);
            free(num_str);
        }
    }
    new_map[rows] = NULL;
    return new_map;
}

void isometric_projection(int x, int y, int z, int *x_out, int *y_out, int x_fraction, int y_fraction)
{
	double angle_x = x_fraction * (M_PI / 180);
	double angle_y = y_fraction * (M_PI / 180);
	double rotated_x = x * cos(angle_y) + z * sin(angle_y);
	double rotated_z = -x * sin(angle_y) + z * cos(angle_y);
	double rotated_y = 0;
	
	rotated_z = y * sin(angle_x) + rotated_z * cos(angle_x);
	*x_out = (rotated_x - rotated_y) * cos(M_PI / 180);
	*y_out = (rotated_x + rotated_y) * sin(M_PI / 180) - rotated_z;
}

void draw_line(t_data *img, int x1, int y1, int x2, int y2, int color)
{
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);
	int sx = (x1 < x2) ? 1 : -1;
	int sy = (y1 < y2) ? 1 : -1;
	int err = dx - dy;

	while (1) {
		if (x1 >= 0 && x1 < SCREENX && y1 >= 0 && y1 < SCREENY)
		{
			my_mlx_pixel_put(img, x1, y1, color);
		}
		if (x1 == x2 && y1 == y2)
			break;
		int e2 = err * 2;
		if (e2 > -dy)
		{
			err -= dy;
			x1 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y1 += sy;
		}
	}
}

void draw_map(t_data *img, int x_offset, int y_offset, int size, int color, int **new_map, char **map, int x_angle, int y_angle)
{
	if (!new_map || !map)
	{
		printf("Error: Map is NULL\n");
		return;
	}
	int square_size = size;
	int rows = 0;
	while (new_map[rows]) rows++;
	int cols = num_count(map[0]);
	int map_width = cols * square_size;
	int map_height = rows * square_size;
	int center_x = map_width / 2;
	int center_y = map_height / 2;

	int i = 0, j = 0;
	while (new_map[i]) {
		j = 0;
		while (j < num_count(map[i])) {
			int x = j * square_size;
			int y = i * square_size;
			int z = new_map[i][j] * square_size / 5;
			x -= center_x;
			y -= center_y;
			int iso_x, iso_y;
			isometric_projection(x, y, z, &iso_x, &iso_y, x_angle, y_angle);
			iso_x += center_x + x_offset;
			iso_y += center_y + y_offset;

			// Calcola il colore in base all'altezza z
			int draw_color = 0x00FFFFFF + new_map[i][j];
			my_mlx_pixel_put(img, iso_x, iso_y, draw_color);
			if (j + 1 < num_count(map[i])) {
				int next_x = (j + 1) * square_size - center_x;
				int next_y = i * square_size - center_y;
				int next_z = new_map[i][j + 1] * square_size / 5;

				int iso_next_x, iso_next_y;
				isometric_projection(next_x, next_y, next_z, &iso_next_x, &iso_next_y, x_angle, y_angle);
				iso_next_x += center_x + x_offset;
				iso_next_y += center_y + y_offset;
				draw_line(img, iso_x, iso_y, iso_next_x, iso_next_y, draw_color);
			}
			if (new_map[i + 1]) {
				int below_x = j * square_size - center_x;
				int below_y = (i + 1) * square_size - center_y;
				int below_z = new_map[i + 1][j] * square_size / 5;

				int iso_below_x, iso_below_y;
				isometric_projection(below_x, below_y, below_z, &iso_below_x, &iso_below_y, x_angle, y_angle);
				iso_below_x += center_x + x_offset;
				iso_below_y += center_y + y_offset;
				draw_line(img, iso_x, iso_y, iso_below_x, iso_below_y, draw_color);
			}
			j++;
		}
		i++;
	}
}

int read_input(int keycode, t_object *obj)
{
	if (keycode == XK_Escape)
		exit(0);
	else if (keycode == XK_w)
		obj->angle_x -= 1;
	else if (keycode == XK_s)
		obj->angle_x += 1;
	else if (keycode == XK_a)
		obj->angle_y -= 1;
	else if (keycode == XK_d)
		obj->angle_y += 1;

	mlx_destroy_image(obj->img.mlx, obj->img.img);
	obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
	obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
	draw_map(&obj->img, 500, 500, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y);
	mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
	return 0;
}

int	calculate_size(char **map)
{
	int rows = 0;
	while (map[rows])
		rows++;
	int cols = num_count(map[0]);
	int size = (SCREENX / cols < SCREENY / rows) ? SCREENX / cols : SCREENY / rows;
	return size / 3;
}

int main(int ac, char **av)
{
	t_object obj = {0};
	obj.color = 0x00FFFFFF;
	obj.angle_x = 0;
	obj.angle_y = 0;

	obj.map = read_map(av);
	if (!obj.map)
		return 0;
	obj.size = calculate_size(obj.map);
	ft_printf("Size: %d\n", obj.size);
	obj.new_map = transform_map(obj.map);
	if (!obj.new_map)
		return 0;
	obj.img.mlx = mlx_init();
	obj.img.win = mlx_new_window(obj.img.mlx, SCREENX, SCREENY, "Hello world!");
	obj.img.img = mlx_new_image(obj.img.mlx, SCREENX, SCREENY);
	obj.img.addr = mlx_get_data_addr(obj.img.img, &obj.img.bits_per_pixel, &obj.img.line_length, &obj.img.endian);

	draw_map(&obj.img, 500, 500, obj.size, obj.color, obj.new_map, obj.map, obj.angle_x, obj.angle_y);
	mlx_put_image_to_window(obj.img.mlx, obj.img.win, obj.img.img, 0, 0);
	mlx_hook(obj.img.win, KeyPress, KeyPressMask, read_input, &obj);
	mlx_loop(obj.img.mlx);
}

