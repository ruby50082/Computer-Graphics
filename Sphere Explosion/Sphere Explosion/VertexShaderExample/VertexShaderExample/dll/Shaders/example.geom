#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
 
in vec2 texcoord[3];
in vec3 normal[3];
in vec3 FragPos[3];
out vec2 tex;
out vec3 nor;
out vec3 pos;
out mat3 TBN;
uniform vec3 Time;
uniform mat4 Rotation;

mat3 tangent_basis(int i)
{
	vec3 deltaPos1 = FragPos[1] - FragPos[0];
	vec3 deltaPos2 = FragPos[2] - FragPos[0];
	vec2 deltaUV1 = texcoord[1] - texcoord[0];
	vec2 deltaUV2 = texcoord[2] - texcoord[0];
	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	vec3 T = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	vec3 B = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;  
	vec3 N = normal[i];

	return mat3(T, B, N);
}

void main()
{
	vec3 avg=vec3(0,0,0);
	for(int i = 0; i < gl_in.length(); i++){
		avg += normalize(normal[i]);
	}
	
	avg = normalize(avg) * Time.x * Time.x;

	for(int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = (gl_in[i].gl_Position + vec4(avg,0));
		tex = texcoord[i];
		nor = normal[i];
		TBN = tangent_basis(i);
		pos = FragPos[i];
		EmitVertex();
	}
	EndPrimitive();
}