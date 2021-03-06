#version 330 core


struct Material { vec4 diffuse,specular; sampler2D diffuseMap,specularMap; int diffuseMapOn,specularMapOn; float shininess; };
struct DirLight { vec3 direction,ambient,diffuse,specular; };
struct PointLight { vec3 position,ambient,diffuse,specular; float constant,linear,quadratic; };
struct Spotlight { vec3 direction,position,ambient,diffuse,specular; float cutoff,outerCutoff,constant,linear,quadratic; };


uniform Material material;
uniform DirLight dirLight;
uniform mat4 view;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;


out vec4 FragColor;


vec3 CalcDirLight(DirLight,vec3,vec3);
vec3 CalcPointLight(PointLight,vec3,vec3,vec3,mat4);
vec3 CalcSpotlight(Spotlight,vec3,vec3,vec3,mat4);
vec3 CalcPhongSpecular(vec3,vec3,vec3,vec3);
vec3 CalcBlinnSpecular(vec3,vec3,vec3,vec3);
vec3 CalcAmbient(vec3);
vec3 CalcDiffuse(vec3,vec3,vec3);

void main()
{
	vec3 result = vec3(0.0);
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(-fragPos);
	result += CalcDirLight(dirLight,norm,viewDir);
	FragColor = vec4(result,1.0);
}
vec3 CalcDirLight(DirLight light,vec3 norm,vec3 viewDir)
{
	vec3 lightDir = normalize(light.direction);
	vec3 ambient = CalcAmbient(light.ambient);
	vec3 diffuse = CalcDiffuse(light.diffuse,lightDir,norm);
	vec3 specular = CalcBlinnSpecular(light.specular,lightDir,norm,viewDir);
	return (ambient + diffuse + specular);
}
vec3 CalcPointLight(PointLight light,vec3 norm,vec3 fragPos,vec3 viewDir,mat4 view)
{
	vec3 lightDir = normalize(fragPos - vec3(view*vec4(light.position,1.0)));
	vec3 ambient = CalcAmbient(light.ambient);
	vec3 diffuse = CalcDiffuse(light.diffuse,lightDir,norm);
	vec3 specular = CalcBlinnSpecular(light.specular,lightDir,norm,viewDir);

	// attenuation
	float distance = length(vec3(light.position) - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
			light.quadratic * (distance*distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}



vec3 CalcBlinnSpecular(vec3 color,vec3 lightDir,vec3 norm,vec3 viewDir)
{
	vec3 halfwayDir = normalize(-lightDir+viewDir);
	float spec = pow(max(dot(norm,halfwayDir),0.0),material.shininess);
	return vec3(material.specular);
	return color * spec
		* ((material.specularMapOn * vec3(texture(material.specularMap,texCoord)))
		+ ((1-material.specularMapOn) * vec3(material.specular)));
}
vec3 CalcPhongSpecular(vec3 color,vec3 lightDir,vec3 norm,vec3 viewDir)
{
	vec3 reflectDir = reflect(lightDir,norm);
	float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
	return color * spec
		* ((material.specularMapOn * vec3(texture(material.specularMap,texCoord)))
		+ ((1-material.specularMapOn) * vec3(material.specular)));
}
vec3 CalcDiffuse(vec3 color,vec3 lightDir,vec3 norm)
{
	float diff = max(dot(norm,-lightDir),0.0);
	return color * diff
		* ((material.diffuseMapOn * vec3(texture(material.diffuseMap,texCoord)))
		+ ((1-material.diffuseMapOn) * vec3(material.diffuse)));
}
vec3 CalcAmbient(vec3 color)
{
	return color
		* ((material.diffuseMapOn * vec3(texture(material.diffuseMap,texCoord)))
		+ ((1-material.diffuseMapOn) * vec3(material.diffuse)));
}
