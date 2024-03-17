#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t );
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	c = {vec3(0),1.0f,0.0f,vec4(1.0f,0.5f,0.5f,1.0f)};
	circles.emplace_back(c);

	return circles;
}

inline void circle_t::update( float t )
{
	theta	= t;
	float c	= cos(theta), s=sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	// x-axis
	mat4 rotate_x{
		1, 0, 0, 0,
		0, c, -s, 0,
		0, s, c, 0,
		0, 0, 0, 1
	};
	// y-axis
	mat4 rotate_y{
		c, 0, s, 0,
		0, 1, 0, 0,
		-s,0, c, 0,
		0, 0, 0, 1
	};
	// z-axis
	mat4 rotate_z{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	mat4 rotation_matrix = rotate_z;

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
	//model_matrix = scale_matrix * rotation_matrix * translate_matrix;

}

#endif
