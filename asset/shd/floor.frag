#version 330 core

in VS_OUT
{
	vec3 fragPos,normal;
	vec2 texCoord;
	vec4 fragPosShadowSpace;
} fs_in;
uniform sampler2D diffuseMap;
uniform sampler2D shadowMap;
uniform mat4 view;
uniform vec3 lightDirection;
out vec4 FragColor;


float CalcShadow(vec4,vec3,vec3);
void main()
{
	vec3 color = texture(diffuseMap,fs_in.texCoord).rgb;
	vec3 norm = normalize(fs_in.normal);
	vec3 lightColor = vec3(1.0);
	vec3 lightDir = normalize(lightDirection);
	vec3 viewDir = normalize(-fs_in.fragPos);

	// ambient
	vec3 ambient = 0.4 * lightColor;

	// diffuse
	float diff = max(dot(-lightDir,norm),0.0);
	vec3 diffuse = lightColor * diff;

	// specular
	vec3 halfwayDir = normalize(-lightDir+viewDir);
	float spec = pow(max(dot(norm,halfwayDir),0.0),64.0);
	vec3 specular = spec * lightColor;

	// shadow
	float shadow = CalcShadow(fs_in.fragPosShadowSpace,norm,lightDir);

	vec3 lighting = (ambient + ((1.0-shadow)*(diffuse+specular))) * color;
	FragColor = vec4(lighting,1.0);

}
float CalcShadow(vec4 fragPosShadowSpace,vec3 norm,vec3 lightDir)
{
	vec3 projCoord = fragPosShadowSpace.xyz /  fragPosShadowSpace.w;
	if (projCoord.z > 1.0)
		return 0.0;

	projCoord = projCoord * 0.5 + 0.5;
	float closestDepth = texture(shadowMap,projCoord.xy).r;
	float currentDepth = projCoord.z;
	float bias = max(0.05 * (1.0 - dot(norm,-lightDir)),0.005);

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap,0);
	for (int x=-1;x<=1;++x)
	{
		for (int y=-1;y<=1;++y)
		{
			float pcfDepth = texture(shadowMap,projCoord.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	return shadow / 9.0;
}


