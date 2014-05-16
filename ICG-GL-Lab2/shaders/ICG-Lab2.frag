#version 430 core
#pragma optimize(off)
uniform bool turnLight;
in FSOutput {
	vec3 SpecularColor;
	vec3 DiffuseColor;
	vec3 AmbientColor;
	vec3 fColor;
} In;

out vec4 fragColor;          
void main()
{
	if(turnLight) {
	vec3 phongColor = clamp(In.AmbientColor + In.DiffuseColor + In.SpecularColor,0.0,1.0);
	fragColor = vec4(phongColor,1.0f);
	}
	else {
		fragColor = vec4(In.fColor,1.0f);
	}
}
