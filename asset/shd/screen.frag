#version 330 core

in vec2 texCoord;
uniform sampler2D screen;
out vec4 FragColor;

void main()
{
	FragColor = texture(screen,texCoord);
	FragColor.rgb *= pow(FragColor.rgb,vec3(1.0/2.2));
}

