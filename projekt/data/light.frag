#version 120

varying vec3 Normal;
varying vec3 LightVector;
varying vec3 EyeVector;
varying vec3 Pos;


void main()
{
	vec3 norm = normalize(Normal);
	vec3 eye = normalize(-Pos);
	
	vec3 ambient = vec3(0.2, 0.2, 0.2);
	vec3 diffuse = vec3(0, 0, 0);
	vec3 specular = vec3(0, 0, 0);
	

	vec3 light = normalize(LightVector);
	vec3 halfVec = normalize(eye + light);
	
	// diffuse
	float diffElem = max(0.0, dot(norm, light));
	diffuse += gl_Color.rgb * diffElem;
	
	// specular
	specular = vec3(1.0, 1.0, 1.0) * pow(max(0.0, dot(norm, halfVec )), 64.0);

	gl_FragColor.rgb = gl_Color.rgb * (ambient + diffuse) + specular;
	gl_FragColor.a = 1.0;
}
