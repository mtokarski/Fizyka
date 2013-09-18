#version 120


void main()
{
	vec3 normal;
  	normal.xy = (gl_PointCoord.xy * 2.0) - vec2(1.0);
	float mag = dot(normal.xy, normal.xy);

  	if (mag > 1.0f) discard;
  
	normal.z = sqrt(1.0f - mag);
	normal = normalize(normal);

  	float diffuseIntensity = dot(normal, vec3(0.0f, 0.0f, 1.0f));
  
  	gl_FragColor.rgb = gl_Color.rgb * diffuseIntensity;
	gl_FragColor.a = 1.0f;
}
