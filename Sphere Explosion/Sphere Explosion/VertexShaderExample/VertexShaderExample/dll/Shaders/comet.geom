#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
in vec2 texcoord[3];
out vec2 tex;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform vec3 Time;
void main()
{
	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		tex = texcoord[i];
		EmitVertex();
	}
	EndPrimitive();
}