#include <mlx.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "fdf.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define PI 3.14159265358979323846

#define KeyPressMask       (1L<<0)
#define KeyReleaseMask     (1L<<1)
#define KeyPress           2
#define KeyRelease         3
#define RED "\001\033[1;31m\002"
#define GREEN "\001\033[1;32m\002"
#define RESET "\001\033[0m\002"

typedef struct s_point {
	int x;
	int y;
	int z;
	int color;
	int exists;
}               t_point;

typedef enum e_projection {
    PROJECTION_ISOMETRIC,
    PROJECTION_SPHERICAL
}               t_projection;

typedef struct s_keys {
	int key_w;
	int key_s;
	int key_a;
	int key_d;
	int key_q;
	int key_e;
	int key_up;
	int key_down;
	int key_left;
	int key_right;
}               t_keys;

typedef struct s_mouse {
	int is_dragging;
	int prev_x;
	int prev_y;
}               t_mouse;

typedef struct s_map {
	int width;
	int height;
	t_point **points;
	int *row_lengths;
}               t_map;

typedef struct s_transform {
	double angle_x;
	double angle_y;
	double angle_z;
	double translate_x;
	double translate_y;
}               t_transform;

typedef struct s_data {
    void *mlx;
    void *win;
    void *img;
    char *addr;
    int bits_per_pixel;
    int line_length;
    int endian;
    t_map map;
    t_transform transform;
    t_keys keys;
    t_mouse mouse;
    double scale;
    t_projection projection;
}               t_data;

char *ft_strchr(const char *s, int c);

void    *ft_malloc_error(void)
{
	perror("Memory allocation failed");
	exit(EXIT_FAILURE);
	return NULL;
}

int hex_to_int(char *str)
{
	return (strtol(str, NULL, 16));
}

int is_valid_hex(char *str)
{
	
	if (*str != '0')
		return (0);
	str++;
	if (*str != 'x' && *str != 'X')
		return (0);
	str++;
	while (*str)
	{
		if (!((*str >= '0' && *str <= '9') || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F')))
		{
			if (*str == ' ' || *str == '\n')
				return (1);
			else
			{
				fprintf(stderr, "Error: Invalid hexadecimal character: "RED"%c\n", *str);
				return (0);
			}
		}
		str++;
	}
	return (1);
}

t_map read_map(char *filename)
{
	int     fd;
	char    *line;
	t_map   map;
	int     i = 0;
	int     current_row_width;
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror("Error opening file");
		exit(EXIT_FAILURE);
	}
	map.width = 0;
	map.height = 0;
	map.points = NULL;
	map.row_lengths = NULL;
	int last_printed_percentage = -1;
	while ((line = get_next_line(fd)) != NULL)
	{
		map.height++;
		current_row_width = 0;
		char *tmp = line;
		
		while (*tmp)
		{
			while (*tmp && (*tmp == ' ' || *tmp == '\n'))
				tmp++;
			if (*tmp)
			{
				current_row_width++;
				char *comma = ft_strchr(tmp, ',');
				if (comma)
				{
					comma++;
					if (!is_valid_hex(comma))
						exit(EXIT_FAILURE);
					*comma = ',';
					tmp = comma + 1;
				}
				else if (!ft_isdigit(*tmp) && *tmp != '-')
				{
					fprintf(stderr, "Error: Invalid map format. Only numbers or numbers with hexadecimal colors are allowed.\nFound: "RED"%c\n", *tmp);
					exit(EXIT_FAILURE);
				}
				while (*tmp && *tmp != ' ')
					tmp++;
			}
		}
		if (current_row_width > map.width)
			map.width = current_row_width;

		int percentage = (map.height * 100) / map.width;
		if (percentage != last_printed_percentage)
		{
			ft_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			ft_printf(RESET"        Lettura Mappa: %d%%(1/2)\n", percentage);
			int bar_width = 50;
			int filled_width = (percentage * bar_width) / 100;
			ft_printf(" [");
			for (int k = 0; k < bar_width; k++)
			{
				if (k < filled_width)
					ft_printf(GREEN"#");
				else
					ft_printf(" ");
			}
			ft_printf(RESET"]\n");
			ft_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			last_printed_percentage = percentage;
		}
		free(line);
	}
	close(fd);
	if (map.width == 0 || map.height == 0)
	{
		fprintf(stderr, "Error: Map is empty or improperly formatted.\n");
		exit(EXIT_FAILURE);
	}
	close(fd);
	map.row_lengths = malloc(sizeof(int) * map.height);
	if (!map.row_lengths)
		ft_malloc_error();
	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		perror("Error reopening file");
		exit(EXIT_FAILURE);
	}
	map.points = malloc(sizeof(t_point *) * map.height);
	if (!map.points)
		ft_malloc_error();
	for (i = 0; i < map.height; i++)
	{
		map.points[i] = malloc(sizeof(t_point) * map.width);
		if (!map.points[i])
			ft_malloc_error();
		for (int j = 0; j < map.width; j++)
			map.points[i][j].exists = 0;
	}
	last_printed_percentage = -1;
	i = 0;
	while ((line = get_next_line(fd)) != NULL && i < map.height)
	{
		int j = 0;
		char *token = strtok(line, " ");
		while (token && j < map.width)
		{
			if (j >= map.width)
				break;
			char *comma = ft_strchr(token, ',');
			if (comma)
			{
				*comma = '\0';
				map.points[i][j].z = ft_atoi(token);
				if (*(comma + 1) == '0' && (*(comma + 2) == 'x' || *(comma + 2) == 'X'))
				{
					map.points[i][j].color = hex_to_int(comma + 3);
				}
				else
				{
					map.points[i][j].color = 0xFFFFFF;
				}
			}
			else
			{
				map.points[i][j].z = ft_atoi(token);
				map.points[i][j].color = 0xFFFFFF;
			}
			map.points[i][j].exists = 1;
			j++;
			token = strtok(NULL, " ");
		}
		map.row_lengths[i] = j;
		for (; j < map.width; j++)
		{
			map.points[i][j].z = 0;
			map.points[i][j].color = 0x000000;
		}
		i++;
		int percentage = (i * 100) / map.height;
		if (percentage != last_printed_percentage)
		{
			ft_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			ft_printf(RESET"           Caricamento Mappa: %d%%(2/2)\n", percentage);
			
			int bar_width = 50;
			int filled_width = (percentage * bar_width) / 100;
			ft_printf(" [");
			for (int k = 0; k < bar_width; k++)
			{
				if (k < filled_width)
					ft_printf(GREEN"#");
				else
					ft_printf(" ");
			}
			ft_printf(RESET"]\n");
			ft_printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
			last_printed_percentage = percentage;
		}

		free(line);
	}
	close(fd);

	return map;
}

void init_map_points(t_map *map)
{
	int mid_x = map->width / 2;
	int mid_y = map->height / 2;
	for (int i = 0; i < map->height; i++)
	{
		for (int j = 0; j < map->width; j++)
		{
			if (map->points[i][j].exists)
			{
				map->points[i][j].x = j - mid_x;
				map->points[i][j].y = i - mid_y;
			}
			else
			{
				map->points[i][j].x = 0;
				map->points[i][j].y = 0;
				map->points[i][j].z = 0;
				map->points[i][j].color = 0x000000;
			}
		}
	}
}

t_point project_point_spherical(t_data *data, t_point p) {
    t_point projected;
    double r = 10;//fmin(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.0;
    double x_mercator = (p.x - SCREEN_WIDTH / 2.0) / r;
    double y_mercator = (p.y - SCREEN_HEIGHT / 2.0) / r;
    double longitude = x_mercator * PI;
    double latitude = 2 * atan(exp(y_mercator * PI)) - PI / 2;
    projected.x = r * cos(latitude) * cos(longitude);
    projected.y = r * cos(latitude) * sin(longitude);
    projected.z = r * sin(latitude);

    projected.color = p.color;
    return projected;
} // Assicurati di includere i tuoi file di intestazione

#define RADIUS 100000000

void rotate_3d(double *x, double *y, double *z, double angle_x, double angle_y, double angle_z)
{
    double rad_x = angle_x * PI / 180.0;
    double rad_y = angle_y * PI / 180.0;
    double rad_z = angle_z * PI / 180.0;
    double y_temp = *y * cos(rad_x) - *z * sin(rad_x);
    double z_temp = *y * sin(rad_x) + *z * cos(rad_x);
    *y = y_temp;
    *z = z_temp;
    double x_temp = *x * cos(rad_y) + *z * sin(rad_y);
    z_temp = -*x * sin(rad_y) + *z * cos(rad_y);
    *x = x_temp;
    *z = z_temp;
    x_temp = *x * cos(rad_z) - *y * sin(rad_z);
    y_temp = *x * sin(rad_z) + *y * cos(rad_z);
    *x = x_temp;
    *y = y_temp;
}

t_point project_point(t_data *data, t_point p)
{
    if (data->projection == PROJECTION_ISOMETRIC)
    {
		t_point projected;
		double x_rot, y_rot, z_rot;
		double height_scale = 0.2;
		double rad_x = data->transform.angle_x * PI / 180.0;
		y_rot = p.y * cos(rad_x) - (p.z * height_scale) * sin(rad_x);
		z_rot = p.y * sin(rad_x) + (p.z * height_scale) * cos(rad_x);
		double rad_y = data->transform.angle_y * PI / 180.0;
		double x_rot_temp = p.x * cos(rad_y) + z_rot * sin(rad_y);
		double z_rot_temp = -p.x * sin(rad_y) + z_rot * cos(rad_y);
		x_rot = x_rot_temp;
		z_rot = z_rot_temp;
		double rad_z = data->transform.angle_z * PI / 180.0;
		double x_final = x_rot * cos(rad_z) - y_rot * sin(rad_z);
		double y_final = x_rot * sin(rad_z) + y_rot * cos(rad_z);
		double scale = data->scale * fmin((double)SCREEN_WIDTH / (data->map.width * 2.0),
										  (double)SCREEN_HEIGHT / (data->map.height));
		projected.x = (int)(x_final * scale) + SCREEN_WIDTH / 2 + (int)data->transform.translate_x;
		projected.y = (int)(y_final * scale) + SCREEN_HEIGHT / 2 + (int)data->transform.translate_y;
		projected.color = p.color;
	
		return projected;
	}	
    else if (data->projection == PROJECTION_SPHERICAL)
    {
        t_point projected;
        double sphere_radius = fmin(SCREEN_WIDTH, SCREEN_HEIGHT) / 2.5;
        double u = (double)p.x / (data->map.width - 1);
        double v = (double)p.y / (data->map.height - 1);
        double lon = (u * 2.0 * PI) - PI;
        double lat = ((1.0 - v) * PI) - (PI);
        double x_sphere = cos(lat) * cos(lon);
        double y_sphere = sin(lat);
        double z_sphere = cos(lat) * sin(lon);
        double height_factor = 10.0;
        double scale = ((1.0 + (p.z * height_factor / sphere_radius)) / (data->map.height * 0.1)) * data->scale;
        x_sphere *= scale;
        y_sphere *= scale;
        z_sphere *= scale;
        rotate_3d(&x_sphere, &y_sphere, &z_sphere,
                  data->transform.angle_x, 
                  data->transform.angle_y, 
                  data->transform.angle_z);
        projected.x = x_sphere * sphere_radius + SCREEN_WIDTH / 2 + data->transform.translate_x;
        projected.y = y_sphere * sphere_radius + SCREEN_HEIGHT / 2 + data->transform.translate_y;
        projected.z = z_sphere * sphere_radius;
        projected.color = p.color;
        return (projected);
    }
    t_point default_proj = {0, 0, 0, 0xFFFFFF, 1};
    return default_proj;
}


void my_mlx_pixel_put(t_data *data, int x, int y, int color)
{
	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
		return;
	char *dst;

	dst = data->addr + (y * data->line_length + x * (data->bits_per_pixel / 8));
	*(unsigned int*)dst = color;
}

unsigned int interpolate_color(unsigned int color1, unsigned int color2, double t)
{
    if (t > 1.0)
        t = 1.0;
    if (t < 0.0)
        t = 0.0;
    unsigned char r1 = (color1 >> 16) & 0xFF;
    unsigned char g1 = (color1 >> 8) & 0xFF;
    unsigned char b1 = color1 & 0xFF;
    unsigned char r2 = (color2 >> 16) & 0xFF;
    unsigned char g2 = (color2 >> 8) & 0xFF;
    unsigned char b2 = color2 & 0xFF;
    unsigned char r = r1 + (unsigned char)((r2 - r1) * t);
    unsigned char g = g1 + (unsigned char)((g2 - g1) * t);
    unsigned char b = b1 + (unsigned char)((b2 - b1) * t);
    return (r << 16) | (g << 8) | b;
}

void draw_line(t_data *data, t_point p1, t_point p2)
{
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);
    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;
    int err = dx - dy;
    double length = sqrt((p2.x - p1.x) * (p2.x - p1.x) + 
                         (p2.y - p1.y) * (p2.y - p1.y));
    double t = 0.0;
    int steps = 0;
    int max_steps = (int)length;

    while (1)
    {
        double current_t = (length != 0) ? (double)steps / length : 0;
        my_mlx_pixel_put(data, p1.x, p1.y, interpolate_color(p1.color, p2.color, current_t));
        if (p1.x == p2.x && p1.y == p2.y)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            p1.y += sy;
        }
        steps++;
    }
}

void draw_map(t_data *data)
{
	for (int i = 0; i < data->map.height; i++)
	{
		for (int j = 0; j < data->map.width; j++)
		{
			if (!data->map.points[i][j].exists)
				continue;

			t_point p = project_point(data, data->map.points[i][j]);

			if (j < data->map.width - 1 && data->map.points[i][j + 1].exists)
			{
				t_point right = project_point(data, data->map.points[i][j + 1]);
				draw_line(data, p, right);
			}

			if (i < data->map.height - 1 && data->map.points[i + 1][j].exists)
			{
				t_point down = project_point(data, data->map.points[i + 1][j]);
				draw_line(data, p, down);
			}
		}
	}
}

void draw_ui(t_data *data)
{
	int start_x = 10;
	int start_y = 10;
	int line_height = 20;
	int color = 0x00FF00;

	mlx_string_put(data->mlx, data->win, start_x, start_y, color, "Comandi:");
	mlx_string_put(data->mlx, data->win, start_x, start_y + line_height, color, "W/S: Ruota sull'asse X");
	mlx_string_put(data->mlx, data->win, start_x, start_y + 2 * line_height, color, "A/D: Ruota sull'asse Y");
	mlx_string_put(data->mlx, data->win, start_x, start_y + 3 * line_height, color, "Q/E: Ruota sull'asse Z");
	mlx_string_put(data->mlx, data->win, start_x, start_y + 4 * line_height, color, "Freccette: Trasla");
	mlx_string_put(data->mlx, data->win, start_x, start_y + 5 * line_height, color, "F: Cambia proiezione(Sferica-Isometrica)");
	mlx_string_put(data->mlx, data->win, start_x, start_y + 6 * line_height, color, "Esc: Esci");
}

void update_image(t_data *data)
{
	ft_memset(data->addr, 0, SCREEN_WIDTH * SCREEN_HEIGHT * (data->bits_per_pixel / 8));
	draw_map(data);
	mlx_put_image_to_window(data->mlx, data->win, data->img, 0, 0);
	draw_ui(data);
}

int update(t_data *data)
{
	int updated = 0;
	float increment = 0.2 / (data->scale * 0.2);

	if (data->map.width > 300 && data->map.height > 300)
	{
		increment = 5;
	}
	if (data->keys.key_w)
	{
		data->transform.angle_x += increment;
		updated = 1;
	}
	if (data->keys.key_s)
	{
		data->transform.angle_x -= increment;
		updated = 1;
	}
	if (data->keys.key_a)
	{
		data->transform.angle_y -= increment;
		updated = 1;
	}
	if (data->keys.key_d)
	{
		data->transform.angle_y += increment;
		updated = 1;
	}
	if (data->keys.key_q)
	{
		data->transform.angle_z -= increment;
		updated = 1;
	}
	if (data->keys.key_e)
	{
		data->transform.angle_z += increment;
		updated = 1;
	}
	if (data->keys.key_up)
	{
		data->transform.translate_y -= (10 * data->scale) * 0.2;
		updated = 1;
	}
	if (data->keys.key_down)
	{
		data->transform.translate_y += (10 * data->scale) * 0.2;
		updated = 1;
	}
	if (data->keys.key_left)
	{
		data->transform.translate_x -= (10 * data->scale) * 0.2;
		updated = 1;
	}
	if (data->keys.key_right)
	{
		data->transform.translate_x += (10 * data->scale) * 0.2;
		updated = 1;
	}
	if (updated)
		update_image(data);

	return (0);
}

int handle_keypress_event(int key, t_data *data)
{
	if (key == 65307)
		exit(0);
	else if (key == 119)
		data->keys.key_w = 1;
	else if (key == 115)
		data->keys.key_s = 1;
	else if (key == 97)
		data->keys.key_a = 1;
	else if (key == 100)
		data->keys.key_d = 1;
	else if (key == 113)
		data->keys.key_q = 1;
	else if (key == 101)
		data->keys.key_e = 1;
	else if (key == 102)
	{
		if (data->projection == PROJECTION_ISOMETRIC)
			data->projection = PROJECTION_SPHERICAL;
		else
			data->projection = PROJECTION_ISOMETRIC;
		update_image(data);
	}
	else if (key == 65362)
		data->keys.key_up = 1;
	else if (key == 65364)
		data->keys.key_down = 1;
	else if (key == 65361)
		data->keys.key_left = 1;
	else if (key == 65363)
		data->keys.key_right = 1;
	return (0);
}

int handle_keyrelease_event(int key, t_data *data)
{
	if (key == 119)
		data->keys.key_w = 0;
	else if (key == 115)
		data->keys.key_s = 0;
	else if (key == 97)
		data->keys.key_a = 0;
	else if (key == 100)
		data->keys.key_d = 0;
	else if (key == 113)
		data->keys.key_q = 0;
	else if (key == 101)
		data->keys.key_e = 0;
	else if (key == 65362)
		data->keys.key_up = 0;
	else if (key == 65364)
		data->keys.key_down = 0;
	else if (key == 65361)
		data->keys.key_left = 0;
	else if (key == 65363)
		data->keys.key_right = 0;
	else if (key == 65307)
		exit(0);
	return (0);
}

int handle_mouse_press(int button, int x, int y, t_data *data)
{
	if (button == 1)
	{
		data->mouse.is_dragging = 1;
		data->mouse.prev_x = x;
		data->mouse.prev_y = y;
	}
	return (0);
}

int handle_mouse_release(int button, int x, int y, t_data *data)
{
	if (button == 1)
	{
		data->mouse.is_dragging = 0;
	}
	return (0);
}

int handle_mouse_event(int button, int x, int y, t_data *data)
{
	if (button == 4)
	{
		if (data->scale < 10)
			data->scale *= 1.1;
		update_image(data);
	}
	else if (button == 5)
	{
		if (data->scale > 0.1)
			data->scale /= 1.1;
		update_image(data);
	}
	return (0);
}

int handle_mouse_move_event(int x, int y, t_data *data)
{
	if (data->mouse.is_dragging)
	{
		int delta_x = x - data->mouse.prev_x;
		int delta_y = y - data->mouse.prev_y;
		data->transform.translate_x += delta_x;
		data->transform.translate_y += delta_y;
		data->mouse.prev_x = x;
		data->mouse.prev_y = y;
		update_image(data);
	}
	return (0);
}

int handle_close(t_data *data)
{
	for (int i = 0; i < data->map.height; i++)
	{
		free(data->map.points[i]);
	}
	free(data->map.points);
	free(data->map.row_lengths);
	mlx_destroy_image(data->mlx, data->img);
	mlx_destroy_window(data->mlx, data->win);
	exit(0);
	return (0);
}

int main(int argc, char **argv)
{
	t_data  data;
	if (argc != 2)
	{
		ft_printf("Usage: %s <map_file>\n", argv[0]);
		return (0);
	}
	data.map = read_map(argv[1]);
	init_map_points(&data.map);
	data.mlx = mlx_init();
	if (!data.mlx)
	{
		perror("MLX initialization failed");
		exit(EXIT_FAILURE);
	}
	data.win = mlx_new_window(data.mlx, SCREEN_WIDTH, SCREEN_HEIGHT, "FDF");
	if (!data.win)
	{
		perror("Window creation failed");
		exit(EXIT_FAILURE);
	}
	data.img = mlx_new_image(data.mlx, SCREEN_WIDTH, SCREEN_HEIGHT);
	data.addr = mlx_get_data_addr(data.img, &data.bits_per_pixel, &data.line_length, &data.endian);
	data.transform.angle_x = 30;
	data.transform.angle_y = 30;
	data.transform.angle_z = 0;
	data.transform.translate_x = 0.0;
	data.transform.translate_y = 0.0;
	ft_memset(&data.keys, 0, sizeof(t_keys));
	ft_memset(&data.mouse, 0, sizeof(t_mouse));
	data.scale = 1.0;
    data.projection = PROJECTION_ISOMETRIC;
	update_image(&data);
	mlx_hook(data.win, KeyPress, KeyPressMask, handle_keypress_event, &data);
	mlx_hook(data.win, KeyRelease, KeyReleaseMask, handle_keyrelease_event, &data);
	mlx_mouse_hook(data.win, handle_mouse_event, &data);
	mlx_hook(data.win, 6, 1L<<6, handle_mouse_move_event, &data);
	mlx_hook(data.win, 17, 0L, handle_close, &data);
	
	mlx_loop_hook(data.mlx, update, &data);
	mlx_loop(data.mlx);
	return (0);
}