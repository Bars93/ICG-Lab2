varying vec3 vecColor;
void main()
{
	const vec3 defColor = vec3(0.125,0.825,0.565);
 	gl_FragColor = vec4(vecColor+defColor,1.0);
}
