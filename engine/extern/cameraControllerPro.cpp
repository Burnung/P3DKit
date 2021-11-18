
#include "common/P3D_Utils.h"
#include "cameraControllerPro.h"

BEGIN_P3D_NS
void CameraControllerPro::processKeyboard(const std::vector<KEY_STATES>&keyStates) {
	if (mCamera == nullptr || mCamera->getType() != Camera::CAMERA_TYPE_PRO)
		return;

	float32 xStep = 0.0;
	float32 zStep = 0.0;

	bool posChange = false;

	if (keyStates[KEY_NAME_A] == KEY_STATE_PRESS) {
		xStep += moveSpeed + extraSpeed;
		posChange = true;
	}
	if (keyStates[KEY_NAME_D] == KEY_STATE_PRESS) {
		xStep -= moveSpeed + extraSpeed;
		posChange = true;
	}
	if (keyStates[KEY_NAME_W] == KEY_STATE_PRESS) {
		zStep += moveSpeed + extraSpeed;
		posChange = true;
	}
	if (keyStates[KEY_NAME_S] == KEY_STATE_PRESS) {
		posChange = true;
		zStep -= moveSpeed + extraSpeed;
	}
	if (keyStates[KEY_NAME_SPACE] == KEY_STATE_PRESS) {
		if (spaceRelease) {
			mCamera->print();
			spaceRelease = false;
		}
	}
	else
		spaceRelease = true;

	if (!posChange)
		return;

	mCamera->move(xStep, zStep);

}

void CameraControllerPro::processMouse(const std::vector<MOUSE_BUTTON_STATES>&keyStates, Vec2f mousePos) {
	if (mCamera == nullptr || mCamera->getType() != Camera::CAMERA_TYPE_PRO)
		return;
	if (keyStates[MOUSE_BUTTON_LEFT] == MOUSE_STATE_PRESS) {
		if (mouseStateLast == MOUSE_STATE_PRESS) {
			auto posDelta = (mousePos - mousePositionLast)*rotSpeed;
			if(posDelta.length() > 0)
				mCamera->rotate(-posDelta.y, posDelta.x);
		 }
	}
	mouseStateLast = keyStates[MOUSE_BUTTON_LEFT];
	mousePositionLast = mousePos;
}




END_P3D_NS