#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__


float X_MAX = 1.5f;
float Y_MAX = 1.0f;


// 변수명 바꾸기
float INITIAL_SPEED = 1.0f;

vec2 radndomPos(void) {
	float x = X_MAX * sin(float(rand()));
	float y = Y_MAX * sin(float(rand()));
	return vec2(x, y);
}

float randomRadius(int N) {
	float r = abs(sin((float)rand())) + 0.5f;
	r /= float(sqrt(N) * 2);
	return r;
}

vec4 randomColor(void) {
	return vec4(sin(float(rand())), sin(float(rand())), sin(float(rand())), 1.0f);
}



struct circle_t
{
	vec2	center=vec2(0);		
	float	radius=1.0f;		
	vec4	color;

	vec2 velocity = vec2(0);
	vec2 delta_v = vec2(0);

	bool is_clicked = false;

	mat4	model_matrix;		
	// public functions
	void calculate(std::vector<circle_t>& circles, float delta_t);
	void	update( float delta_t );
};

inline std::vector<circle_t> create_circles(int N){
	std::vector<circle_t> circles;
	circle_t c;
	
	float x, y, r;
	bool collide_c, collide_w;
	for (int i = 0; i < N; i++) {
		while (1) {
			c = { radndomPos(), randomRadius(N) };
			x = c.center.x; y = c.center.y; r = c.radius;
			if ((x + r >= X_MAX) || (x - r <= -X_MAX) || (y + r >= Y_MAX) || (y - r <= -Y_MAX)) {
				collide_w = true;
				continue;
			}
			collide_w = false;
			collide_c = false;
			for (auto& c0 : circles) {
				if (distance(c0.center, c.center) <= c0.radius + c.radius) {
					collide_c = true;
					break;
				}
			}
			if (!collide_c && !collide_w) {
				break;
			}
		}
		c.color = randomColor();
		c.velocity = INITIAL_SPEED * vec2(sin((float)rand()), sin((float)rand()));

		circles.emplace_back(c);
	}
	return circles;
}

inline void circle_t::calculate(std::vector<circle_t>&circles, float delta_t) {
	
	vec2 prev_pos = center - delta_t * velocity;
	vec2 cur_pos = center;

	delta_v = vec2(0);

	vec2 c0_prev_pos, c0_cur_pos;
	float f1, f2;
	float m, c0_m;
	for (auto& c0 : circles) {
		if (c0.center == center) {
			continue;
		}

		c0_prev_pos = c0.center - delta_t * c0.velocity;
		c0_cur_pos = c0.center;

		f1 = radius + c0.radius - distance(prev_pos, c0_prev_pos);
		f2 = radius + c0.radius - distance(cur_pos, c0_cur_pos);

		//if (f2 > 0 && f1 < f2) {
		if (f2 > 0 && f1 < f2){

			m = radius * radius;
			c0_m = c0.radius * c0.radius;

			if (c0.is_clicked == true) m = 0;

			
			vec2 v_diff = velocity - c0.velocity;
			vec2 p_diff = cur_pos - c0_cur_pos;
			delta_v -= ((c0_m + c0_m) / (m + c0_m)) * (dot(v_diff, p_diff) / dot(p_diff, p_diff)) * p_diff;
		
		}

		if (is_clicked == true) delta_v = vec2(0);
	}

	bool left_wall = false, right_wall = false, top_wall = false, bottom_wall = false;
	f1 = prev_pos.x + radius - X_MAX;
	f2 = cur_pos.x + radius - X_MAX;
	if (f2 > 0 && f1 < f2) { right_wall = true; }
	f1 = -X_MAX - prev_pos.x + radius;
	f2 = -X_MAX - cur_pos.x + radius;
	if (f2 > 0 && f1 < f2) { left_wall = true; }
	f1 = prev_pos.y + radius - Y_MAX;
	f2 = cur_pos.y + radius - Y_MAX;
	if (f2 > 0 && f1 < f2) { top_wall = true; }
	f1 = -Y_MAX - prev_pos.y + radius;
	f2 = -Y_MAX - cur_pos.y + radius;
	if (f2 > 0 && f1 < f2) { bottom_wall = true; }

	if (left_wall && right_wall) { delta_v.x -= velocity.x;}
	else if (left_wall || right_wall) { delta_v.x -= 2*velocity.x; }


	if (top_wall && bottom_wall) { delta_v.y -= velocity.y; }
	else if (top_wall || bottom_wall) { delta_v.y -= 2*velocity.y; }



}

inline void circle_t::update( float delta_t )
{
	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	center += delta_t * velocity;

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	
	model_matrix = translate_matrix*scale_matrix;
}

#endif
