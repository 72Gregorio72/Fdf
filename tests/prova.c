/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prova.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/17 10:36:16 by gpicchio          #+#    #+#             */
/*   Updated: 2025/01/23 15:03:28 by gpicchio         ###   ########.fr       */
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
#include <sys/time.h>
#include "fdf.h"
#define SCREENX 1920
#define SCREENY 1080

typedef struct s_proj_point {
    int x;
    int y;
}		t_proj_point;

typedef struct s_button {
    int x;
    int y;
    int width;
    int height;
    int base_color;
    int hover_color;
    int is_hovered;
    int (*funct_ptr)();
    void *param;
	int is_selected;
}           t_button;

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

typedef struct s_int_map {
	int num;
	int color;
	int trash;
}		t_int_map;

typedef struct s_map_data {
	char **map;
	int size_x;
	int size_y;
	t_int_map **new_map;
}			t_map_data;

typedef struct s_transform {
    int angle_x;
    int angle_y;
    int angle_z;
    double rad_x;
    double rad_y;
    double rad_z;
    double cos_x;
    double sin_x;
    double cos_y;
    double sin_y;
    double cos_z;
    double sin_z;
    int x_pos;
    int y_pos;
    int last_x;
    int last_y;
} t_transform;

typedef struct s_object
{
    t_data img;
	t_button x_axis_button;
	t_button y_axis_button;
	t_button z_axis_button;
	t_button xy_axis_button;
	t_button projection_button;
	t_button	crazy_button;
    int color;
	int color_end;
    t_map_data map_data;
    t_transform transform;
	t_mouse_data mouse_data;
	int	size;
	int crazy_mode;
	int projection_mode;
	t_proj_point **proj_points;
}		t_object;

void	draw_all(t_object *obj, t_data *img);
void	draw_map(t_data *img, t_object *obj);
void	draw_buttons(t_object *obj);
int	read_mouse(int button, int x, int y, t_object *obj);
int release_mouse(int button, int x, int y, t_object *obj);
int handle_mouse_move(int x, int y, t_object *obj);


void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
    if (x < 0 || x >= SCREENX || y < 0 || y >= SCREENY) {
        ft_printf("Pixel fuori dai limiti: x=%d, y=%d\n", x, y);
        return;
    }
    char *dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
    *(unsigned int *)dst = color;
}
void	draw_all(t_object *obj, t_data *img)
{
    ft_memset(img->addr, 0, SCREENX * SCREENY * (img->bits_per_pixel / 8));
    draw_map(img, obj);
    mlx_put_image_to_window(obj->img.mlx, obj->img.win, obj->img.img, 0, 0);
    draw_buttons(obj);
}

void	draw_rectangle(t_data *img, int x, int y, int width, int height, int color, int radius)
{
	int i, j;
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if ((i < radius && j < radius && sqrt((radius - i) * (radius - i) + (radius - j) * (radius - j)) > radius) ||
				(i < radius && j >= width - radius && sqrt((radius - i) * (radius - i) + (radius - (width - j - 1)) * (radius - (width - j - 1))) > radius) ||
				(i >= height - radius && j < radius && sqrt((radius - (height - i - 1)) * (radius - (height - i - 1)) + (radius - j) * (radius - j)) > radius) ||
				(i >= height - radius && j >= width - radius && sqrt((radius - (height - i - 1)) * (radius - (height - i - 1)) + (radius - (width - j - 1)) * (radius - (width - j - 1))) > radius))
			{
				continue;
			}
			my_mlx_pixel_put(img, x + j, y + i, color);
		}
	}
}

int    is_mouse_over(t_button *button, int mouse_x, int mouse_y)
{
	return (mouse_x >= button->x && mouse_x <= (button->x + button->width) &&
		 mouse_y >= button->y && mouse_y <= (button->y + button->height));
}

void draw_rounded_button_with_effects(t_object *obj, int x, int y, int width, int height, int base_color, int hover_color, int is_hovered, int is_selected)
{
    if (is_hovered || is_selected)
        draw_rectangle(&obj->img, x, y, width, height, hover_color, 10);
    else
        draw_rectangle(&obj->img, x, y, width, height, base_color, 10);
}

void draw_button(t_object *obj, t_button *button, int x, int y, int width, int height, int base_color, int hover_color)
{
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    button->base_color = base_color;
    button->hover_color = hover_color;
	draw_rounded_button_with_effects(obj, button->x, button->y, button->width, button->height, button->base_color, button->hover_color, button->is_hovered, button->is_selected);
}

void draw_buttons(t_object *obj)
{
	draw_button(obj, &obj->x_axis_button, 10, 10, 100, 50, 0xff2d00, 0x800000);
	int text_x = obj->x_axis_button.x + (obj->x_axis_button.width / 2) - 20;
	int text_y = obj->x_axis_button.y + (obj->x_axis_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "X Axis");

	draw_button(obj, &obj->y_axis_button, 10, 70, 100, 50, 0x47ec30, 0x008000);
	text_x = obj->y_axis_button.x + (obj->y_axis_button.width / 2) - 20;
	text_y = obj->y_axis_button.y + (obj->y_axis_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "Y Axis");

	draw_button(obj, &obj->z_axis_button, 10, 130, 100, 50, 0x009ffa, 0x000080);
	text_x = obj->z_axis_button.x + (obj->z_axis_button.width / 2) - 20;
	text_y = obj->z_axis_button.y + (obj->z_axis_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "Z Axis");

	draw_button(obj, &obj->xy_axis_button, 10, 190, 100, 50, 0xf3ff00, 0x808000);
	text_x = obj->xy_axis_button.x + (obj->xy_axis_button.width / 2) - 20;
	text_y = obj->xy_axis_button.y + (obj->xy_axis_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "XY Axis");

	draw_button(obj, &obj->projection_button, 10, 250, 100, 50, 0x00ffff, 0x008080);
	text_x = obj->projection_button.x + (obj->projection_button.width / 2) - 20;
	text_y = obj->projection_button.y + (obj->projection_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "Parallel");
	mlx_string_put(obj->img.mlx, obj->img.win, text_x - 7, text_y + 10, 0x000000, "Projection");

	draw_button(obj, &obj->crazy_button, 10, 310, 100, 50, 0xff00ff, 0x800080);
	text_x = obj->crazy_button.x + (obj->crazy_button.width / 2) - 20;
	text_y = obj->crazy_button.y + (obj->crazy_button.height / 2);
	mlx_string_put(obj->img.mlx, obj->img.win, text_x, text_y, 0x000000, "Crazy");
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
		int k = 0;
		while (line[k] && (ft_isdigit(line[k]) || line[k] == ' ' || line[k] == '-' || line[k] == '\n' || line[k] == ','))
		{
			ft_printf("line[%d]: %c\n", k, line[k]);
			if (line[k] == ',' && (line[k + 1] == '0' && line[k + 2] == 'x'))
			{
				k += 3;
				while(line[k] && ft_isdigit(line[k]) || (line[k] >= 'a' && line[k] <= 'f') || (line[k] >= 'A' && line[k] <= 'F'))
					k++;
				if (line[k] != ' ' && line[k] != '\0' && line[k] != '\n')
					return (ft_printf("Error: Invalid color in map: %d\n", line[k]), NULL);
			}
			else if (!ft_isdigit(line[k]) && line[k] != ' ' && line[k] != '-' && line[k] != '\n')
				return (ft_printf("Error: Invalid color in map: %c\n", line[k]), NULL);
			k++;
		}
		if (line[k] != '\0')
			return (ft_printf("Error: Invalid character in map: %c\n", line[k]), NULL);
		if (i >= map_size)
		{
			map_size *= 2;
			char **new_map = malloc(sizeof(char *) * map_size);
			if (new_map) {
				for (int j = 0; j < i; j++) {
					new_map[j] = map[j];
				}
				free(map);
			}
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

t_int_map **transform_map(char **map)
{
	if (!map)
		return NULL;
	int rows = 0;
	while (map[rows])
		rows++;
	if (rows == 0)
		return NULL;
	t_int_map **new_map = (t_int_map **)malloc(sizeof(t_int_map *) * (rows + 1));
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
			new_map[i] = (t_int_map *)malloc(sizeof(t_int_map));
			if (!new_map[i])
			{
				perror("Error allocating memory for new_map row");
				for (int k = 0; k < i; k++)
					free(new_map[k]);
				free(new_map);
				return NULL;
			}
			new_map[i][0].num = 0;
			new_map[i][0].color = -1;
			new_map[i][0].trash = 0;
			continue;
		}
		new_map[i] = (t_int_map *)malloc(sizeof(t_int_map) * count);
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
			new_map[i][j].color = -1;
			new_map[i][j].trash = 0;
			if (map[i][start] == ',')
			{
				start++;
				if (map[i][start] == '0' && (map[i][start + 1] == 'x' || map[i][start + 1] == 'X'))
				{
					start += 3;
					int color_start = start;
					while (map[i][start] && ((map[i][start] >= '0' && map[i][start] <= '9') || 
											 (map[i][start] >= 'a' && map[i][start] <= 'f') || 
											 (map[i][start] >= 'A' && map[i][start] <= 'F')))
					{
						start++;
					}
					int color_length = start - color_start;
					char *color_str = (char *)malloc(sizeof(char) * (color_length + 1));
					if (!color_str)
					{
						perror("Error allocating memory for color_str");
						for (int m = 0; m <= i; m++)
							free(new_map[m]);
						free(new_map);
						return NULL;
					}
					strncpy(color_str, &map[i][color_start], color_length);
					color_str[color_length] = '\0';
					new_map[i][j].color = strtol(color_str, NULL, 16);
					free(color_str);
				}
			}
			strncpy(num_str, &map[i][num_start], length);
			num_str[length] = '\0';
			new_map[i][j].num = ft_atoi(num_str);
			new_map[i][j].trash = 1;
			free(num_str);
			j++;
		}
	}
	new_map[rows] = NULL;
	return (new_map);
}

double normalize_angle(double angle)
{
    if (angle < 0)
    {
        angle += 360.0;
    }
    else if (angle >= 360.0)
    {
        angle -= 360.0;
    }
    return angle;
}

#include <math.h>

void calculate_map_bounds(t_map_data *map, int *x_min, int *x_max, int *y_min, int *y_max)
{
    *x_min = INT_MAX;
    *x_max = INT_MIN;
    *y_min = INT_MAX;
    *y_max = INT_MIN;

    for (int y = 0; y < map->size_y; y++)
    {
        for (int x = 0; x < map->size_x; x++)
        {
            if (x < *x_min) *x_min = x;
            if (x > *x_max) *x_max = x;
            if (y < *y_min) *y_min = y;
            if (y > *y_max) *y_max = y;
        }
    }
}

void get_x_bounds(t_map_data *map, int *x_min, int *x_max)
{
    *x_min = INT_MAX;
    *x_max = INT_MIN;

    for (int y = 0; y < map->size_y; y++)
    {
        for (int x = 0; x < map->size_x; x++)
        {
            if (x < *x_min) *x_min = x;
            if (x > *x_max) *x_max = x;
        }
    }
}

void get_y_bounds(t_map_data *map, int *y_min, int *y_max)
{
    *y_min = INT_MAX;
    *y_max = INT_MIN;

    for (int y = 0; y < map->size_y; y++)
    {
        if (y < *y_min) *y_min = y;
        if (y > *y_max) *y_max = y;
    }
}

void update_rotation(t_transform *transform)
{
    transform->rad_x = transform->angle_x * M_PI / 180.0;
    transform->rad_y = transform->angle_y * M_PI / 180.0;
    transform->rad_z = transform->angle_z * M_PI / 180.0;
    transform->cos_x = cos(transform->rad_x);
    transform->sin_x = sin(transform->rad_x);
    transform->cos_y = cos(transform->rad_y);
    transform->sin_y = sin(transform->rad_y);
    transform->cos_z = cos(transform->rad_z);
    transform->sin_z = sin(transform->rad_z);
}

void rotate_3d_optimized(double x, double y, double z, double *rx, double *ry, double *rz, t_transform *transform)
{

    // Rotazione attorno all'asse X

    double y1 = y * transform->cos_x - z * transform->sin_x;

    double z1 = y * transform->sin_x + z * transform->cos_x;


    // Rotazione attorno all'asse Y

    double x2 = x * transform->cos_y + z1 * transform->sin_y;

    double z2 = -x * transform->sin_y + z1 * transform->cos_y;


    // Rotazione attorno all'asse Z

    double x3 = x2 * transform->cos_z - y1 * transform->sin_z;

    double y3 = x2 * transform->sin_z + y1 * transform->cos_z;


    *rx = x3;

    *ry = y3;

    *rz = z2;

}

void rotate_3d(double x, double y, double z, double *rx, double *ry, double *rz, double angle_x, double angle_y, double angle_z)
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



void project(int x, int y, int z, int *x_out, int *y_out, t_object *obj)
{
    double rx, ry, rz;

    rotate_3d_optimized(x, y, z, &rx, &ry, &rz, &obj->transform);

    if (obj->projection_mode == 0)
    {
        double iso_x = (rx - ry);
        double iso_y = ((rx + ry) / 2) - rz;
        *x_out = (int)iso_x;
        *y_out = (int)iso_y;
    }
    else
    {
        *x_out = (int)rx;
        *y_out = (int)ry;
    }
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

int calculate_color(double z, double z_min, double z_max, t_object *obj) {
	double t = (z - z_min) / (z_max - z_min);
	t = t < 0 ? 0 : (t > 1 ? 1 : t);

	int color_start = obj->color;
	int color_end = obj->color;

	return interpolate_color(color_start, color_end, t);
}

void draw_line(t_data *img, int x1, int y1, int x2, int y2, int color1, int color2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int distance = sqrt(dx * dx + dy * dy);  // Potrebbe essere ottimizzato ulteriormente
    if (distance == 0) distance = 1; // Evitare divisioni per zero

    while (1) {
        if (x1 >= 0 && x1 < SCREENX && y1 >= 0 && y1 < SCREENY) {
            // Interpolazione lineare del colore usando interi
            double t = (double)(abs(x1 - x2) + abs(y1 - y2)) / distance;
            int color = interpolate_color(color1, color2, t);
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
    }
}

void check_map_null(t_int_map **new_map, char **map)
{
    if (!new_map || !map)
    {
        ft_printf("Error: Map is NULL\n");
        return;
    }
}

void calculate_min_max_z(t_int_map **new_map, char **map, double *z_min, double *z_max)
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
			if (new_map[i][j].num < *z_min)
			*z_min = new_map[i][j].num;
			if (new_map[i][j].num > *z_max)
			*z_max = new_map[i][j].num;
			j++;
		}
		i++;
    }
}

void draw_horizontal_lines(t_data *img, int i, int j, int square_size, int center_x, int center_y, double z_min, double z_max, t_object *obj)
{
    int x, y, iso_x, iso_y, iso_next_x, iso_next_y, next_color;
    double z, next_z;

    x = j * square_size - center_x;
    y = i * square_size - center_y;
    z = obj->map_data.new_map[i][j].num * square_size / 5.0;
    project(x, y, z, &iso_x, &iso_y, obj);
    iso_x += obj->transform.x_pos;
    iso_y += obj->transform.y_pos;
	if (obj->map_data.new_map[i][j].num != -1 && obj->map_data.new_map[i][j].trash)
    {
        next_z = obj->map_data.new_map[i][j + 1].num * square_size / 5.0;
        project((j + 1) * square_size - center_x, y, next_z, &iso_next_x, &iso_next_y, obj);
        iso_next_x += obj->transform.x_pos;
        iso_next_y += obj->transform.y_pos;

        next_color = calculate_color(next_z, z_min, z_max, obj);
		if (obj->map_data.new_map[i][j + 1].color != -1)
			next_color = obj->map_data.new_map[i][j + 1].color;
        draw_line(img, iso_x, iso_y, iso_next_x, iso_next_y, calculate_color(z, z_min, z_max, obj), next_color);
    }
}

void draw_vertical_lines(t_data *img, int i, int j, int square_size, int center_x, int center_y, double z_min, double z_max, t_object *obj)
{
    int x, y, proj_x, proj_y, proj_below_x, proj_below_y, below_color;
    double z, below_z;

    x = j * square_size - center_x;
    y = i * square_size - center_y;
    z = obj->map_data.new_map[i][j].num * square_size / 5.0;
    project(x, y, z, &proj_x, &proj_y, obj);
    proj_x += obj->transform.x_pos;
    proj_y += obj->transform.y_pos;
    if (obj->map_data.new_map[i + 1] && obj->map_data.new_map[i + 1][j].trash)
    {
        below_z = obj->map_data.new_map[i + 1][j].num * square_size / 5.0;
        project(x, (i + 1) * square_size - center_y, below_z, &proj_below_x, &proj_below_y, obj);
        proj_below_x += obj->transform.x_pos;
        proj_below_y += obj->transform.y_pos;

        below_color = calculate_color(below_z, z_min, z_max, obj);
		if (obj->map_data.new_map[i + 1][j].color != -1)
			below_color = obj->map_data.new_map[i + 1][j].color;
        draw_line(img, proj_x, proj_y, proj_below_x, proj_below_y, calculate_color(z, z_min, z_max, obj), below_color);
    }
}

void draw_map(t_data *img, t_object *obj)
{
    int square_size, rows, cols, center_x, center_y;
    double z_min, z_max;
    int i, j;

    check_map_null(obj->map_data.new_map, obj->map_data.map);
    square_size = obj->size;
    rows = 0;
    while (obj->map_data.new_map[rows])
        rows++;
    cols = 0;
    while (obj->map_data.new_map[2][cols].trash)
        cols++;
    ft_printf("cols: %d\n", cols);
    center_x = (cols * square_size) / 2;
    center_y = (rows * square_size) / 2;

    // Chiamata unica a update_rotation
    update_rotation(&obj->transform);

    calculate_min_max_z(obj->map_data.new_map, obj->map_data.map, &z_min, &z_max);
    i = 0;
    while (obj->map_data.new_map[i])
    {
        j = 0;
        cols = 0;
        while (obj->map_data.new_map[i][cols].trash == 1)
            cols++;
        while (j < cols - 1)
        {
            draw_horizontal_lines(img, i, j, square_size, center_x, center_y, z_min, z_max, obj);
            j++;
        }
        j = 0;
        while (j < cols)
        {
            draw_vertical_lines(img, i, j, square_size, center_x, center_y, z_min, z_max, obj);
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
	obj->map_data.size_x = rows;
	obj->map_data.size_y = cols;
	return size / 3;
}

int read_input(int keycode, t_object *obj)
{
    if (keycode == XK_Escape)
        exit(0);
    else if (keycode == XK_w)
        obj->transform.angle_x += 5;
    else if (keycode == XK_s)
        obj->transform.angle_x -= 5;
    else if (keycode == XK_a)
        obj->transform.angle_z -= 5;
    else if (keycode == XK_d)
        obj->transform.angle_z += 5;
    else if (keycode == XK_q)
        obj->transform.angle_y -= 5;
    else if (keycode == XK_e)
        obj->transform.angle_y += 5;
	else if (keycode == XK_p)
        obj->projection_mode = !obj->projection_mode;

    draw_all(obj, &obj->img);
    return 0;
}

void	move_graph(t_object *obj, int x, int y)
{
	int center_x = (obj->map_data.size_x * obj->size) / 2;
	int center_y = (obj->map_data.size_y * obj->size) / 2;
	int target_x = x - center_x;
	int target_y = y - center_y;

	double speed = 0.1;
	while (obj->transform.x_pos != target_x || obj->transform.y_pos != target_y)
	{
		obj->transform.x_pos += (int)((target_x - obj->transform.x_pos) * speed);
		obj->transform.y_pos += (int)((target_y - obj->transform.y_pos) * speed);
		draw_all(obj, &obj->img);
		if (abs(obj->transform.x_pos - target_x) < 15 && abs(obj->transform.y_pos - target_y) < 15)
			speed = 1;
		else
			speed = 0.3;
	}
}

void get_crazy(t_object *obj)
{
	static double time = 0;
	int i, j;
	double wave_amplitude = 1.1;
	double wave_frequency = 0.5;

	if (obj->crazy_button.is_selected)
	{
		for (i = 0; obj->map_data.new_map[i]; i++) {
			for (j = 0; j < num_count(obj->map_data.map[i]); j++) {
				double distance = sqrt(i * i + j * j);
				obj->map_data.new_map[i][j].num = obj->map_data.new_map[i][j].num - (int)(wave_amplitude * sin(distance * wave_frequency + time));
			}
		}
		time += 0.1;
	}
	draw_all(obj, &obj->img);
}


int	read_mouse(int button, int x, int y, t_object *obj)
{
	if (button == 4)
	{
		obj->size *= 1.2;
		obj->transform.x_pos -= obj->size / 10;
	}
	else if (button == 5 && obj->size > 5)
	{
		obj->size /= 1.2;
		obj->transform.x_pos += obj->size / 10;
	}
	else if (button == 1)
	{
		if (obj->x_axis_button.is_hovered && !obj->x_axis_button.is_selected)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
			obj->z_axis_button.is_selected = 0;
			obj->xy_axis_button.is_selected = 0;
		}
		else if (obj->y_axis_button.is_hovered && !obj->y_axis_button.is_selected)
		{
			obj->mouse_data.axis_selected = 1;
			obj->y_axis_button.is_selected = 1;
			obj->x_axis_button.is_selected = 0;
			obj->z_axis_button.is_selected = 0;
			obj->xy_axis_button.is_selected = 0;
		}
		else if (obj->z_axis_button.is_hovered && !obj->z_axis_button.is_selected)
		{
			obj->mouse_data.axis_selected = 2;
			obj->z_axis_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
			obj->x_axis_button.is_selected = 0;
			obj->xy_axis_button.is_selected = 0;
		}
		else if (obj->xy_axis_button.is_hovered && !obj->xy_axis_button.is_selected)
		{
			obj->mouse_data.axis_selected = 3;
			obj->xy_axis_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
			obj->z_axis_button.is_selected = 0;
			obj->x_axis_button.is_selected = 0;
		}
		else if (obj->projection_button.is_hovered && !obj->projection_button.is_selected)
		{
			obj->mouse_data.axis_selected = 5;
			obj->projection_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
			obj->z_axis_button.is_selected = 0;
			obj->xy_axis_button.is_selected = 0;
			obj->x_axis_button.is_selected = 0;
			obj->projection_mode = !obj->projection_mode;
		}
		else if (obj->crazy_button.is_hovered && !obj->crazy_button.is_selected)
		{
			obj->mouse_data.axis_selected = 4;
			obj->crazy_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
		}
		else if (obj->y_axis_button.is_selected && obj->y_axis_button.is_hovered)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
			obj->y_axis_button.is_selected = 0;
		}
		else if (obj->z_axis_button.is_selected && obj->z_axis_button.is_hovered)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
			obj->z_axis_button.is_selected = 0;
		}
		else if (obj->xy_axis_button.is_selected && obj->xy_axis_button.is_hovered)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
			obj->xy_axis_button.is_selected = 0;
		}
		else if (obj->crazy_button.is_selected && obj->crazy_button.is_hovered)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
		}
		else if (obj->projection_button.is_selected && obj->projection_button.is_hovered)
		{
			obj->mouse_data.axis_selected = 0;
			obj->x_axis_button.is_selected = 1;
			obj->projection_button.is_selected = 0;
		}
		else {
			obj->mouse_data.mouse_pressed = 1;
			move_graph(obj, x, y);
		}
	}
	else if (button == 2)
	{
		obj->transform.angle_x = 0;
		obj->transform.angle_y = 0;
		obj->transform.angle_z = 0;
		obj->transform.x_pos = SCREENX / 2;
		obj->transform.y_pos = SCREENY / 2;
	}
	else if (button == 3)
	{
		obj->mouse_data.right_mouse_pressed = 1;
	}
	draw_all(obj, &obj->img);
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

int	all_buttons_hovered(t_object *obj, int x, int y)
{
	obj->x_axis_button.is_hovered = is_mouse_over(&obj->x_axis_button, x, y);
	obj->y_axis_button.is_hovered = is_mouse_over(&obj->y_axis_button, x, y);
	obj->z_axis_button.is_hovered = is_mouse_over(&obj->z_axis_button, x, y);
	obj->xy_axis_button.is_hovered = is_mouse_over(&obj->xy_axis_button, x, y);
	obj->crazy_button.is_hovered = is_mouse_over(&obj->crazy_button, x, y);
	obj->projection_button.is_hovered = is_mouse_over(&obj->projection_button, x, y);
	return (obj->x_axis_button.is_hovered
		&& obj->y_axis_button.is_hovered
		&& obj->z_axis_button.is_hovered
		&& obj->xy_axis_button.is_hovered
		&& obj->crazy_button.is_hovered
		&& obj->projection_button.is_hovered);
}


int handle_mouse_move(int x, int y, t_object *obj)
{
	static struct timeval last_time = {0, 0};
    struct timeval current_time;
	int framerate;

    gettimeofday(&current_time, NULL);
    long elapsed = (current_time.tv_sec - last_time.tv_sec) * 1000 + (current_time.tv_usec - last_time.tv_usec) / 1000;
	framerate = 1000 / obj->size;
    if (elapsed < framerate)
        return (0);

    last_time = current_time;
	
    if (all_buttons_hovered(obj, x, y))
        draw_all(obj, &obj->img);
	if (!all_buttons_hovered(obj, x, y))
		draw_all(obj, &obj->img);
	
	if (obj->mouse_data.mouse_pressed)
	{
		int center_x = (obj->map_data.size_x * obj->size) * 0.5;
		int center_y = (obj->map_data.size_y * obj->size) * 0.5;
		int target_x = x - center_x;
		int target_y = y - center_y;

		double speed = 0.5;
		obj->transform.x_pos += (int)((target_x - obj->transform.x_pos) * speed);
		obj->transform.y_pos += (int)((target_y - obj->transform.y_pos) * speed);
		draw_all(obj, &obj->img);
	}
	if (obj->mouse_data.right_mouse_pressed)
	{
		int delta_x = x - obj->mouse_data.initial_mouse_pos[0];
		int delta_y = y - obj->mouse_data.initial_mouse_pos[1];
        if (obj->mouse_data.axis_selected == 0)
        {
			if (delta_x < 0)
            	obj->transform.angle_z += obj->mouse_data.mouse_speed;
			else
				obj->transform.angle_z -= obj->mouse_data.mouse_speed;
        }
        else if (obj->mouse_data.axis_selected == 1)
        {
			if (delta_y < 0)
				obj->transform.angle_x += obj->mouse_data.mouse_speed;
			else
				obj->transform.angle_x -= obj->mouse_data.mouse_speed;
        }
		else if (obj->mouse_data.axis_selected == 2)
		{
			if (delta_x < 0)
				obj->transform.angle_y += obj->mouse_data.mouse_speed;
			else
				obj->transform.angle_y -= obj->mouse_data.mouse_speed;
		}
		else if (obj->mouse_data.axis_selected == 3)
		{
			if (delta_x < 0 && abs(delta_x) > abs(delta_y))
				obj->transform.angle_z += obj->mouse_data.mouse_speed;
			else if (abs(delta_x) > abs(delta_y))
				obj->transform.angle_z -= obj->mouse_data.mouse_speed;

			if (delta_y < 0 && abs(delta_y) > abs(delta_x))
				obj->transform.angle_x += obj->mouse_data.mouse_speed;
			else if (abs(delta_y) > abs(delta_x))
				obj->transform.angle_x -= obj->mouse_data.mouse_speed;
		}
		draw_all(obj, &obj->img);
		obj->mouse_data.initial_mouse_pos[0] = x;
		obj->mouse_data.initial_mouse_pos[1] = y;
	}
	else if (obj->mouse_data.axis_selected == 4)
	{
		obj->crazy_mode = 1;
	}
	return (0);
}

int update(t_object *obj)
{
    if (obj->crazy_mode)
        get_crazy(obj);
    return 0;
}

int main(int ac, char **av)
{
	t_object obj = {0};
	obj.color = 0xFF0000;
	obj.color_end = 0x0000FF;
	obj.transform.angle_x = 0;
	obj.transform.angle_y = 0;
	obj.transform.angle_z = 0;
	obj.transform.x_pos = SCREENX / 2;
	obj.transform.y_pos = SCREENY / 2;
	obj.mouse_data.mouse_pressed = 0;
	obj.mouse_data.right_mouse_pressed = 0;
	obj.transform.last_x = SCREENX / 2;
	obj.transform.last_y = SCREENY / 2;
	obj.mouse_data.axis_selected = 0;
	obj.mouse_data.initial_mouse_pos[0] = 0;
	obj.mouse_data.initial_mouse_pos[1] = 0;
	obj.mouse_data.mouse_speed = 5;
	obj.projection_mode = 0;

	obj.map_data.map = read_map(av);
	if (!obj.map_data.map)
		return 0;
	
	obj.size = calculate_size(obj.map_data.map, &obj);
	ft_printf("size: %d\n", obj.size);
	obj.map_data.new_map = transform_map(obj.map_data.map);
	if (!obj.map_data.new_map)
		return 0;
	/* for (int i = 0; obj.map_data.map[i]; i++) {
		for (int j = 0; j < num_count(obj.map_data.map[i]); j++) {
			printf("%d ", obj.map_data.new_map[i][j].trash);
		}
		printf("\n");
	} */
	obj.img.mlx = mlx_init();
	obj.img.win = mlx_new_window(obj.img.mlx, SCREENX, SCREENY, "The best Fdf ever");

	obj.img.img = mlx_new_image(obj.img.mlx, SCREENX, SCREENY);
	obj.img.addr = mlx_get_data_addr(obj.img.img, &obj.img.bits_per_pixel, &obj.img.line_length, &obj.img.endian);
	draw_all(&obj, &obj.img);
	mlx_hook(obj.img.win, KeyPress, KeyPressMask, read_input, &obj);
    mlx_hook(obj.img.win, ButtonPress, ButtonPressMask, read_mouse, &obj);
    mlx_hook(obj.img.win, ButtonRelease, ButtonReleaseMask, release_mouse, &obj);
    mlx_hook(obj.img.win, MotionNotify, PointerMotionMask, handle_mouse_move, &obj);
    mlx_loop_hook(obj.img.mlx, update, &obj);
    mlx_loop(obj.img.mlx);
}
