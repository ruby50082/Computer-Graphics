#version 450

layout(binding = 3) uniform sampler2D Texture4;

in vec2 tex;
out vec4 outColor;

void main(){

	outColor = texture2D(Texture4, tex);
}