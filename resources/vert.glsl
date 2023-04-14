#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 MVIT;
attribute vec4 aPos; // In object space

uniform float t;

varying vec3 vPos;
varying vec3 vNor;

void main()
{
	float dx1 = 1;
	float dx2 = -sin(aPos.x + t) * cos(aPos.y);
	float dx3 = -sin(aPos.x + t) * sin(aPos.y);

	float dTheta1 = 0;
	float dTheta2 = (- 1 * (cos(aPos.x + t) + 2)) * sin(aPos.y);
	float dTheta3 = (cos(aPos.x + t) + 2) * cos(aPos.y);

	float a = (dTheta2 * dx3) - (dTheta3 * dx2);
	float b = (dTheta3 * dx1) - (dTheta1 * dx3);
	float c = (dTheta1 * dx2) - (dTheta2 * dx1);
	float magnitude = sqrt((a * a) + (b * b) + (c * c));
			
	vec3 nor = vec3(0, 0, 0);
	vec4 pos = vec4(aPos.xyz, 1.0f);
	
	pos.x = (aPos.x);
	pos.y = ((cos(aPos.x + t) + 2) * cos(aPos.y));
	pos.z = ((cos(aPos.x + t) + 2) * sin(aPos.y));

	nor.x = (a / magnitude);
	nor.y = (b / magnitude);
	nor.z = (c / magnitude);

	gl_Position = P * MV * pos;
	vPos = vec3(MV * pos);
	vNor = normalize(vec3(MVIT * vec4(nor, 0.0))); // Assuming MV contains only translations and rotations
}
