#version 430 core
#pragma optimize(off)
uniform vec3 vColor;
out vec4 fColor;

void main() {
	fColor = vec4(vColor,1.0);
}