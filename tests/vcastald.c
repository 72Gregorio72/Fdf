/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vcastald.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpicchio <gpicchio@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 12:29:33 by vcastald          #+#    #+#             */
/*   Updated: 2025/01/23 12:34:14 by gpicchio         ###   ########.fr       */
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

int main()
{
	t_data img;
	img.mlx = mlx_init();
	img.win = mlx_new_window(img.mlx, SCREENX, SCREENY, "!(!(vcastald gay))");
	mlx_loop(img.mlx);
}