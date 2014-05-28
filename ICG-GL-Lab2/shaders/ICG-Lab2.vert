#version 430 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 texCoords;

layout(binding = 1) uniform matrices {
	mat4 Model;
	mat4 View;
	mat4 Projection;
	mat4 ModelView;
	mat4 ModelViewProjection;
	mat4 Normal;
} Transform;

uniform bool turnLight;
out gl_PerVertex 
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out FSOutput {
	vec3 ecNorm;
	vec3 ecPosition;
	vec2 fTexCoords;
} Out;
void getEyeSpace(out vec3 norm, out vec3 position) {
	norm = vec3(Transform.ModelView*vec4(vNormal,0.0f));
	//norm = vNormal;
	position = vec3(Transform.ModelView*vec4(vPosition,1.0f));
}

void main()
{  
	if(turnLight) {
		getEyeSpace(Out.ecNorm,Out.ecPosition);
	}
	Out.fTexCoords = texCoords;
	gl_Position = Transform.ModelViewProjection * vec4(vPosition,1.0);
} 


