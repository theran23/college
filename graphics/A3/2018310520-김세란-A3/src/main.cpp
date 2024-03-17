#include "cgmath.h"		// slee's simple math library
#include "cgut.h"		// slee's OpenGL utility
#include "trackball.h"	// virtual trackball
#include "circle.h"

//*************************************
// global constants
static const char*	window_name = "cgbase - trackball";
static const char*	vert_shader_path = "shaders/trackball.vert";
static const char*	frag_shader_path = "shaders/trackball.frag";


static const uint LAT = 36;
static const uint LNG = 36 * 2;;

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

//*************************************
// window objects
GLFWwindow*	window = nullptr;
ivec2		window_size = cg_default_window_size(); // initial window size

//*************************************
// OpenGL objects
GLuint program	= 0;	// ID holder for GPU program
GLuint vertex_array = 0; // ID holder for vertex array object

//*************************************
// global variables
int		frame = 0;				// index of rendering frames

float	t = 0.0f;						// current simulation parameter

bool b_rotation = true;
bool r = false;
bool	b_wireframe = false;

float last_t, delta_t = 0.0f, r_stop_t = 0.0f, theta = 0.0f;


auto	circles = std::move(create_circles());
bool	b_index_buffer = true;
std::vector<vertex>	unit_sphere_vertices;


//*************************************
// scene objects
camera		cam;
trackball	tb;

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
}

void render()
{
	// clear screen (with background color) and clear depth buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// notify GL that we use our own program
	glUseProgram( program );

	// bind vertex array object
	glBindVertexArray(vertex_array);

	// render two circles: trigger shader program to process vertex data
	for (auto& c : circles)
	{
		// per-circle update
		c.update(theta);

		// update per-circle uniforms
		GLint uloc;
		uloc = glGetUniformLocation(program, "model_matrix");
		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, c.model_matrix);

		// sphere draw calls
		if (b_index_buffer)	glDrawElements(GL_TRIANGLES, (LNG + 1) * LAT * 2 * 3, GL_UNSIGNED_INT, nullptr);
		//else 삭제 
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
	printf( "- press Pause to pause the simulation\n");
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


			indices.push_back(k + 1);	// the origin
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
	if (!vertex_array) { printf("%s(): failed to create vertex aray\n", __func__); return; }
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

		else if (key == GLFW_KEY_PAUSE)
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

bool user_init()
{
	// log hotkeys
	print_help();

	// init GL states
	glClearColor( 39/255.0f, 40/255.0f, 34/255.0f, 1.0f );	// set clear color
	glEnable( GL_CULL_FACE );								// turn on backface culling
	glEnable( GL_DEPTH_TEST );								// turn on depth tests

	// load the mesh
	//pMesh = cg_load_mesh( mesh_vertex_path, mesh_index_path );
	//if(pMesh==nullptr){ printf( "Unable to load mesh\n" ); return false; }

	// define the position of four corner vertices
	unit_sphere_vertices = std::move(create_sphere_vertices(LAT, LNG));
	// create vertex buffer; called again when index buffering mode is toggled
	update_vertex_buffer(unit_sphere_vertices);

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
