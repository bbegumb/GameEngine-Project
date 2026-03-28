#version 330 core

struct DirectionalLight {
	vec3 direction;
	vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};

struct PointLight {
	vec3 position;
	vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

	float constant;
	float linear;
	float quadratic;
};

struct Material {
    vec3 albedo;
    float ambientReflectance;
    float specularReflectance;
    float shininess;
};

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 uViewPos;

uniform bool uHasDirectionalLight;
uniform bool uHasPointLight;

uniform DirectionalLight uDirectionalLight;
uniform PointLight uPointLight;

uniform Material uMaterial;

vec3 calculateDirectionalLight(DirectionalLight light, Material mat, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);

    vec3 ambient = mat.ambientReflectance * light.ambientIntensity * mat.albedo * light.color;
    vec3 diffuse = light.diffuseIntensity * diff * mat.albedo * light.color;
    vec3 specular = mat.specularReflectance * light.specularIntensity * spec * light.color;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), mat.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (
        light.constant +
        light.linear * distance +
        light.quadratic * distance * distance
    );

    vec3 ambient = mat.ambientReflectance * light.ambientIntensity * mat.albedo * light.color;
    vec3 diffuse = light.diffuseIntensity * diff * mat.albedo * light.color;
    vec3 specular = mat.specularReflectance * light.specularIntensity * spec * light.color;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vFragPos);

    vec3 lighting = vec3(0.0);

    if (uHasDirectionalLight)
        lighting += calculateDirectionalLight(uDirectionalLight, uMaterial, normal, viewDir);

    if (uHasPointLight)
        lighting += calculatePointLight(uPointLight, uMaterial, normal, vFragPos, viewDir);

    FragColor = vec4(lighting, 1.0);
}