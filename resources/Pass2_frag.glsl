#version 120
    
uniform sampler2D posTexture;
uniform sampler2D norTexture;
uniform sampler2D keTexture;
uniform sampler2D kdTexture;
uniform vec2 windowSize;
    
uniform vec3 lightColors[10];
uniform vec3 lightPositions[10];
    
void main()
{
    vec2 tex;
    tex.x = gl_FragCoord.x/windowSize.x;
    tex.y = gl_FragCoord.y/windowSize.y;
    
    // Fetch shading data
    vec3 pos = texture2D(posTexture, tex).rgb;
    vec3 nor = texture2D(norTexture, tex).rgb;
    vec3 ke = texture2D(keTexture, tex).rgb;
    vec3 kd = texture2D(kdTexture, tex).rgb;

    vec3 n = normalize(nor);
	vec3 e = normalize(vec3(0, 0, 0) - pos);
	vec3 finalColor = vec3(0, 0, 0);

	for (int i = 0; i < 10; i++) {
		vec3 l = normalize(lightPositions[i] - pos);
		vec3 h = normalize(e + l);
		vec3 color = ke;
		float dotProd = dot(l, n);
		float exponent = pow(dot(h, n), 10);

		if (dotProd > 0) {
			color += (kd * dotProd);
		}

		if (exponent > 0) {
			color += (vec3(1, 1, 1) * exponent);
		}

		float dist = sqrt(
			((lightPositions[i][0] - pos.r) * (lightPositions[i][0] - pos.r)) +
			((lightPositions[i][1] - pos.g) * (lightPositions[i][1] - pos.g)) +
			((lightPositions[i][2] - pos.b) * (lightPositions[i][2] - pos.b))		
		);

		float attenuation = 1.0 / (1.0 + (0.0429 * dist) + (0.9857 * dist * dist));

		finalColor.r += lightColors[i][0] * color.r * attenuation;
		finalColor.g += lightColors[i][1] * color.g * attenuation;
		finalColor.b += lightColors[i][2] * color.b * attenuation;
	}
	
	gl_FragColor.rgb = finalColor;
	//gl_FragColor.rgb = pos;
	//gl_FragColor.rgb = nor;
	//gl_FragColor.rgb = kd;
	//gl_FragColor.rgb = ke;
}