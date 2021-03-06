#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
uniform mat4 projection,view,model,shadowSpaceMatrix;
out VS_OUT
{
	vec3 fragPos,normal;
	vec2 texCoord;
	vec4 fragPosShadowSpace;
} vs_out;

void main()
{
	vs_out.fragPos = vec3(view * model * vec4(aPos,1.0));
	vs_out.normal = transpose(inverse(mat3(view*model))) * aNormal;
	vs_out.texCoord = aTexCoord * 1000;
	vs_out.fragPosShadowSpace = shadowSpaceMatrix * model * vec4(aPos,1.0);
	gl_Position = projection * view * model * vec4(aPos,1.0);
}
