#version 120

uniform vec3 lightColors[10];
uniform vec3 lightPositions[10];
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 vPos;
varying vec3 vNor;

void main()
{
	vec3 n = normalize(vNor);
	vec3 e = normalize(vec3(0, 0, 0) - vPos);
	vec3 finalColor = vec3(0, 0, 0);

	for (int i = 0; i < 10; i++) {
		vec3 l = normalize(lightPositions[i] - vPos);
		vec3 h = normalize(e + l);
		vec3 color = ka;
		float dotProd = dot(l, n);
		float exponent = pow(dot(h, n), s);

		if (dotProd > 0) {
			color += (kd * dotProd);
		}

		if (exponent > 0) {
			color += (ks * exponent);
		}

		float dist = sqrt(
			((lightPositions[i][0] - vPos[0]) * (lightPositions[i][0] - vPos[0])) +
			((lightPositions[i][1] - vPos[1]) * (lightPositions[i][1] - vPos[1])) +
			((lightPositions[i][2] - vPos[2]) * (lightPositions[i][2] - vPos[2]))		
		);

		float attenuation = 1.0 / (1.0 + (0.0429 * dist) + (0.9857 * dist * dist));

		finalColor.r += lightColors[i][0] * color.r * attenuation;
		finalColor.g += lightColors[i][1] * color.g * attenuation;
		finalColor.b += lightColors[i][2] * color.b * attenuation;
	}
	
	gl_FragColor = vec4(finalColor.r, finalColor.g, finalColor.b, 1.0);
}
