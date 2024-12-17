/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   0_parsing.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   by: zoolszew <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:49:30 by zoolszew          #+#    #+#             */
/*   Updated: 2024/12/17 16:49:32 by zoolszew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"



void	initbivector(t_camera *camera, t_vec3 *model, t_bivec3 *model_t)
{
	int i;
	t_coord sqr;
	
	sqr.l = 0;
	i = 0;
	while (sqr.l < (*camera).modelheight)
	{
		sqr.c = 0;
		while (sqr.c < (*camera).modelwidth - 1)
		{
			(*(model_t + i)).a = *(model + sqr.l * (*camera).modelwidth + sqr.c);
			(*(model_t + i)).b = *(model + sqr.l * (*camera).modelwidth + sqr.c + 1);
			++sqr.c;
			++i;
		}
		++sqr.l;
	}
	sqr.c = 0;
	while (sqr.c < (*camera).modelwidth)
	{
		sqr.l = 0;
		while (sqr.l < (*camera).modelheight - 1)
		{
			(*(model_t + i)).a = *(model + sqr.l * (*camera).modelwidth + sqr.c);
			(*(model_t + i)).b = *(model + (sqr.l + 1) * (*camera).modelwidth + sqr.c);
			++sqr.l;
			++i;
		}
		++sqr.c;
	}
}

void	fill3dspace(t_file *file, t_camera *camera, t_screenspace *screenspace)
{
	t_coord	sqr;
	int		i;
	int		z;
	int		x;

	(*camera).model = NULL;
	(*camera).model = malloc(((*file).lines * (*file).columns * sizeof(t_vec3))
		+ ((*file).lines * ((*file).columns - 1) + ((*file).lines - 1) * (*file).columns) * sizeof(t_bivec3)
		+ COLOR_BUFFER_SIZE * sizeof(float));
	if ((*camera).model == NULL)
		return ;
	(*camera).model_t = (t_bivec3*)((*camera).model + ((*file).lines * (*file).columns));
	(*screenspace).z_buffer = (float*)((*camera).model + ((*file).lines * (*file).columns)
		+ ((*file).lines * ((*file).columns - 1) + ((*file).lines - 1) * (*file).columns));
	z = (*file).lines / 2;
	i = 0;
	sqr.l = 0;
	sqr.c = 0;
	x = -((*file).columns / 2);
	while (i < (*file).offset)
	{
		if (*((*file).str + i) == '\n')
		{
			while (sqr.c < (*file).columns)
			{
				(*((*camera).model + sqr.l * (*file).columns + sqr.c)).is_empty = 1;
				++(sqr.c);
			}
			--z;
			++(sqr.l);
			x = -((*file).columns / 2);
			++i;
			sqr.c = 0;
		}
		if (*((*file).str + i) >= '0' && *((*file).str + i) <= '9' || *((*file).str + i) == '-')
		{
			(*((*camera).model + sqr.l * (*file).columns + sqr.c)).y = ft_atoi_alt((*file).str, &i);
			(*((*camera).model + sqr.l * (*file).columns + sqr.c)).x = x;
			(*((*camera).model + sqr.l * (*file).columns + sqr.c)).z = z;
			(*((*camera).model + sqr.l * (*file).columns + sqr.c)).is_empty = 0;
		++x;
			++(sqr.c);
		}
		while (*((*file).str + i) != ' ' && *((*file).str + i) != '\n' && *((*file).str + i))
				++i;
		while (*((*file).str + i) == ' ')
				++i;
	}
}

void get_modelsize(t_file *file, t_camera *camera)
{
	int i = 0;
	int len = 0;

	while (i < (*file).offset)
	{
		if (*((*file).str + i) == '\n')
		{
			++((*file).lines);
			++i;
			if (len > (*file).columns)
				(*file).columns = len;
			len = 0;
		}
		if (*((*file).str + i) >= '0' && *((*file).str + i) <= '9' || *((*file).str + i) == '-')
		{
			++((*file).modelsize);
			++len;
			while (*((*file).str + i) >= '0' && *((*file).str + i) <= '9' || *((*file).str + i) == '-')
				++i;
		}
		while (*((*file).str + i) != ' ' && *((*file).str + i) != '\n' && *((*file).str + i))
				++i;
		while (*((*file).str + i) == ' ')
				++i;
	}
	(*camera).bivecsize = (*file).lines * ((*file).columns - 1) + ((*file).lines - 1) * (*file).columns; 
	(*camera).modelsize = (*file).lines * (*file).columns;
	(*camera).modelwidth = (*file).columns;
	(*camera).modelheight = (*file).lines;
	((*camera).trans).z = (*file).lines / 2;
}

char increasevector(t_file *file)
{
	char *strtmp;

	(*file).strsize = ((*file).strsize + (*file).readsize) << 1;
	strtmp = malloc((*file).strsize * sizeof(char));
	if (strtmp == NULL)
		return (free((*file).str), 1);
	(*file).readsize <<= 1;
	for (int i = 0; i < (*file).strsize; ++i)
		*(strtmp + i) = 0;
	for (int i = 0; i < (*file).offset; ++i)
		*(strtmp + i) = *((*file).str + i);
	free((*file).str);
	(*file).str = strtmp;
	return 0;
}

void parsing(char *path, t_camera *camera, t_screenspace *screenspace)
{
	t_file file = {
		.fd = -1,
		.modelsize = 0,
		.readbytes = 0,
		.readsize = 9,
		.strsize = 10,
		.offset = 0,
		.lines = 0,
		.columns = 0,
		.str = NULL
	};
	if (path == NULL)
		return (write(2, "NULL path.\n", 11), (void)0);
	file.fd = open(path, O_RDONLY);
	if (file.fd == -1)
		return (void)0;
	file.str = malloc(file.strsize * sizeof(char));
	if (file.str == NULL)
		return (write(2, "malloc failed.\n", 15), (void)0);
	file.readbytes = read(file.fd, file.str, file.readsize);
	file.offset += file.readbytes;
	while (file.readbytes > 0)
	{
		if (file.offset + file.readsize > file.strsize)
			if (increasevector(&file))
				return (write(2, "malloc failed.\n", 15), (void)0);
		file.readbytes = read(file.fd, file.str + file.offset, file.readsize);
		file.offset += file.readbytes * (file.readbytes > 0); 
	}
	get_modelsize(&file, camera);
	fill3dspace(&file, camera, screenspace);
	if ((*camera).model == NULL)
		return (write(2, "malloc failed.\n", 15), free(file.str), (void)0);
	initbivector(camera, (*camera).model, (*camera).model_t);
	free(file.str);
}
