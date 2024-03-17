#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	vec3	center = vec3(0);		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	theta = 0.0f;			// rotation angle
	float rot_speed = 0.0f;
	float rev_speed = 0.0f;
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};

inline std::vector<circle_t> create_circles()
{	
	vec3 center;
	float radius, theta, rot_speed, rev_speed;
	vec4 color_p = vec4(0.5f, 1.0f, 1.0f, 1.0f);

	std::vector<circle_t> circles;
	circle_t c;

	// sun 0
	center = vec3(0, 0, 0);
	radius = 30.0f;
	theta = 0.0f;
	rot_speed = 0.2f;
	rev_speed = 0.0f;
	c = { center, radius, theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// mercury 1
	center = vec3(40.0f, -40.0f, 0);
	radius = 0.4f;
	theta = 0.0f;
	rot_speed = 0.3f;
	rev_speed = 4.0f;
	c = { center, radius * 4, theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// venus 2
	center = vec3(55.0f, -55.0f, 0);
	radius = 0.9f;
	theta = 0.0f;
	rot_speed = 0.2f;
	rev_speed = 3.0f;
	c = { center, radius * 4, theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// earth 3
	center = vec3(70.0f, 70.0f, 0);
	radius = 1.0f;
	theta = 0.0f;
	rot_speed = 0.09f;
	rev_speed = 2.0f;
	c = { center, radius * 4, theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// mars 4
	center = vec3(95.0f, -95.0f, 0);
	radius = 0.5f;
	theta = 0.0f;
	rot_speed = 0.9f;
	rev_speed = 1.2f;
	c = { center, radius * 4, theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// jupiter 5
	center = vec3(-100.0f, 100.0f, 0);
	radius = 11.2f;
	theta = 0.0f;
	rot_speed = 1.6f;
	rev_speed = 1.0f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// saturn 6
	center = vec3(-110.0f, -110.0f, 0);
	radius = 9.4f;
	theta = 0.0f;
	rot_speed = 2.0f;
	rev_speed = 0.8f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// uranus 7
	center = vec3(-150.0f, 150.0f, 0);
	radius = 4.0f;
	theta = 0.0f;
	rot_speed = 1.3f;
	rev_speed = 0.6f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// neptune 8
	center = vec3(210.0f, -210.0f, 0);
	radius = 3.9f;
	theta = 0.0f;
	rot_speed = 1.4f;
	rev_speed = 0.4f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	
	return circles;
}

inline void circle_t::update(float t)
{
	theta = t;
	//float c = cos(theta), s = sin(theta);
	float c, s;
	mat4 rotate_z;

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};

	c = cos(rot_speed * theta);
	s = sin(rot_speed * theta);
	rotate_z = 
	{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	mat4 rotation_matrix = rotate_z;
	c = cos(rev_speed * theta);
	s = sin(rev_speed * theta);
	rotate_z =
	{
		c, -s, 0, 0,
		s, c, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 revolution_matrix = rotate_z;

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};

	//model_matrix = translate_matrix * rotation_matrix * scale_matrix * revolution_matrix;
	//model_matrix = scale_matrix * rotation_matrix * translate_matrix;
	model_matrix = rotation_matrix * translate_matrix * scale_matrix * revolution_matrix;


}

#endif
