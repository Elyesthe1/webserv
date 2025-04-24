/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tovetouc <tovetouc@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/23 15:32:44 by tovetouc          #+#    #+#             */
/*   Updated: 2024/12/23 15:51:12 by tovetouc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "mlx.h"
# include "../src/get_next_line/get_next_line.h"
# include <X11/X.h>
# include <X11/keysym.h>
# include <errno.h>
# include <fcntl.h>
# include <limits.h>
# include <math.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define RED "\e[91m"
# define RESET "\033[0m"

# define WIDTH 1600
# define HEIGHT 900
# define W 119
# define S 115
# define D 100
# define A 97
# define UP 65362
# define LEFT 65361
# define DOWN 65364
# define RIGHT 65363
# define SPEED 0.005
# define ESC 65307
# define ANGLE_SPEED 0.02
# define SHIFT 65505
# define CTRL 65507
# define SPACE 32

# define ERR_FIELD_EMPTY "Error\n[\e[91m!\033[0m] The '%s' field is empty\n"
# define ERR_NO_PLAYER "Error\n[\e[91m!\033[0m] No player in grid\n"

typedef struct s_tex
{
	void				*img[4];
	int					*tex[4];
	int					width;
	int					height;
	int					bpp;
	int					size_line;
	int					endian;
	double				wallx;
	int					texx;
	double				texpos;
	int					texy;
	double				step;
	int					num;
	struct s_map		**map;
}						t_tex;

typedef struct s_mlx
{
	void				*mlx;
	void				*win;
	void				*img;
	char				*addr;
	int					bits_per_pixel;
	int					size_line;
	int					end;
}						t_mlx;

typedef struct s_map
{
	char				*north_texture;
	char				*south_texture;
	char				*west_texture;
	char				*east_texture;
	int					*floor_color;
	int					*ceiling_color;
	int					**grid;
	int					*grid_x;
	int					*grid_y;
	int					*player_x;
	int					*player_y;
	char				*player_direction;
}						t_map;

typedef struct s_player
{
	int					action;
	double				x;
	double				y;
	double				dirx;
	double				diry;
	double				planex;
	double				planey;
	int					**map;
	int					map_x;
	int					map_y;
	bool				shift;
	int					run;
	bool				keyw;
	bool				keya;
	bool				keys;
	bool				keyd;
	bool				keyup;
	bool				keyleft;
	bool				keyright;
	bool				keydown;
	int					floor_color;
	int					ceiling_color;
	struct s_ray		*ray;
	struct s_mlx		*mlx;
	struct s_tex		*texture;
	t_map				**parsing;
}						t_player;

typedef struct s_map_line
{
	char				*line;
	struct s_map_line	*next;
}						t_map_line;
typedef struct s_ray
{
	double				sidedistx;
	double				sidedisty;
	int					mapx;
	int					mapy;
	double				deltadistx;
	double				deltadisty;
	double				walldist;
	double				raydirx;
	double				raydiry;
	double				camerax;
	double				stepx;
	double				stepy;
	int					lineh;
	int					drawstart;
	int					drawend;
	int					color;
	int					x;
	int					y;
	int					side;
	t_player			player;
}						t_ray;

void					my_mlx_pixel_put(t_mlx *mlx, int x, int y, int color);

int						sortie(int key, t_player *player);
int						keypress(int key, t_player *player);
int						keyrelease(int key, t_player *player);
void					moove_player(t_player *player);
void					init_mlx(t_mlx *mlx, t_player *player, t_tex *tex,
							t_map **map);
bool					hit(float px, float py, t_player *player);
int						draw(t_player *player);
void					init_value(t_ray *ray, t_player *player);

int						croix(t_player *mlx);

void					put_pixel(int x, int y, int color, t_mlx *mlx);

int						ft_strcmp(const char *s1, const char *s2);
void					*ft_memset(void *b, int c, size_t len);
void					*ft_calloc(size_t count, size_t size);
void					*ft_memcpy(void *dst, const void *src, size_t n);
char					**ft_split(char const *s, char c);
void					ft_free_split(char **split);
int						ft_split_size(char **split);
char					*ft_strdupn(char *str, int len);
int						ft_isdigit(int c);
int						ft_isint(char *str);
int						ft_atoi(const char *str);
void					mooveup(t_player *player);
void					moovedown(t_player *player);

void					rotate_player(t_player *player, double angle);
void					drawline(t_ray *ray, t_player *player, t_tex *tex);
void					texture(t_ray *ray, t_player *player, t_tex *tex);
void					pixel_calcul(t_ray *ray);
void					start_dna(t_ray *ray, t_player *player);
void					set_direction(t_ray *ray, t_player *player);
int						keypress(int key, t_player *player);
int						keyrelease(int key, t_player *player);
void					moove_player(t_player *player);

void					put_pixel(int x, int y, int color, t_mlx *mlx);

void					split_lines(char *content, t_map_line **map_lines);
int						has_map_file(int ac);
int						is_map_file_extension_valid(char *map_file);
int						read_map_file(int map_fd, t_map_line **map_lines);
int						open_map_file(char *map_file, int *map_fd);
int						initialize_map(t_map **map);
int						parsing(int ac, char **av, t_map **map,
							t_player *player);
size_t					ft_strlen_delim(char const *s, char c);
void					free_map_lines(t_map_line **map_lines);
void					next_map_line(t_map_line **map_lines);
void					free_map(t_map **map);
int						is_grid(t_map_line **map_lines);
bool					is_grid_valid(t_map_line *map_lines);
int						get_int_from_grid_char(char c);
int						is_grid_closed(t_map *map);
void					alloc_grid(t_map_line *map_lines, t_map **map);
bool					is_color(char *str);
void					assign_color(char *field, char **color, t_map **map);
int						handle_color(char *str, t_map **map);
bool					is_field_uninitialized(char *field, t_map **map);
int						setup_grid(t_map_line *map_lines, t_map **map);
int						parse_grid(t_map_line **map_lines, t_map **map);
bool					is_texture(char *str);
void					assign_to_texture(char *field, char *value,
							t_map **map);
int						handle_texture(char *str, t_map **map);
bool					is_line_empty(char *line);
bool					has_one_player_in_grid(t_map **map);
void					set_player_data(t_map *map, t_player *player);
int						set_player_pos(t_map *map);
void					unexpected_line(t_map_line *map_lines);
void					missing_field(void);
void					multiple_players(void);
void					color_len(int size, char *field_name);
void					color_range(char *field_name);
void					draw_fps(t_player *player);
char					*ft_itoa(int n);

// int	parse_map(int map_fd);
#endif
