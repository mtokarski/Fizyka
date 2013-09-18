#version 120

varying vec3 Normal;
varying vec3 LightVector;
varying vec3 EyeVector;
varying vec3 Pos;

void main()
{
	vec4 v = gl_Vertex;
	gl_Position = gl_ModelViewProjectionMatrix * v;
	v = gl_ModelViewMatrix * v;
	Pos = v.xyz / v.w;

	LightVector = normalize(/*gl_LightSource[0].position.xyz*/ - v.xyz);
	
	EyeVector = normalize(-v.xyz);
	Normal = gl_NormalMatrix * gl_Normal;

	gl_FrontColor = gl_Color;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
