#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 tex;

out vec3 FragPos;
out vec2 texcoord;
out vec3 normal;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 Rotation1;
uniform mat4 Rotation2;

void main() {
	mat4 model = Rotation2 * Rotation1;
	gl_Position =  Projection * ModelView * model * vec4(pos, 1.0);
	FragPos = vec3(model * vec4(pos, 1.0));
	normal = vec3(model * vec4(nor, 1.0));
	texcoord = tex;
}