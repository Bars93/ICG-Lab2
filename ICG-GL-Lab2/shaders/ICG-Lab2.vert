varying vec3 vecColor;
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	vecColor = vec3(dot(gl_Vertex.xyz,gl_Vertex.zxy));
} 
