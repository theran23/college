#ifdef GL_ES
	#ifndef GL_FRAGMENT_PRECISION_HIGH	// highp may not be defined
		#define highp mediump
	#endif
	precision highp float; // default precision needs to be defined
#endif

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;


// the only output variable
out vec4 fragColor;
uniform sampler2D TEX0, TEX1, TEX2, TEX3;
uniform int mode;

// uniform variables
uniform mat4	view_matrix;
uniform float	shininess;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

vec4 phongphong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = Kd*Id*0.8;					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

void main()
{	

	// light position in the eye space
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0.0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	vec4 iKd = texture( TEX0, tc );	// Kd from image
	

	//fragColor = vec4(normalize(norm), 1.0);
	//fragColor = vec4(tc.xy,0, 1.0);
	// fragColor = texture( TEX0, tc );
	fragColor = phong( l, n, h, iKd );

	if(mode == 0){
		fragColor = texture( TEX0, tc );
	}
	else if(mode == 2){
		fragColor = phongphong( l, n, h, iKd );
		fragColor.a = texture(TEX1, tc).x;
	}
	else if (mode == 3){
		iKd = texture( TEX0, tc );
		vec3 c1 = cross(norm,vec3(0,0,1));
		vec3 c2 = cross(norm,vec3(0,1,0));
		vec3 tangent = normalize(length(c1)>length(c2)?c1:c2);
		vec3 binormal = cross(norm,tangent);
		
		// NORM: normal map
		// normal: world-space normal, tnormal: tangent-space normal
		vec3 tnormal = texture( TEX1, tc ).xyz;
		tnormal = normalize(tnormal-0.5);
		// tbn: rotation matrix to world space from tangent space
		mat3 tbn = mat3( tangent, binormal, norm );
		vec3 world_space_bumped_normal = tbn * tnormal;
		
		fragColor = phong( l, world_space_bumped_normal, h, iKd );
	}
	else{
		fragColor = phong( l, n, h, iKd );
	}
}

