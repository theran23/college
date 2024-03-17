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

struct dwarf_t
{
	vec3	relative_center = vec3(0);		// 2D position for translation
	float	radius = 1.0f;		// radius
	float	theta = 0.0f;			// rotation angle
	float rot_speed = 0.0f;
	float rev_speed = 0.0f;
	vec3 planet_center = vec3(0);
	float planet_rev_speed = 0.0f;
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update(float t);
};

struct ring_t
{
	float	interval = 1.0f;
	float	thickness = 0.0f;			
	vec3 planet_center = vec3(0);
	float planet_radius = 0.0f;
	float rotate_speed = 0.0f;
	// vec4	color;				// RGBA color in [0,1]
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
	center = vec3(-130.0f, 130.0f, 0);
	radius = 11.2f;
	theta = 0.0f;
	rot_speed = 1.6f;
	rev_speed = 1.0f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// saturn 6
	center = vec3(-150.0f, -150.0f, 0);
	radius = 9.4f;
	theta = 0.0f;
	rot_speed = 2.0f;
	rev_speed = 0.8f;
	c = { center, radius , theta, rot_speed, rev_speed, color_p };
	circles.emplace_back(c);
	// uranus 7
	center = vec3(-180.0f, 180.0f, 0);
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

inline std::vector<dwarf_t> create_dwarf_planets(circle_t earth, circle_t jupiter, circle_t saturn, circle_t neptune)
{
	vec3 relative_center;
	vec3 planet_center;
	float planet_rev_speed;
	vec4 color_p = vec4(0.5f, 1.0f, 1.0f, 1.0f);

	std::vector<dwarf_t> dwarf_planets;
	dwarf_t d;

	//// earth 
	planet_center = earth.center;
	planet_rev_speed = earth.rev_speed;
	// 1/1
	relative_center = vec3(earth.radius+3.5f, earth.radius+3.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);

	//// jupiter
	planet_center = jupiter.center;
	planet_rev_speed = jupiter.rev_speed;
	// 1/4
	relative_center = vec3(jupiter.radius+3.5f, jupiter.radius + 3.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 2/4
	relative_center = vec3(-(jupiter.radius + 3.5f), jupiter.radius + 3.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 3/4
	relative_center = vec3(jupiter.radius + 3.5f, -(jupiter.radius + 3.5f), 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 4/4
	relative_center = vec3(-(jupiter.radius + 3.5f), -(jupiter.radius + 3.5f), 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);

	// saturn
	planet_center = saturn.center;
	planet_rev_speed = saturn.rev_speed;
	// 1/5
	relative_center = vec3(saturn.radius+2.5f, saturn.radius + 2.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 2/5
	relative_center = vec3(saturn.radius - 2.0f, saturn.radius + 4.8f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 3/5
	relative_center = vec3(saturn.radius + 2.5f, -(saturn.radius + 2.5f), 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 4/5
	relative_center = vec3(-(saturn.radius + 2.5f), saturn.radius + 2.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 5/5
	relative_center = vec3(-(saturn.radius + 2.5f), -(saturn.radius + 2.5f), 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);

	// neptune
	planet_center = neptune.center;
	planet_rev_speed = neptune.rev_speed;
	// 1/2
	relative_center = vec3(neptune.radius+ 3.5f, neptune.radius + 3.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);
	// 2/2
	relative_center = vec3(-(neptune.radius + 3.5f), neptune.radius + 3.5f, 0);
	d = { relative_center, 2.0f, 0.0f, 5.0f, 8.0f, planet_center, planet_rev_speed, color_p };
	dwarf_planets.emplace_back(d);


	return dwarf_planets;
}

inline std::vector<ring_t> create_rings(circle_t saturn, circle_t uranus)
{
	float interval, thickness, planet_radius, rotate_speed;
	vec3 planet_center;

	std::vector<ring_t> rings;
	ring_t r;

	//// saturn 
	planet_center = saturn.center;
	planet_radius = saturn.radius;
	
	interval = 2.0f;
	thickness = 2.5f;
	rotate_speed = saturn.rev_speed;
	r = { interval, thickness, planet_center, planet_radius, rotate_speed };
	rings.emplace_back(r);

	//// uranus 
	planet_center = uranus.center;
	planet_radius = uranus.radius;

	interval = 2.0f;
	thickness = 4.0f;
	rotate_speed = uranus.rev_speed;

	r = { interval, thickness, planet_center, planet_radius, rotate_speed };
	rings.emplace_back(r);
	return rings;
}

mat4 get_scale_matrix(float radius) {
	return {
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, radius, 0,
		0, 0, 0, 1
	};
}
mat4 get_rotate_z_matrix(float speed, float theta) {
	float c, s;
	c = cos(speed * theta);
	s = sin(speed * theta);
	return {
		c, -s, 0, 0,
			s, c, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
	};
}
mat4 get_translate_matrix(vec3 center) {
	return {
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, center.z,
		0, 0, 0, 1
	};
}


inline void circle_t::update(float t)
{
	theta = t;

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix = get_scale_matrix(radius);
	mat4 rotation_matrix = get_rotate_z_matrix(rot_speed, theta);
	mat4 revolution_matrix = get_rotate_z_matrix(rev_speed, theta);
	mat4 translate_matrix = get_translate_matrix(center);

	model_matrix = revolution_matrix * translate_matrix * scale_matrix * rotation_matrix;

}

inline void dwarf_t::update(float t) {

	theta = t;

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix = get_scale_matrix(radius);
	mat4 rotation_matrix = get_rotate_z_matrix(rot_speed, theta);
	mat4 revolution_matrix = get_rotate_z_matrix(rev_speed, theta);
	mat4 translate_matrix = get_translate_matrix(relative_center);

	mat4 planet_revolution = get_rotate_z_matrix(planet_rev_speed, theta);
	mat4 planet_translate = get_translate_matrix(planet_center);

	model_matrix = planet_revolution * planet_translate * revolution_matrix * translate_matrix * scale_matrix * rotation_matrix;
}

inline void ring_t::update(float t) {
	
	float scale = planet_radius + interval;
	mat4 scale_matrix = get_scale_matrix(scale);
	mat4 translate_matrix = get_translate_matrix(planet_center);
	mat4 rotate_matrix = get_rotate_z_matrix(rotate_speed, t);
	model_matrix = rotate_matrix * translate_matrix * scale_matrix;


}


#endif
