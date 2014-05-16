#version 430 core
#pragma optimize(off)
struct lightSourceParams {
	vec4 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct materialParams {
	vec3 refAmbient;
	vec3 refDiffuse;
	vec3 refSpecular;
	float Shininess;
};
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(binding=1) uniform matrices {
	mat4 Model;
	mat4 View;
	mat4 Projection;
	mat4 ModelView;
	mat4 ModelViewProjection;
	mat4 Normal;
} Transform;

uniform vec3 CameraPosition;
uniform vec3 vColor;
uniform lightSourceParams light0;
uniform materialParams material;
uniform bool turnLight;
out gl_PerVertex 
{
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};

out FSOutput {
	vec3 SpecularColor;
	vec3 DiffuseColor;
	vec3 AmbientColor;
	vec3 fColor;
} Out;
void getEyeSpace(out vec3 norm, out vec4 position) {
	norm = normalize(mat3(Transform.Normal)*vNormal);
	position = Transform.ModelView*vec4(vPosition,1.0f);
}
void phongModel(const in vec4 position, const in vec3 norm, out vec3 difColor, out vec3 ambColor, out vec3 specColor) {
	vec3 s = normalize(vec3(light0.position - position));
	vec3 v = normalize(-position.xyz);
	vec3 r = reflect(-s,norm);
	ambColor = light0.ambient * material.refAmbient;
	float sDotN = max(dot(s,norm),0.0f);
	difColor = light0.diffuse * material.refDiffuse * sDotN;
	specColor = vec3(0.0f);
	if(sDotN > 0.0f) {
		float rDotv = max(dot(r,v),0.0f);
		specColor = light0.specular * material.refSpecular * pow(rDotv,material.Shininess);
	}
}
void main()
{  
	if(turnLight) {
		vec4 ecPosition;
		vec3 tnorm;
		getEyeSpace(tnorm,ecPosition);
		phongModel(ecPosition,tnorm,Out.DiffuseColor,Out.AmbientColor,Out.SpecularColor);
	}
	else {
		Out.fColor = vColor;
	}
	gl_Position = Transform.ModelViewProjection * vec4(vPosition,1.0);
} 
