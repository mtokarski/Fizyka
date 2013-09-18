#version 120

void main()
{
	vec4 v = gl_Vertex;

	// calculate position based on time and simple movement:
	float time = gl_MultiTexCoord0.x;
	vec3 startVel = gl_MultiTexCoord0.yzw;
	vec3 gravity = vec3(0.0f, -9.5f, 0.0f);

	v.xyz += startVel * time + 0.5f * gravity * time*time;
	
	gl_Position = gl_ModelViewProjectionMatrix * v;

	gl_FrontColor = gl_Color;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
