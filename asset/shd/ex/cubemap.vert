#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 texDir;
uniform mat4 projection;
uniform mat4 view;

void main()
{
	texDir = aPos;
	vec4 pos = projection * view * vec4(aPos,1.f);
	gl_Position = pos.xyww;
}

