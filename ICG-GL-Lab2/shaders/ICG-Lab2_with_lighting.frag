#version 430 core
#pragma optimize(off)
#pragma debug(on)
in vec4 Color;
//in vec4 Normal;
out vec4 fColor;          
struct taglightParams {
	vec4 position;
	vec4 diffuse;
	vec4 LightDirection;
	vec4 HalfVector;
	float Shininess;
	float Strength;
};
void main()
{
 	fColor = Color;
}
