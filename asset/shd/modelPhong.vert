#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
uniform mat4 projection,view,model;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

void main()
{
	gl_Position = projection * view * model * vec4(aPos,1.0);
	texCoord = aTexCoord;
	normal = mat3(transpose(inverse(view*model))) * aNormal;
	fragPos = vec3(view * model * vec4(aPos,1.0));
}
