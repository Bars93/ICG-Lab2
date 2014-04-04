#version 120
attribute vec2 position;
varying vec3 curColor;
void main()
{
	vec4 pos = vec4(position,0.0,1.0);
	gl_FrontColor = gl_Color;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	curColor = vec3(dot(gl_Vertex.xyz,gl_Vertex.yxz));
} 
