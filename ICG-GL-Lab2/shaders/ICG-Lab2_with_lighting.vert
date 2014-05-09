#version 430 core
#pragma optimize(off)
#pragma debug(on)

in vec4 vPosition;
in vec3 vColor;
//in vec3 normalVec;

uniform mat4 MVP;
//uniform mat4 NormalMatrix;
struct lightSource
{
  vec4 position;
  vec4 diffuse;
};
/*lightSource light0 = lightSource(
    vec4(-1.0, 1.0, -1.0, 0.0),
    vec4(1.0, 1.0, 1.0, 1.0)
);*/
 
struct material
{
  vec4 diffuse;
};
uniform bool lightingOn = false;
out vec4 Color;
void main()
{   
	if(lightingOn) {
	//Normal = normalize(NormalMatrix*vec4(normalVec,0.0));
	//Color = vec4(vColor,1.0);              
	}
	else {
		Color = vec4(vColor,1.0);
		gl_Position = MVP * vPosition;
	}
} 
