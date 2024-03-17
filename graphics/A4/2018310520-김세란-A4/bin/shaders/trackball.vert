// vertex attributes
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texcoord;


// matrices
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
uniform sampler2D TEX2;
uniform int mode;


// outputs of vertex shader = input to fragment shader
out vec4 epos;	// eye-space position
out vec3 norm;
out vec2 tc;

void main()
{	
	vec4 wpos;
	if(mode == 3){
		vec3 bump = position;
		bump += texture( TEX2, texcoord ).x*0.2;
		wpos = model_matrix * vec4(bump,1);
	}
	else{
		wpos = model_matrix * vec4(position,1);

	}
		epos = view_matrix * wpos;
	gl_Position = projection_matrix * epos;

	// pass eye-coordinate normal to fragment shader
	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	tc = texcoord;

}
