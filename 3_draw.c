/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_draw.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zoolszew <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 16:50:06 by zoolszew          #+#    #+#             */
/*   Updated: 2024/12/17 16:50:09 by zoolszew         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fdf.h"

void put_pixel(int x, int y, t_screenspace *screenspace)
{
	*((*screenspace).color_buffer + (y + (*screenspace).y_offset)
		* WIDTH + x + (*screenspace).x_offset) = DEFAULT_VECTOR_COLOR;
}

void rotatemodel(t_bivec3 *bivector, t_cosin *cs_x, t_cosin *cs_y)
{
	float x;
	float y;
	float z;

	z = (*bivector).a.z * (*cs_x).cos - (*bivector).a.y * (*cs_x).sin;
	y = (*bivector).a.y * (*cs_x).cos + (*bivector).a.z * (*cs_x).sin;
	(*bivector).a.z = z;
	(*bivector).a.y = y;
	x = (*bivector).a.x * (*cs_y).cos - (*bivector).a.z * (*cs_y).sin;
	z = (*bivector).a.z * (*cs_y).cos + (*bivector).a.x * (*cs_y).sin;
	(*bivector).a.x = x;
	(*bivector).a.z = z;
	z = (*bivector).b.z * (*cs_x).cos - (*bivector).b.y * (*cs_x).sin;
	y = (*bivector).b.y * (*cs_x).cos + (*bivector).b.z * (*cs_x).sin;
	(*bivector).b.z = z;
	(*bivector).b.y = y;
	x = (*bivector).b.x * (*cs_y).cos - (*bivector).b.z * (*cs_y).sin;
	z = (*bivector).b.z * (*cs_y).cos + (*bivector).b.x * (*cs_y).sin;
	(*bivector).b.x = x;
	(*bivector).b.z = z;
}

void add_translations(t_bivec3 *bivector, t_vec3 *trans)
{
	(*bivector).a.x += (*trans).x;
	(*bivector).a.y += (*trans).y;
	(*bivector).a.z += (*trans).z;	
	(*bivector).b.x += (*trans).x;
	(*bivector).b.y += (*trans).y;
	(*bivector).b.z += (*trans).z;
}

void drawline(t_bivec3 *bivec, t_screenspace *screenspace, float steps)
{
	float xnc;
	float ync;
    float dx = (*bivec).b.x - (*bivec).a.x;
    float dy = (*bivec).b.y - (*bivec).a.y;
	if (fabs(dx) > fabs(dy))
		steps = fabs(dx);
	else
		steps = fabs(dy);
    xnc = dx / steps;
    ync = dy / steps;
    for (int i = 0; i <= steps; ++i) {
		put_pixel((int)round((*bivec).a.x), (int)round((*bivec).a.y), screenspace);
        (*bivec).a.x += xnc;
        (*bivec).a.y += ync;
}
}

void rotate_camera(t_bivec3 *bivector, t_camera *camera)
{
	float x;
	float y;
	float z;

	x = (*bivector).a.x * (*camera).angle_y.cos - (*bivector).a.z * (*camera).angle_y.sin;
	z = (*bivector).a.z * (*camera).angle_y.cos + (*bivector).a.x * (*camera).angle_y.sin;
	(*bivector).a.x = x;
	(*bivector).a.z = z;
	x = (*bivector).b.x * (*camera).angle_y.cos - (*bivector).b.z * (*camera).angle_y.sin;
	z = (*bivector).b.z * (*camera).angle_y.cos + (*bivector).b.x * (*camera).angle_y.sin;
	(*bivector).b.x = x;
	(*bivector).b.z = z;

	z = (*bivector).a.z * (*camera).angle_x.cos - (*bivector).a.y * (*camera).angle_x.sin;
	y = (*bivector).a.y * (*camera).angle_x.cos + (*bivector).a.z * (*camera).angle_x.sin;
	(*bivector).a.z = z;
	(*bivector).a.y = y;
	z = (*bivector).b.z * (*camera).angle_x.cos - (*bivector).b.y * (*camera).angle_x.sin;
	y = (*bivector).b.y * (*camera).angle_x.cos + (*bivector).b.z * (*camera).angle_x.sin;
	(*bivector).b.z = z;
	(*bivector).b.y = y;
}

void rotate_iso(t_bivec3 *bivector)
{
	float x;
	float y;
	float z;

	x = (*bivector).a.x * cos(-0.7854) - (*bivector).a.z * sin(-0.7854);
	z = (*bivector).a.z * cos(-0.7854) + (*bivector).a.x * sin(-0.7854);
	(*bivector).a.x = x;
	(*bivector).a.z = z;
	z = (*bivector).a.z * cos(-0.61548) - (*bivector).a.y * sin(-0.61548);
	y = (*bivector).a.y * cos(-0.61548) + (*bivector).a.z * sin(-0.61548);
	(*bivector).a.z = z;
	(*bivector).a.y = y;
	x = (*bivector).b.x * cos(-0.7854) - (*bivector).b.z * sin(-0.7854);
	z = (*bivector).b.z * cos(-0.7854) + (*bivector).b.x * sin(-0.7854);
	(*bivector).b.x = x;
	(*bivector).b.z = z;
	z = (*bivector).b.z * cos(-0.61548) - (*bivector).b.y * sin(-0.61548);
	y = (*bivector).b.y * cos(-0.61548) + (*bivector).b.z * sin(-0.61548);
	(*bivector).b.z = z;
	(*bivector).b.y = y;
}

void project_iso(t_camera *camera, t_screenspace *screenspace, int i)
{
	float		zoom;
	t_bivec3	bivector;

	i = 0;
	while (i < (*camera).bivecsize)
	{
		if (!((*((*camera).model_t + i)).a.is_empty) && !((*((*camera).model_t + i)).b.is_empty)) 
		{
			rotatemodel((*camera).model_t + i, &((*camera).cosin_x), &((*camera).cosin_y));
			bivector = *((*camera).model_t + i);
			rotate_iso(&bivector);
			add_translations(&bivector, &((*camera).trans));
			rotate_camera(&bivector, camera);
			zoom = (*camera).zoom / 15;
			bivector.a.x = (bivector).a.x * zoom;
			bivector.a.y = (bivector).a.y * zoom;
			bivector.b.x = (bivector).b.x * zoom;
			bivector.b.y = (bivector).b.y * zoom;
			if ((bivector.a.x >= -((*screenspace).x_offset) && bivector.a.x <= ((*screenspace).x_offset))
					&& (bivector.a.y >= -((*screenspace).y_offset) && bivector.a.y <= ((*screenspace).y_offset))
					&& (bivector.b.x >= -((*screenspace).x_offset) && bivector.b.x <= ((*screenspace).x_offset))
					&& (bivector.b.y >= -((*screenspace).y_offset) && bivector.b.y <= ((*screenspace).y_offset)))
				drawline(&bivector, screenspace, 0);
		}
		++i;
	}
}

void project(t_camera *camera, t_screenspace *screenspace, int i)
{
	float		z;
	t_bivec3	bivector;
	i = 0;
	while (i < (*camera).bivecsize)
	{
		if (!((*((*camera).model_t + i)).a.is_empty) && !((*((*camera).model_t + i)).b.is_empty)) 
		{
			rotatemodel((*camera).model_t + i, &((*camera).cosin_x), &((*camera).cosin_y));
			bivector = *((*camera).model_t + i);
			add_translations(&bivector, &((*camera).trans));
			rotate_camera(&bivector, camera);
			if (((bivector).a.z) > ZMIN && ((bivector).b.z) > ZMIN)
			{
				z = (*camera).zoom / (bivector).a.z;
				bivector.a.x = (bivector).a.x * z;
				bivector.a.y = (bivector).a.y * z;
				z = (*camera).zoom / (bivector).b.z;
				bivector.b.x = (bivector).b.x * z;
				bivector.b.y = (bivector).b.y * z;
				if ((bivector.a.x >= -((*screenspace).x_offset) && bivector.a.x <= ((*screenspace).x_offset))
						&& (bivector.a.y >= -((*screenspace).y_offset) && bivector.a.y <= ((*screenspace).y_offset))
						&& (bivector.b.x >= -((*screenspace).x_offset) && bivector.b.x <= ((*screenspace).x_offset))
						&& (bivector.b.y >= -((*screenspace).y_offset) && bivector.b.y <= ((*screenspace).y_offset)))
					drawline(&bivector, screenspace, 0);
			}
		}
		++i;
	}
}

void clear(t_scene *scene)
{
	for (int i = 0; i < COLOR_BUFFER_SIZE; ++i)
		*((*(*scene).screenspace).color_buffer + i) = BACKGROUND_COLOR;
}

void drawsquare(t_scene *scene , int size, t_vec2 cursor)
{
	t_screenspace *screenspace = (*scene).screenspace;
	int x;

	size = (size >> 1) + ((size <= 1) >> 1);

	for (int y = (*screenspace).y_offset - size + cursor.y; y < (*screenspace).y_offset + size + cursor.y; ++y)
	{
		if ((y < HEIGHT) && (y > -1))
		{
			x = (*screenspace).x_offset - size + cursor.x;
			for (; x < (*screenspace).x_offset + size + cursor.x; ++x)
				if ((x < WIDTH) && (x > -1))
					*((*(*scene).screenspace).color_buffer + y * WIDTH + x) = 0x0000ff00;
		}
	}
}
