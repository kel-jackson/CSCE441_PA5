#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"

using namespace std;

class Light {
public:
	glm::vec3 lightColor;
	glm::vec3 lightPos;
	string name;

	Light() {
		lightColor = glm::vec3(1, 1, 1);
		lightPos = glm::vec3(1, 1, 0);
		name = "";
	}

	Light(glm::vec3 color, glm::vec3 pos, string lightName) {
		lightColor = color;
		lightPos = pos;
		name = lightName;
	}
};