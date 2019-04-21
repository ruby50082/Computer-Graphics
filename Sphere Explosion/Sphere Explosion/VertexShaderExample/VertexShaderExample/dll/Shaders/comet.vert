#version 450

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 nor;
layout(location = 2) in vec2 tex;

out vec2 texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat4 Translation_c;

void main() {
	mat4 model = Translation_c;

	gl_Position =  Projection * ModelView * model * vec4(pos, 1.0);
	texcoord = tex;
}