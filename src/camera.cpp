#include "camera.h"
#include "gl.h"
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/rotate_vector.hpp>
#include <gtc/type_ptr.hpp>
#include "app.h"
#include <math.h>
#include "gldebug.h"
#include <algorithm>
#include "mouse.h"

const float yRestrictionAngle = 3.0f;
const float eulerian = 0.0001f;
camera::Camera camera::main_camera = camera::Camera(camera::CAMERA_MODES[1]);

void camera::Camera::init()
{
	mode.apply(this);
	pos = glm::vec3(0.0, 30.0, 0.0);
	//glEnable(GL_DEPTH_CLAMP);
	cross = glm::cross(lookAt, normal);

	viewMatrix = glm::lookAt(pos, pos - lookAt, normal);
	projectionMatrix = glm::perspective(mode.frustum.fov, mode.frustum.width / mode.frustum.height, mode.frustum.nearPlane, mode.frustum.farPlane);
	infiniteProjectionMatrix = projectionMatrix;
	infiniteProjectionMatrix[2][2] = eulerian - 1.0f;
	infiniteProjectionMatrix[3][2] = mode.frustum.nearPlane*(eulerian - 2.0f);

	orthoProjectionMatrix = glm::ortho(-mode.frustum.width/2.0f, mode.frustum.height / 2.0f, -mode.frustum.width / 2.0f, mode.frustum.height / 2.0f, -100.0f, 100.0f);

}

void camera::Camera::setPosition(glm::vec3 pPos)
{
	pos = pPos;
}
void camera::Camera::translateLocal(float pX, float pY, float pZ)
{
	translateLocal(glm::vec3(pX, pY, pZ));
}
void camera::Camera::translateLocal(glm::vec3 pDir)
{
	glm::vec3 d = 
		*((glm::vec3*)mode.movement.right_off) * mode.movement.right_factor * pDir.x +
		-*((glm::vec3*)mode.movement.forward_off) * mode.movement.forward_factor * pDir.z +
		*((glm::vec3*)mode.movement.upward_off) * mode.movement.upward_factor * pDir.y;

	pos += d*mode.movement.speed * (float)app::timeFactor;
}

void camera::Camera::translateGlobal(glm::vec3 pDir)
{
	pos += pDir;
}

void camera::Camera::look(glm::vec2 pDir)
{
	if (followCursor) {
		
		pDir *= mode.look.sensitivity;
		glm::vec3& y_rotator = *(glm::vec3*)mode.look.y_rotator_off;
		float top_lim = 1.0 - mode.look.axis_limits.y;
		float bot_lim = -1.0 + mode.look.axis_limits.y;
		pDir.y = std::max(pDir.y, pDir.y*(bot_lim < y_rotator.y));//only allow positive pDir.y when y_rotator.y is less than lower y axis limit
		pDir.y = std::min(pDir.y, pDir.y*(top_lim > y_rotator.y));//only allow negative pDir.y when y_rotator.y is greater than upper y axis limit
		y_rotator = glm::rotate(y_rotator, pDir.y*mode.look.y_rotation_factor, *(glm::vec3*)mode.look.y_rotation_axis_off);
		y_rotator = glm::rotate(y_rotator, pDir.x*mode.look.x_rotation_factor, -(*(glm::vec3*)mode.look.x_rotation_axis_off));
	}
}

void camera::Camera::lookAtCenter()
{
	lookAt = mode.frustum.center;
}

void camera::Camera::update()
{


	cross = glm::normalize(glm::cross(lookAt, *(glm::vec3*)mode.look.cross_up_off));
	normal = glm::normalize(glm::cross(cross, lookAt));
	//update camera matrices
	if (mov != glm::vec3()) {
		translateLocal(normalize(mov));
		mov = glm::vec3();
	}

	
	viewMatrix = glm::lookAt(pos, pos + lookAt, normal);
}

void camera::Camera::setMode(CameraMode pMode)
{
	mode = pMode;
	mode.apply(this);
}

void camera::Camera::forward()
{
	move(glm::vec3(0.0f, 0.0f, -1.0f));
}

void camera::Camera::back()
{
	move(glm::vec3(0.0f, 0.0f, 1.0f));
}

void camera::Camera::left()
{
	move(glm::vec3(-1.0f, 0.0f, 0.0f));
}

void camera::Camera::right()
{
	move(glm::vec3(1.0f, 0.0f, 0.0f));
}

void camera::Camera::up()
{
	move(glm::vec3(0.0f, 1.0f, 0.0f));
}

void camera::Camera::down()
{
	move(glm::vec3(0.0f, -1.0f, 0.0f));
}

void camera::Camera::move(glm::vec3 pDir) {
	mov += pDir;
}

void camera::Camera::toggleLook()
{
	followCursor = !followCursor;
}

glm::vec3 & camera::Camera::getPos()
{
	return pos;
}

glm::mat4 & camera::Camera::getProjection()
{
	return projectionMatrix;
}

glm::mat4 & camera::Camera::getView()
{
	return viewMatrix;
}

void camera::forward(Camera & pCam)
{
	pCam.forward();
}

void camera::back(Camera & pCam)
{
	pCam.back();
}

void camera::left(Camera & pCam)
{
	pCam.left();
}

void camera::right(Camera & pCam)
{
	pCam.right();
}

void camera::up(Camera & pCam)
{
	pCam.up();
}

void camera::down(Camera & pCam)
{
	pCam.down();
}

void camera::toggleLook(Camera & pCam)
{
	pCam.toggleLook();
}

void camera::setMode(Camera & pCam, CameraMode pMode)
{
	pCam.setMode(pMode);
}

void camera::cycleModes(Camera & pCam)
{
	pCam.nextMode();
}
void camera::setSpeed(Camera & pCam, float pSpeed)
{
	pCam.setSpeed(pSpeed);
}
void camera::setSensitivity(Camera & pCam, float pSensitivity)
{
	pCam.setSensitivity(pSensitivity);
}
void camera::Camera::nextMode() {
	mode = CAMERA_MODES[(mode.id + 1) % CAMERA_MODE_COUNT];
	mode.apply(this);
}

void camera::Camera::setSpeed(float pNewSpeed)
{
	mode.movement.speed = pNewSpeed;
}

void camera::Camera::setSensitivity(float pNewSensitivity)
{
	mode.look.sensitivity = pNewSensitivity;
}

void camera::CameraMode::apply(Camera* pCam) {
	movement.apply(pCam);
	look.apply(pCam);
	pCam->lookAt = frustum.center;
        pCam->normal = frustum.up;
}
