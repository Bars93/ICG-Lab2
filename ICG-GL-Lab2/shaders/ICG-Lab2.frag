#version 430 core

uniform bool turnLight;
uniform sampler2D cobTex;
struct lightSourceParams {
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic;
}
;
struct materialParams {
    vec3 refAmbient;
    vec3 refDiffuse;
    vec3 refSpecular;
    vec3 emission;
    float Shininess;
}
;
in FSOutput {
    vec3 ecNorm;
    vec3 ecPosition;
    vec2 fTexCoords;
}
 In;
uniform lightSourceParams light0;
uniform materialParams material;
uniform bool textureOn = false;
out vec4 fragColor;
float distFunc(const in float d) {
    // distantion attenuation
    return 1.0f/(light0.attenuation_constant + 
	d * light0.attenuation_linear + 
	d * d * light0.attenuation_quadratic);
}



vec3 phongModel(const in vec3 surfPos, const in vec3 Normal) {
    vec3 N = normalize(Normal);
    vec3 L = vec3(0.0f);
    float distCoef = 1.0f;
    if(abs(light0.position.w) <= 1e-7) {
        L = normalize(light0.position.xyz);
    }

	else {
        L = normalize(light0.position.xyz - surfPos);
        distCoef = distFunc(length(L)); // get distance attenuation
    }

	vec3 V = normalize(-surfPos);
    vec3 H = normalize(V + L);
    // halfway vector
	vec4 texColor = texture(cobTex,In.fTexCoords);
    vec3 diffuseColor = light0.diffuse *  material.refDiffuse * max(dot(N,L),0.0f);
    if(textureOn) {
        diffuseColor *= texColor.rgb;
    }
	return material.emission + // emission
	light0.ambient * material.refAmbient + // calculation ambient light 
	distCoef*(diffuseColor + // calculation diffuse light 
	light0.specular * material.refSpecular * pow(max(dot(H,N),0.0f),material.Shininess));
    // calculation specular light
}


void main() {
    if(turnLight) {
        fragColor = vec4(phongModel(In.ecPosition,In.ecNorm),1.0f);
    }

	else {
	   vec3 texColor = vec3(0.0f);
	   if(textureOn) {
			texColor = texture(cobTex,In.fTexCoords).rgb;
	   }
	   else {
			texColor = material.refDiffuse;
	   }
       fragColor = vec4(texColor,1.0f);
    }
}