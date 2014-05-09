#version 430 core
#pragma optimize(off)
#pragma debug(on)
layout (location = 0) in vec4 vPosition;
layout (location = 2) in vec3 VertexColor;

uniform mat4 MVP;
out vec4 Color;
void main()
{   
	Color = vec4(VertexColor,1.0);              
	gl_Position = MVP * vPosition;
} 
