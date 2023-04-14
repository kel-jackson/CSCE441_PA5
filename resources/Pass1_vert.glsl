#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVIT;

attribute vec4 aPos;
attribute vec3 aNor;

varying vec3 vPos;
varying vec3 vNor;

void main()
{
	gl_Position = P * MV * aPos;
	vPos = vec3(MV * aPos);
	vNor = normalize(vec3(MVIT * vec4(aNor, 0.0)));
}
