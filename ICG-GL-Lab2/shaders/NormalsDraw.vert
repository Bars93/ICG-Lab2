#version 430 core
#pragma optimize(off)
#pragma debug(on)
#extension GL_ARB_explicit_attrib_location : require
layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vertNormal;

out VertData {
	vec3 normal;
} vDataOut;
out gl_PerVertex {
	vec4 gl_Position;
	float gl_PointSize;
	float gl_ClipDistance[];
};
void main()
{  
	vDataOut.normal = vertNormal;
	gl_Position = vec4(vPosition,1.0);
} 