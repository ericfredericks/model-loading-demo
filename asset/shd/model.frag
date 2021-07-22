#version 330 core

in vec2 texCoord;
in vec3 normal;

uniform sampler2D diffuseMap;
uniform int diffuseMapOn;
uniform vec4 diffuse;

out vec4 FragColor;

void main()
{
	FragColor = diffuse*(1-diffuseMapOn)
		+ texture(diffuseMap,texCoord)*diffuseMapOn;
}
