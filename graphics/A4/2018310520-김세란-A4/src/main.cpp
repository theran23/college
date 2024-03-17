#include "cgmath.h"		// slee's simple math library
#define STB_IMAGE_IMPLEMENTATION

#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "circle.h"
//*************************************
// global constants
static const char*	window_name = "full system";
static const char*	vert_shader_path = "shaders/trackball.vert";
static const char*	frag_shader_path = "shaders/trackball.frag";
static const char* earth_path = "textures/earth.jpg";
static const char* earth_bump_path = "textures/earth-bump.jpg";
static const char* earth_normal_path = "textures/earth-normal.jpg";
static const char* jupiter_path = "textures/jupiter.jpg";
static const char* mars_path = "textures/mars.jpg";
static const char* mars_bump_path = "textures/mars-bump.jpg";
static const char* mars_normal_path = "textures/mars-normal.jpg";
static const char* mercury_path = "textures/mercury.jpg";
static const char* mercury_bump_path = "textures/mercury-bump.jpg";
static const char* mercury_normal_path = "textures/mercury-normal.jpg";
static const char* moon_path = "textures/moon.jpg";
static const char* moon_bump_path = "textures/moon-bump.jpg";
static const char* moon_normal_path = "textures/moon-normal.jpg";
static const char* neptune_path = "textures/neptune.jpg";
static const char* saturn_path = "textures/saturn.jpg";
static const char* saturn_ring_path = "textures/saturn-ring.jpg";
static const char* saturn_ring_alpha_path = "textures/saturn-ring-alpha.jpg";
static const char* sun_path = "textures/sun.jpg";
static const char* uranus_path = "textures/uranus.jpg";
static const char* uranus_ring_path = "textures/uranus-ring.jpg";
static const char* uranus_ring_alpha_path = "textures/uranus-ring-alpha.jpg";
static const char* venus_path = "textures/venus.jpg";
static const char* venus_bump_path = "textures/venus-bump.jpg";
static const char* venus_normal_path = "textures/venus-normal.jpg";

static const uint LAT = 36;
static const uint LNG = 36 * 2;;

static const uint NUM_TESS = 256;

enum SolarSystem {   
	sun = 0, 
	mercury,
	venus,
	earth,
	mars,
	jupiter,
	saturn,
	uranus,
	neptune
};

//*************************************
// common structures
struct camera
{
	vec3	eye = vec3( 0, 200,0 );
	vec3	at = vec3( 0, 0, 0 );
	vec3	up = vec3( 0, 0, 1 );
	mat4	view_matrix = mat4::look_at( eye, at, up );
		
	float	fovy = PI/4.0f; // must be in radian
	float	aspect;
	float	dnear = 1.0f;
	float	dfar = 1000.0f;
	mat4	projection_matrix;
};

struct light_t
{	
	// Sun's position 
	vec4	position = vec4(0.0f, 0.0f, 0.0f, 1.0f);   // directional light
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float	shininess = 1000.0f;
};


//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint program	= 0;	// ID holder for GPU program
GLuint vertex_array = 0; // ID holder for vertex array object
GLuint vertex_array2 = 0;

GLuint SUN = 0, MERCURY = 0, VENUS = 0, EARTH = 0, MARS = 0, JUPITER = 0, SATURN = 0, URANUS = 0, NEPTUNE = 0, MOON = 0;
GLuint SATURN_RING = 0, URANUS_RING = 0, SATURN_RING_ALPHA=0, URANUS_RING_ALPHA=0;
GLuint EARTH_NORMAL = 0, EARTH_BUMP = 0, MERCURY_NORMAL = 0, MERCURY_BUMP = 0, MARS_NORMAL = 0, MARS_BUMP = 0, MOON_NORMAL = 0, MOON_BUMP = 0, VENUS_NORMAL = 0, VENUS_BUMP = 0;

//*************************************
// global variables
int		frame = 0;				// index of rendering frames
float	t = 0.0f;						// current simulation parameter

bool b_rotation = true;
bool r = false;
bool	b_wireframe = false;

float last_t, delta_t = 0.0f, r_stop_t = 0.0f, theta = 0.0f;


auto	circles = std::move(create_circles());
auto dwarfs = std::move(create_dwarf_planets(circles[earth], circles[jupiter], circles[uranus], circles[neptune]));
auto rings = std::move(create_rings(circles[saturn], circles[uranus]));
bool	b_index_buffer = true;
std::vector<vertex>	unit_sphere_vertices;
std::vector<vertex>	unit_ring_vertices;


//*************************************
// scene objects
camera		cam;
trackball	tb;
light_t		light;
material_t	material;

//*************************************
void update()
{
	// update global simulation parameter
	delta_t = float(glfwGetTime()) * 0.4f - last_t;
	last_t = float(glfwGetTime()) * 0.4f;
	if (r) { // 이전 update()와 현재 update() 사이에 r 토글 버튼 눌림.
		if (b_rotation) { // rotation 켜짐
			theta += 0;
		}
		else { // rotation 꺼짐
			theta += delta_t;
		}
		r = false;
	}
	else {
		if (b_rotation) { // rotation 계속 켜진 상태
			theta += delta_t;
		}
		else { // rotation 계속 꺼진 상태
			theta += 0;
		}
	}

	// update projection matrix
	cam.aspect = window_size.x/float(window_size.y);
	cam.projection_matrix = mat4::perspective( cam.fovy, cam.aspect, cam.dnear, cam.dfar );

	// build the model matrix for oscillating scale
	float scale	= 1.0f+float(cos(t*1.5f))*0.05f;
	mat4 model_matrix = mat4::scale( scale, scale, scale );

	// update uniform variables in vertex/fragment shaders
	GLint uloc;
	uloc = glGetUniformLocation( program, "view_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.view_matrix );
	uloc = glGetUniformLocation( program, "projection_matrix" );	if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, cam.projection_matrix );
	uloc = glGetUniformLocation( program, "model_matrix" );			if(uloc>-1) glUniformMatrix4fv( uloc, 1, GL_TRUE, model_matrix );

	// setup light properties
	glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
	glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
	glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

	// setup material properties
	glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
	glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
	glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
	glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);

}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// bind program:  notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray(vertex_array);
	// bind textures 1
	glActiveTexture(GL_TEXTURE0);
	int i = 0;
	// render circles: trigger shader program to process vertex data
	for (auto& c : circles)
	{	// bind textures 2
		if (i == sun) {
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SUN);	
			glUniform1i(glGetUniformLocation(program, "mode"), 0);}
		else if(i == mercury) {
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MERCURY);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, MERCURY_NORMAL);

			glUniform1i(glGetUniformLocation(program, "TEX2"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, MERCURY_BUMP);

			glUniform1i(glGetUniformLocation(program, "mode"), 3);}
		else if (i == venus) {
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, VENUS);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, VENUS_NORMAL);

			glUniform1i(glGetUniformLocation(program, "TEX2"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, VENUS_BUMP);

			glUniform1i(glGetUniformLocation(program, "mode"), 3);}
		else if (i == earth) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, EARTH);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, EARTH_NORMAL);

			glUniform1i(glGetUniformLocation(program, "TEX2"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, EARTH_BUMP);

			glUniform1i(glGetUniformLocation(program, "mode"), 3);}
		else if (i == mars) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MARS);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, MARS_NORMAL);

			glUniform1i(glGetUniformLocation(program, "TEX2"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, MARS_BUMP);

			glUniform1i(glGetUniformLocation(program, "mode"), 3);}
		else if (i == jupiter) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, JUPITER); 

			glUniform1i(glGetUniformLocation(program, "mode"), 1);}
		else if (i == saturn) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SATURN); 

			glUniform1i(glGetUniformLocation(program, "mode"), 1);}
		else if (i == uranus) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, URANUS); 
			
			glUniform1i(glGetUniformLocation(program, "mode"), 1);}
		else if (i == neptune) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, NEPTUNE); 
			
			glUniform1i(glGetUniformLocation(program, "mode"), 1);}
		i += 1;

		c.update(theta);
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);
		// sphere draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, (LNG + 1) * LAT * 2 * 3, GL_UNSIGNED_INT, nullptr);

	}

	// render dwarf planets: trigger shader program to process vertex data
	for (auto& d : dwarfs)
	{	
		// bind textures 2
			
		glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, MOON);

		glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, MOON_NORMAL);

		glUniform1i(glGetUniformLocation(program, "TEX2"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, MOON_BUMP);
		glUniform1i(glGetUniformLocation(program, "mode"), 3);
		// per-circle update
		d.update(theta);
		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, d.model_matrix);
		// sphere draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, (LNG + 1) * LAT * 2 * 3, GL_UNSIGNED_INT, nullptr);
	}

	/// rings
	// bind vertex array object	

	glBindVertexArray(vertex_array2);
	
	// render rings:
	i = 0;
	for (auto& r: rings) {
		
		// bind textures 2
		if (i == 0) { 

			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, SATURN_RING);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, SATURN_RING_ALPHA);

		glUniform1i(glGetUniformLocation(program, "mode"), 2); }
		else if (i == 1) { 
			glUniform1i(glGetUniformLocation(program, "TEX0"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, URANUS_RING);

			glUniform1i(glGetUniformLocation(program, "TEX1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, URANUS_RING_ALPHA);

			glUniform1i(glGetUniformLocation(program, "mode"), 2); }
		i += 1;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// per-ring update
		r.update(theta);
		// update per-ring uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, r.model_matrix);
		// per-rings draw calls
		if (b_index_buffer) {
			// glDrawElements(GL_TRIANGLES, 1536*2  , GL_UNSIGNED_INT, nullptr);
			glDrawElements(GL_TRIANGLES, NUM_TESS*3*2*2*2, GL_UNSIGNED_INT, nullptr);

		}
	}

	// swap front and back buffers, and display to screen
	glfwSwapBuffers( window );
}

void reshape( GLFWwindow* window, int width, int height )
{
	// set current viewport in pixels (win_x, win_y, win_width, win_height)
	// viewport: the window area that are affected by rendering 
	window_size = ivec2(width,height);
	glViewport( 0, 0, width, height );
}

void print_help()
{
	printf( "[help]\n" );
	printf( "- press ESC or 'q' to terminate the program\n" );
	printf( "- press F1 or 'h' to see help\n" );
	printf( "- press 'w' to toggle wireframe\n");
	printf( "- press Home to reset camera\n" );
	printf( "- press Space (or Pause) to pause the simulation\n");
	printf( "\n" );
}

std::vector<vertex> create_sphere_vertices(int lati, int longi)
{
	std::vector<vertex> v = {}; // origin
	for (int k = 0; k <= lati; k++)
	{
		float theta = PI * 1.0f * (k / float(lati));
		for (int l = 0; l <= longi; l++) {
			float pi = PI * 2.0f * (l / float(longi));
			// 1.0f: radius of sphere
			vec3 P = 1.0f * vec3(sin(theta) * cos(pi), sin(theta) * sin(pi), cos(theta));
			vec3 Q = P;
			vec2 R = vec2(pi / (2 * PI), 1 - theta / PI);
			v.push_back({ P,Q,R });
		}
	}
	return v;
}

std::vector<vertex> create_ring_vertices(uint N) {
	std::vector<vertex> v = {}; // origin
	for (uint k = 0; k <= N; k++) {
		float theta = PI * 2.0f * (k / float(N)), c = cos(theta), s = sin(theta);
		v.push_back({ 1.0f * vec3(c,s,0), vec3(0,0,1.0f), vec2(0.0f,0.0f) });
	}
	for (uint k = 0; k <= N; k++) {
		float theta = PI * 2.0f * (k / float(N)), c = cos(theta), s = sin(theta);
		v.push_back({ 2.0f * vec3(c,s,0), vec3(0,0,1.0f), vec2(1.0f,1.0f) });
	}
	return v;
}

void update_vertex_buffer(const std::vector<vertex>& vertices)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }


	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;

		uint N = (LNG + 1) * LAT;
		for (uint k = 0; k <= N; k++)
		{
			indices.push_back(k);
			indices.push_back(k + LNG + 1);
			indices.push_back(k + 1);


			indices.push_back(k + 1);
			indices.push_back(k + LNG + 1);
			indices.push_back(k + LNG + 2);
		}
		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	}
	// else 삭제

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array) glDeleteVertexArrays(1, &vertex_array);
	vertex_array = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array) { printf("%s(): failed to create vertex array\n", __func__); return; }
}

void update_ring_vertex_buffer(const std::vector<vertex>& vertices, uint N)
{
	static GLuint vertex_buffer = 0;	// ID holder for vertex buffer
	static GLuint index_buffer = 0;		// ID holder for index buffer

	// clear and create new buffers
	if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
	if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }


	// create buffers
	if (b_index_buffer)
	{
		std::vector<uint> indices;

		for (uint k = 0; k < N; k++) {
			indices.push_back(k+N+2);
			indices.push_back(k+N+1);
			indices.push_back(k);

			indices.push_back(k + N + 2);
			indices.push_back(k + N + 1);
			indices.push_back(k + 1);

			indices.push_back(k);
			indices.push_back(k + 1);
			indices.push_back(k + N + 1);

			indices.push_back(k);
			indices.push_back(k + 1);
			indices.push_back(k + N + 2);

			indices.push_back(k + N + 1);
			indices.push_back(k + N + 2);
			indices.push_back(k);

			indices.push_back(k + N + 1);
			indices.push_back(k + N + 2);
			indices.push_back(k);

			indices.push_back(k + 1);
			indices.push_back(k );
			indices.push_back(k + N + 1);

			indices.push_back(k + 1);
			indices.push_back(k);
			indices.push_back(k + N + 2);


		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(2, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(2, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);


	}
	// else 삭제

	// generate vertex array object, which is mandatory for OpenGL 3.3 and higher
	if (vertex_array2) glDeleteVertexArrays(1, &vertex_array2);
	vertex_array2 = cg_create_vertex_array(vertex_buffer, index_buffer);
	if (!vertex_array2) { printf("%s(): failed to create vertex array\n", __func__); return; }
}

void keyboard( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	if(action==GLFW_PRESS)
	{
		if(key==GLFW_KEY_ESCAPE||key==GLFW_KEY_Q)	glfwSetWindowShouldClose( window, GL_TRUE );
		else if(key==GLFW_KEY_H||key==GLFW_KEY_F1)	print_help();
		else if(key==GLFW_KEY_HOME)					cam=camera();
#ifndef GL_ES_VERSION_2_0
		else if (key == GLFW_KEY_W)
		{
			b_wireframe = !b_wireframe;
			glPolygonMode(GL_FRONT_AND_BACK, b_wireframe ? GL_LINE : GL_FILL);
			printf("> using %s mode\n", b_wireframe ? "wireframe" : "solid");
		}
#endif

		else if (key == GLFW_KEY_PAUSE || key == GLFW_KEY_SPACE)
		{
			b_rotation = !b_rotation;
			r = true;
		}
	}
}

void mouse( GLFWwindow* window, int button, int action, int mods )
{
	if(button==GLFW_MOUSE_BUTTON_LEFT)
	{
		dvec2 pos; glfwGetCursorPos(window,&pos.x,&pos.y);
		vec2 npos = cursor_to_ndc( pos, window_size );
		if(action==GLFW_PRESS)			tb.begin( cam.view_matrix, npos );
		else if(action==GLFW_RELEASE)	tb.end();
	}


	if (button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_SHIFT))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_zooming(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_zooming();
	}

	if (button == GLFW_MOUSE_BUTTON_MIDDLE || button == GLFW_MOUSE_BUTTON_LEFT && (mods & GLFW_MOD_CONTROL))
	{
		dvec2 pos; glfwGetCursorPos(window, &pos.x, &pos.y);
		vec2 npos = cursor_to_ndc(pos, window_size);
		if (action == GLFW_PRESS)			tb.begin_panning(cam.view_matrix, npos);
		else if (action == GLFW_RELEASE)	tb.end_panning();
	}
}

void motion( GLFWwindow* window, double x, double y )
{
	if (!tb.is_tracking() && !tb.is_zooming() && !tb.is_panning()) return;
	vec2 npos = cursor_to_ndc( dvec2(x,y), window_size );
	if (tb.is_tracking())cam.view_matrix = tb.update(npos);
	if (tb.is_zooming())cam.view_matrix = tb.update_zooming(npos);
	if (tb.is_panning())cam.view_matrix = tb.update_panning(npos);
}

// this function will be avaialble as cg_create_texture() in other samples
GLuint create_texture( const char* image_path, bool mipmap=true, GLenum wrap=GL_CLAMP_TO_EDGE, GLenum filter=GL_LINEAR )
{
	// load image
	image*	i = cg_load_image( image_path ); if(!i) return 0; // return null texture; 0 is reserved as a null texture
	int		w=i->width, h=i->height, c=i->channels;

	// induce internal format and format from image
	GLint	internal_format = c==1?GL_R8:c==2?GL_RG8:c==3?GL_RGB8:GL_RGBA8;
	GLenum	format = c==1?GL_RED:c==2?GL_RG:c==3?GL_RGB:GL_RGBA;

	// create a src texture (lena texture)
	GLuint texture;
	glGenTextures( 1, &texture ); if(texture==0){ printf("%s(): failed in glGenTextures()\n", __func__ ); return 0; }
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexImage2D( GL_TEXTURE_2D, 0, internal_format, w, h, 0, format, GL_UNSIGNED_BYTE, i->ptr );
	if(i){ delete i; i=nullptr; } // release image

	// build mipmap
	if( mipmap )
	{
		int mip_levels=0; for( int k=w>h?w:h; k; k>>=1 ) mip_levels++;
		for( int l=1; l < mip_levels; l++ )
			glTexImage2D( GL_TEXTURE_2D, l, internal_format, (w>>l)==0?1:(w>>l), (h>>l)==0?1:(h>>l), 0, format, GL_UNSIGNED_BYTE, nullptr );
		if(glGenerateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
	}

	// set up texture parameters
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, !mipmap?filter:filter==GL_LINEAR?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST_MIPMAP_NEAREST );

	return texture;
}

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests
	glEnable(GL_TEXTURE_2D);			// enable texturing
	glActiveTexture(GL_TEXTURE0);		// notify GL the current texture slot is 0


	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices(LAT, LNG));
	unit_ring_vertices = std::move(create_ring_vertices(NUM_TESS));
	// create vertex buffer; called again when index buffering mode is toggled

	update_vertex_buffer(unit_sphere_vertices);	
	update_ring_vertex_buffer(unit_ring_vertices, NUM_TESS);

	// load the image to a texture
	SUN = create_texture(sun_path, true); if (!SUN) return false;
	MERCURY = create_texture(mercury_path, true); if (!MERCURY) return false;
	VENUS = create_texture(venus_path, true); if (!VENUS) return false;
	EARTH = create_texture(earth_path, true); if (!EARTH) return false;
	MARS = create_texture(mars_path, true); if (!MARS) return false;
	JUPITER = create_texture(jupiter_path, true); if (!JUPITER) return false;
	SATURN = create_texture(saturn_path, true); if (!SATURN) return false;
	URANUS = create_texture(uranus_path, true); if (!URANUS) return false;
	NEPTUNE = create_texture(neptune_path, true); if (!NEPTUNE) return false;

	MOON = create_texture(moon_path, true); if (!MOON) return false;

	SATURN_RING = create_texture(saturn_ring_path, true); if (!SATURN_RING) return false;
	URANUS_RING = create_texture(uranus_ring_path, true); if (!URANUS_RING) return false;

	SATURN_RING_ALPHA = create_texture(saturn_ring_alpha_path, true); if (!SATURN_RING_ALPHA) return false;
	URANUS_RING_ALPHA = create_texture(uranus_ring_alpha_path, true); if (!URANUS_RING_ALPHA) return false;

	EARTH_NORMAL = create_texture(earth_normal_path, true); if (!EARTH_NORMAL) return false;
	MERCURY_NORMAL = create_texture(mercury_normal_path, true); if (!MERCURY_NORMAL) return false;
	MARS_NORMAL = create_texture(mars_normal_path, true); if (!MARS_NORMAL) return false;
	MOON_NORMAL = create_texture(moon_normal_path, true); if (!MOON_NORMAL) return false;
	VENUS_NORMAL = create_texture(venus_normal_path, true); if (!VENUS_NORMAL) return false;

	EARTH_BUMP = create_texture(earth_bump_path, true); if (!EARTH_BUMP) return false;
	MERCURY_BUMP = create_texture(mercury_bump_path, true); if (!MERCURY_BUMP) return false;
	MARS_BUMP = create_texture(mars_bump_path, true); if (!MARS_BUMP) return false;
	MOON_BUMP = create_texture(moon_bump_path, true); if (!MOON_BUMP) return false;
	VENUS_BUMP = create_texture(venus_bump_path, true); if (!VENUS_BUMP) return false;




	return true;
}

void user_finalize()
{
}

int main( int argc, char* argv[] )
{
	// create window and initialize OpenGL extensions
	if(!(window = cg_create_window( window_name, window_size.x, window_size.y ))){ glfwTerminate(); return 1; }
	if(!cg_init_extensions( window )){ glfwTerminate(); return 1; }	// version and extensions

	// initializations and validations
	if(!(program=cg_create_program( vert_shader_path, frag_shader_path ))){ glfwTerminate(); return 1; }	// create and compile shaders/program
	if(!user_init()){ printf( "Failed to user_init()\n" ); glfwTerminate(); return 1; }					// user initialization

	// register event callbacks
	glfwSetWindowSizeCallback( window, reshape );	// callback for window resizing events
    glfwSetKeyCallback( window, keyboard );			// callback for keyboard events
	glfwSetMouseButtonCallback( window, mouse );	// callback for mouse click inputs
	glfwSetCursorPosCallback( window, motion );		// callback for mouse movement

	// enters rendering/event loop
	for( frame=0; !glfwWindowShouldClose(window); frame++ )
	{
		glfwPollEvents();	// polling and processing of events
		update();			// per-frame update
		render();			// per-frame render
	}

	// normal termination
	user_finalize();
	cg_destroy_window(window);

	return 0;
}
