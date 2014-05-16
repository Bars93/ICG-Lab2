#version 430 core
#pragma debug(on)

layout(points) in;
layout(line_strip, max_vertices = 2) out;

uniform mat4 MVP;
in gl_PerVertex
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
} gl_in[];
out gl_PerVertex 
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};
in VertData {
	vec3 normal;
} vDataIn[];

void main() {
	int i;
	vec4 inputPoint;
	vec3 normal;
	for(int i = 0; i < gl_in.length(); i++) {
		inputPoint = gl_in[i].gl_Position;
		normal = normalize(vDataIn[i].normal);
		gl_Position = MVP * inputPoint;
		EmitVertex();
		gl_Position = MVP * vec4(inputPoint.xyz + normal * 20.0f,1.0f);
		EmitVertex();
		EndPrimitive();
	}
}