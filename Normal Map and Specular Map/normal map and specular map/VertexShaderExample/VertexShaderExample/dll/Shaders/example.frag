#version 450

layout(binding = 0) uniform sampler2D Texture1;
layout(binding = 1) uniform sampler2D Texture2;
layout(binding = 2) uniform sampler2D Texture3;

in vec2 tex;
in vec3 nor;
in vec3 pos;
in mat3 TBN;
out vec4 outColor;

uniform mat4 Rotation3;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 turn;

void main(){

	vec3 norm;
	if(turn[1] == 1.0) {
		norm = normalize(texture2D(Texture2, tex).rgb * 2.0 - 1.0);
		norm = normalize(TBN * norm);
	}
	else
		norm = normalize(nor);

	vec3 light = vec3(Rotation3 * vec4(lightPos, 1.0));
	vec3 lightDir = normalize(light - pos);
	vec3 viewDir = normalize(viewPos - pos);
	vec3 reflectDir = reflect(-lightDir, norm);


	vec4 ambient = 0.1 * vec4(0.7, 0.7, 0.7, 1.0);

	vec4 diffuse;
	if(turn[0] == 1.0) 
		diffuse = texture2D(Texture1, tex) * max(dot(norm, lightDir), 0.0);
	else
		diffuse = vec4(0.35, 0.3, 0.15, 1.0) * max(dot(norm, lightDir), 0.0);

	vec4 specular;
	if(turn[2] == 1.0)
		specular = 0.3 * texture2D(Texture3, tex) * pow(max(dot(viewDir, reflectDir), 0.0), 32);
	else
		specular = 0.3 * vec4(1.0, 1.0, 1.0, 1.0) * pow(max(dot(viewDir, reflectDir), 0.0), 32);

	outColor = ambient + diffuse + specular;
}