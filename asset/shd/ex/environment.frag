#version 330 core

in vec3 ePos,eNormal;
in mat4 V;
uniform samplerCube uSkybox;

out vec4 FragColor;

void main()
{
/*
	vec3 R = reflect(normalize(ePos),normalize(eNormal));
	R = vec3(inverse(V) * vec4(R,0.0));
	FragColor = vec4(texture(uSkybox,R).rgb,1.0);	
*/
	float ratio = 1.00 / 1.52;
	vec3 R = refract(normalize(ePos),normalize(eNormal),ratio);
	R = vec3(inverse(V) * vec4(R,0.0));
	FragColor = vec4(texture(uSkybox,R).rgb,1.0);
	
}

