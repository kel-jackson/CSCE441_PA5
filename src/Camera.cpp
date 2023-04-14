#define _USE_MATH_DEFINES
#include <cmath> 
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "MatrixStack.h"

using namespace std;

Camera::Camera() :
	aspect(1.0f),
	fovy((float)(45.0 * M_PI / 180.0)),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, -5.0f),
	rfactor(0.01f),
	tfactor(0.001f),
	sfactor(0.005f),
	eye(glm::vec3(0, 0.5, -5)),
	target(eye),
	up(glm::vec3(0, 1, 0)),
	yaw(0),
	pitch(0),
	forward(glm::vec3(sin(yaw), pitch, cos(yaw)))
{
}

Camera::~Camera()
{
}

void Camera::mouseClicked(float x, float y, bool shift, bool ctrl, bool alt)
{
	mousePrev.x = x;
	mousePrev.y = y;
	if(shift) {
		state = Camera::TRANSLATE;
	} else if(ctrl) {
		state = Camera::SCALE;
	} else {
		state = Camera::ROTATE;
	}
}

void Camera::mouseMoved(float x, float y)
{
	glm::vec2 mouseCurr(x, y);
	glm::vec2 dv = mouseCurr - mousePrev;
	if (dv.x < 0) {
		yaw += 0.1;
	}

	else if (dv.x > 0) {
		yaw -= 0.1;
	}

	if (dv.y < 0 && pitch < 1) {
		pitch += 0.1;
	}

	else if (dv.y > 0 && pitch > -1) {
		pitch -= 0.1;
	}
	mousePrev = mouseCurr;
}

void Camera::HUDProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->multMatrix(glm::perspective((float)(45.0 * M_PI / 180.0), aspect, znear, zfar));
}

void Camera::applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->multMatrix(glm::perspective(fovy, aspect, znear, zfar));
}

void Camera::applyViewMatrix(std::shared_ptr<MatrixStack> MV) const
{
	MV->multMatrix(glm::lookAt(eye, target, up));
}
