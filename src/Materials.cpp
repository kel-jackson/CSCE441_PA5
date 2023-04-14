#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Light.cpp"

using namespace std;

class Materials {
public:
	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float s;

	Materials() {
		ka = glm::vec3(0.0, 0.0, 0.0);
		kd = glm::vec3(1.0, 1.0, 1.0);
		ks = glm::vec3(1.0, 1.0, 1.0);
		s = 200;
	}

	Materials(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shine) {
		ka = ambient;
		kd = diffuse;
		ks = specular;
		s = shine;
	}

	void loadProgram(shared_ptr<Program>& prog) {
		prog->setVerbose(true);
		prog->init();
		prog->addAttribute("aPos");
		prog->addAttribute("aNor");
		prog->addUniform("MV");
		prog->addUniform("MVIT");
		prog->addUniform("P");
		prog->addUniform("lightColors");
		prog->addUniform("lightPositions");
		prog->addUniform("ka");
		prog->addUniform("kd");
		prog->addUniform("ks");
		prog->addUniform("s");
		prog->setVerbose(false);
	}
};