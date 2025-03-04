/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fsfs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:36:16 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/23 11:31:33 by gpicchio         ###   ########.fr       */
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

typedef struct s_mouse_data {
	int mouse_pressed;
	int	right_mouse_pressed;
	int	initial_mouse_pos[2];
	int	axis_selected;
	double mouse_speed;
}			t_mouse_data;

typedef struct s_map_data {
	char **map;
	int size_x;
	int size_y;
    int **new_map;
}			t_map_data;

typedef struct s_transform {
	int angle_x;
    int angle_y;
	int angle_z;
	int	x_pos;
	int	y_pos;
	int last_x;
	int last_y;
}			t_transform;

typedef struct s_object
{
    t_data img;
    int color;
    t_map_data map_data;
    t_transform transform;
	t_mouse_data mouse_data;
	
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
    int **new_map = (int **)malloc(sizeof(int *) * (rows + 1));
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

void rotate_3d(int x, int y, int z, double *rx, double *ry, double *rz, double angle_x, double angle_y, double angle_z)
{
    double rad_x = angle_x * M_PI / 180.0;
    double rad_y = angle_y * M_PI / 180.0;
    double rad_z = angle_z * M_PI / 180.0;
    double cos_x = cos(rad_x);
    double sin_x = sin(rad_x);
    double y1 = y * cos_x - z * sin_x;
    double z1 = y * sin_x + z * cos_x;
    double cos_y = cos(rad_y);
    double sin_y = sin(rad_y);
    double x2 = x * cos_y + z1 * sin_y;
    double z2 = -x * sin_y + z1 * cos_y;
    double cos_z = cos(rad_z);
    double sin_z = sin(rad_z);
    double x3 = x2 * cos_z - y1 * sin_z;
    double y3 = x2 * sin_z + y1 * cos_z;
    *rx = x3;
    *ry = y3;
    *rz = z2;
}


void isometric_projection(int x, int y, int z, int *x_out, int *y_out, double angle_x, double angle_y, double angle_z)
{
    double rx, ry, rz;
    rotate_3d(x, y, z, &rx, &ry, &rz, angle_x, angle_y, angle_z);
    double iso_x = (rx - ry) * cos(M_PI / 6);
    double iso_y = (rx + ry) * sin(M_PI / 6) - rz;
    double scale = 1.0;
    iso_x *= scale;
    iso_y *= scale;
    *x_out = (int)iso_x;
    *y_out = (int)iso_y;
}


int interpolate_color(int color_start, int color_end, double t) {
	int r_start = (color_start >> 16) & 0xFF;
	int g_start = (color_start >> 8) & 0xFF;
	int b_start = color_start & 0xFF;

	int r_end = (color_end >> 16) & 0xFF;
	int g_end = (color_end >> 8) & 0xFF;
	int b_end = color_end & 0xFF;

	int r = r_start + t * (r_end - r_start);
	int g = g_start + t * (g_end - g_start);
	int b = b_start + t * (b_end - b_start);

	return (r << 16) | (g << 8) | b;
}

int calculate_color(double z, double z_min, double z_max) {
	double t = (z - z_min) / (z_max - z_min);
	t = t < 0 ? 0 : (t > 1 ? 1 : t);

	int color_start = 0x0000FF;
	int color_end = 0xFF0000;

	return interpolate_color(color_start, color_end, t);
}

void draw_line(t_data *img, int x1, int y1, int x2, int y2, int color1, int color2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    double distance = sqrt(dx * dx + dy * dy);
    double progress = 0;

    while (1) {
        if (x1 >= 0 && x1 < SCREENX && y1 >= 0 && y1 < SCREENY) {
            int color = interpolate_color(color1, color2, progress / distance);
            my_mlx_pixel_put(img, x1, y1, color);
        }
        if (x1 == x2 && y1 == y2)
            break;
        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
        progress++;
    }
}

void check_map_null(int **new_map, char **map)
{
    if (!new_map || !map)
    {
        printf("Error: Map is NULL\n");
        return;
    }
}

void calculate_min_max_z(int **new_map, char **map, double *z_min, double *z_max)
{
    int i, j;

    *z_min = 0;
    *z_max = 0;
	i = 0;
    while (new_map[i])
    {
		j = 0;
		while (j < num_count(map[i]))
		{
			if (new_map[i][j] < *z_min)
			*z_min = new_map[i][j];
			if (new_map[i][j] > *z_max)
			*z_max = new_map[i][j];
			j++;
		}
		i++;
    }
}

void draw_horizontal_lines(t_data *img, int i, int j, int square_size, int **new_map, char **map, int center_x, int center_y, int x_offset, int y_offset, int x_angle, int y_angle, int z_angle, double z_min, double z_max)
{
    int x, y, iso_x, iso_y, iso_next_x, iso_next_y, next_color;
    double z, next_z;

    x = j * square_size - center_x;
    y = i * square_size - center_y;
    z = new_map[i][j] * square_size / 5.0;
    isometric_projection(x, y, z, &iso_x, &iso_y, x_angle, y_angle, z_angle);
    iso_x += center_x + x_offset;
    iso_y += center_y + y_offset;
    if (j + 1 < num_count(map[i]))
    {
        next_z = new_map[i][j + 1] * square_size / 5.0;
        isometric_projection((j + 1) * square_size - center_x, y, next_z, &iso_next_x, &iso_next_y, x_angle, y_angle, z_angle);
        iso_next_x += center_x + x_offset;
        iso_next_y += center_y + y_offset;

        next_color = calculate_color(next_z, z_min, z_max);
        draw_line(img, iso_x, iso_y, iso_next_x, iso_next_y, calculate_color(z, z_min, z_max), next_color);
    }
}

void draw_vertical_lines(t_data *img, int i, int j, int square_size, int **new_map, char **map, int center_x, int center_y, int x_offset, int y_offset, int x_angle, int y_angle, int z_angle, double z_min, double z_max)
{
    int x, y, iso_x, iso_y, iso_below_x, iso_below_y, below_color;
    double z, below_z;

    x = j * square_size - center_x;
    y = i * square_size - center_y;
    z = new_map[i][j] * square_size / 5.0;
    isometric_projection(x, y, z, &iso_x, &iso_y, x_angle, y_angle, z_angle);
    iso_x += center_x + x_offset;
    iso_y += center_y + y_offset;
    if (new_map[i + 1])
    {
        below_z = new_map[i + 1][j] * square_size / 5.0;
        isometric_projection(x, (i + 1) * square_size - center_y,
                             below_z, &iso_below_x, &iso_below_y,
                             x_angle, y_angle, z_angle);
        iso_below_x += center_x + x_offset;
        iso_below_y += center_y + y_offset;

        below_color = calculate_color(below_z, z_min, z_max);
        draw_line(img, iso_x, iso_y, iso_below_x, iso_below_y,
                  calculate_color(z, z_min, z_max), below_color);
    }
}

void draw_map(t_data *img, int x_offset, int y_offset, int size, int color, int **new_map, char **map, int x_angle, int y_angle, int z_angle)
{
	int square_size, rows, cols, center_x, center_y;
	double z_min, z_max;
	int i, j;

	check_map_null(new_map, map);
	square_size = size;
	rows = 0;
	while (new_map[rows])
		rows++;
	cols = num_count(map[0]);
	center_x = (cols * square_size) / 2;
	center_y = (rows * square_size) / 2;
	calculate_min_max_z(new_map, map, &z_min, &z_max);
	i = 0;
	while (new_map[i])
	{
		j = 0;
		while (j < num_count(map[i]))
		{
			draw_horizontal_lines(img, i, j, square_size, new_map, map, center_x, center_y, x_offset, y_offset, x_angle, y_angle, z_angle, z_min, z_max);
			draw_vertical_lines(img, i, j, square_size, new_map, map, center_x, center_y, x_offset, y_offset, x_angle, y_angle, z_angle, z_min, z_max);
			j++;
		}
		i++;
	}
}
int	calculate_size(char **map, t_object *obj)
{
	int rows = 0;
	while (map[rows])
		rows++;
	int cols = num_count(map[0]);
	int size = (SCREENX / cols < SCREENY / rows) ? SCREENX / cols : SCREENY / rows;
	obj->map_size[0] = rows;
	obj->map_size[1] = cols;
	return size / 3;
}

int read_input(int keycode, t_object *obj)
{
    if (keycode == XK_Escape)
        exit(0);
    else if (keycode == XK_w)
        obj->angle_x += 5;
    else if (keycode == XK_s)
        obj->angle_x -= 5;
    else if (keycode == XK_a)
        obj->angle_z -= 5;
    else if (keycode == XK_d)
        obj->angle_z += 5;
    else if (keycode == XK_q)
        obj->angle_y -= 5;
    else if (keycode == XK_e)
        obj->angle_y += 5;
	else if (keycode == XK_f)
	{
		obj->mouse_data.axis_selected++;
		if (obj->mouse_data.axis_selected == 3)
			obj->mouse_data.axis_selected = 0;
	}

    mlx_destroy_image(obj->img.mlx, obj->img.img);
    obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
    obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
    draw_map(&obj->img, obj->x_pos, obj->y_pos, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y, obj->angle_z);
    mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
    return 0;
}

void	move_graph(t_object *obj, int x, int y)
{
	int center_x = (obj->map_size[1] * obj->size) / 2;
	int center_y = (obj->map_size[0] * obj->size) / 2;
	int target_x = x - center_x;
	int target_y = y - center_y;

	double speed = 0.1;
	while (obj->x_pos != target_x || obj->y_pos != target_y)
	{
		obj->x_pos += (int)((target_x - obj->x_pos) * speed);
		obj->y_pos += (int)((target_y - obj->y_pos) * speed);
		mlx_destroy_image(obj->img.mlx, obj->img.img);
		obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
		obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
		draw_map(&obj->img, obj->x_pos, obj->y_pos, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y, obj->angle_z);
		mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
		if (abs(obj->x_pos - target_x) < 15 && abs(obj->y_pos - target_y) < 15)
			speed = 1;
		else
			speed = 0.3;
	}
}

int	read_mouse(int button, int x, int y, t_object *obj)
{
	if (button == 4)
	{
		obj->size += (obj->size / 3) + 1;
	}
	else if (button == 5)
	{
		if (obj->size > 2)
			obj->size -= (obj->size / 3) - 1;
	}
	else if (button == 1)
	{
		obj->mouse_data.mouse_pressed = 1;
		move_graph(obj, x, y);
	}
	else if (button == 2)
	{
		obj->angle_x = 0;
		obj->angle_y = 0;
		obj->angle_z = 0;
		obj->x_pos = SCREENX / 2;
		obj->y_pos = SCREENY / 2;
	}
	else if (button == 3)
	{
		obj->mouse_data.right_mouse_pressed = 1;
	}
	mlx_destroy_image(obj->img.mlx, obj->img.img);
	obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
	obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
	draw_map(&obj->img, obj->x_pos, obj->y_pos, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y, obj->angle_z);
	mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
	return 0;
}

int release_mouse(int button, int x, int y, t_object *obj)
{
	if (button == 1)
		obj->mouse_data.mouse_pressed = 0;
	if (button == 3)
	{
		obj->mouse_data.right_mouse_pressed = 0;
		obj->mouse_data.initial_mouse_pos[0] = 0;
		obj->mouse_data.initial_mouse_pos[1] = 0;
	}
	return 0;
}

int handle_mouse_move(int x, int y, t_object *obj)
{
	if (obj->mouse_data.mouse_pressed)
	{
		int center_x = (obj->map_size[1] * obj->size) * 0.5;
		int center_y = (obj->map_size[0] * obj->size) * 0.5;
		int target_x = x - center_x;
		int target_y = y - center_y;

		double speed = 0.5;
		obj->x_pos += (int)((target_x - obj->x_pos) * speed);
		obj->y_pos += (int)((target_y - obj->y_pos) * speed);
		mlx_destroy_image(obj->img.mlx, obj->img.img);
		obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
		obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
		draw_map(&obj->img, obj->x_pos, obj->y_pos, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y, obj->angle_z);
		mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
	}
	if (obj->mouse_data.right_mouse_pressed)
	{
		int delta_x = x - obj->mouse_data.initial_mouse_pos[0];
		int delta_y = y - obj->mouse_data.initial_mouse_pos[1];
        if (obj->mouse_data.axis_selected == 0)
        {
			if (delta_x < 0)
            	obj->angle_z += obj->mouse_data.mouse_speed;
			else
				obj->angle_z -= obj->mouse_data.mouse_speed;
        }
        else if (obj->mouse_data.axis_selected == 1)
        {
			if (delta_y < 0)
				obj->angle_x += obj->mouse_data.mouse_speed;
			else
				obj->angle_x -= obj->mouse_data.mouse_speed;
        }
		else if (obj->mouse_data.axis_selected == 2)
		{
			if (delta_x < 0)
            	obj->angle_z += obj->mouse_data.mouse_speed * 0.5;
			else
				obj->angle_z -= obj->mouse_data.mouse_speed * 0.5;
			if (delta_y < 0)
				obj->angle_x += obj->mouse_data.mouse_speed * 0.5;
			else
				obj->angle_x -= obj->mouse_data.mouse_speed * 0.5;
		}
		mlx_destroy_image(obj->img.mlx, obj->img.img);
		obj->img.img = mlx_new_image(obj->img.mlx, SCREENX, SCREENY);
		obj->img.addr = mlx_get_data_addr(obj->img.img, &obj->img.bits_per_pixel, &obj->img.line_length, &obj->img.endian);
		draw_map(&obj->img, obj->x_pos, obj->y_pos, obj->size, obj->color, obj->new_map, obj->map, obj->angle_x, obj->angle_y, obj->angle_z);
		mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
		obj->mouse_data.initial_mouse_pos[0] = x;
		obj->mouse_data.initial_mouse_pos[1] = y;
	}
	return 0;
}

int main(int ac, char **av)
{
	t_object obj = {0};
	obj.color = 0x00FFFFFF;
	obj.angle_x = 0;
	obj.angle_y = 0;
	obj.angle_z = 0;
	obj.x_pos = SCREENX / 2;
	obj.y_pos = SCREENY / 2;
	obj.mouse_data.mouse_pressed = 0;
	obj.mouse_data.right_mouse_pressed = 0;
	obj.last_x = SCREENX / 2;
	obj.last_y = SCREENY / 2;
	obj.mouse_data.axis_selected = 0;
	obj.mouse_data.initial_mouse_pos[0] = 0;
	obj.mouse_data.initial_mouse_pos[1] = 0;
	obj.mouse_data.mouse_speed = 5;

	obj.map = read_map(av);
	if (!obj.map)
		return 0;
	obj.size = calculate_size(obj.map, &obj);
	obj.new_map = transform_map(obj.map);
	if (!obj.new_map)
		return 0;
	obj.img.mlx = mlx_init();
	obj.img.win = mlx_new_window(obj.img.mlx, SCREENX, SCREENY, "Hello world!");
	obj.img.img = mlx_new_image(obj.img.mlx, SCREENX, SCREENY);
	obj.img.addr = mlx_get_data_addr(obj.img.img, &obj.img.bits_per_pixel, &obj.img.line_length, &obj.img.endian);

	draw_map(&obj.img, obj.x_pos, obj.y_pos, obj.size, obj.color, obj.new_map, obj.map, obj.angle_x, obj.angle_y, obj.angle_z);
	mlx_put_image_to_window(obj.img.mlx, obj.img.win, obj.img.img, 0, 0);
	mlx_hook(obj.img.win, KeyPress, KeyPressMask, read_input, &obj);
	mlx_hook(obj.img.win, ButtonPress, ButtonPressMask, read_mouse, &obj);
    mlx_hook(obj.img.win, ButtonRelease, ButtonReleaseMask, release_mouse, &obj);
    mlx_hook(obj.img.win, MotionNotify, PointerMotionMask, handle_mouse_move, &obj);
	mlx_loop(obj.img.mlx);
}

