#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
uniform mat4 uP,uV,uM;

out vec3 ePos,eNormal;
out mat4 V;

void main()
{
	eNormal = transpose(inverse(mat3(uV * uM))) * aNormal;
	ePos = vec3(uV * uM * vec4(aPos,1.0));
	gl_Position = uP * vec4(ePos,1.0);
	V = uV;
}

