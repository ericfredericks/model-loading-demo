#version 330 core

struct Material { vec4 diffuse,specular; sampler2D diffuseMap,specularMap; int diffuseMapOn,specularMapOn; float shininess; };
struct DirLight { vec3 direction,ambient,diffuse,specular; };
struct PointLight { vec3 position,ambient,diffuse,specular; float constant,linear,quadratic; };
struct Spotlight { vec3 direction,position,ambient,diffuse,specular; float cutoff,outerCutoff,constant,linear,quadratic; };


uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Spotlight spotlight;
in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;
layout (std140) uniform Matrices { mat4 projection,view; };


out vec4 FragColor;


vec3 CalcDirLight(DirLight,vec3,vec3);
vec3 CalcPointLight(PointLight,vec3,vec3,vec3,mat4);
vec3 CalcSpotlight(Spotlight,vec3,vec3,vec3,mat4);

void main()
{
	vec3 result = vec3(0.0);

	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(-fragPos);

	result += CalcDirLight(dirLight,norm,viewDir);
	result += CalcPointLight(pointLights[0],norm,fragPos,viewDir,view);
	result += CalcPointLight(pointLights[1],norm,fragPos,viewDir,view);
	result += CalcPointLight(pointLights[2],norm,fragPos,viewDir,view);
	result += CalcPointLight(pointLights[3],norm,fragPos,viewDir,view);
	result += CalcSpotlight(spotlight,norm,fragPos,viewDir,view);
	FragColor = vec4(result,1.0);	
}

vec3 CalcSpotlight(Spotlight light,vec3 norm,vec3 fragPos,vec3 viewDir,mat4 V)
{
	vec3 lightDir = normalize(fragPos - vec3(V*vec4(light.position,1.0)));

	// diffuse
	float diff = max(dot(norm,-lightDir),0.0);

	// specular
	vec3 reflectDir = reflect(lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

	// attenuation
	float distance = length(vec3(light.position) - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
			light.quadratic * (distance*distance));

	// combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuseMap,texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap,texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specularMap,texCoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// cutoff 
	float theta = dot(-lightDir,normalize(-light.direction));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon,0.0,1.0);	
	diffuse *= intensity;
	specular *= intensity;
	return (ambient + diffuse + specular);	
}

vec3 CalcDirLight(DirLight light,vec3 norm,vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);

	// diffuse
	float diff = max(dot(norm,-lightDir),0.0);

	// specular
	vec3 reflectDir = reflect(lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

	// combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuseMap,texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap,texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specularMap,texCoord));
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light,vec3 norm,vec3 fragPos,vec3 viewDir,mat4 V)
{
	vec3 lightDir = normalize(fragPos - vec3(V*vec4(light.position,1.0)));

	// diffuse
	float diff = max(dot(norm,-lightDir),0.0);

	// specular
	vec3 reflectDir = reflect(lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

	// attenuation
	float distance = length(vec3(light.position) - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
			light.quadratic * (distance*distance));

	// combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuseMap,texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap,texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specularMap,texCoord));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}


