#include "common/P3D_Utils.h"
#include "cameraControllerOrth.h"

BEGIN_P3D_NS
void CameraControllerOrth::processKeyboard(const std::vector<KEY_STATES>&keyStates) {
	if (mCamera == nullptr || mCamera->getType() != Camera::CAMERA_TYPE_ORTH)
		return;

	float width = mCamera->getOrthWidth();

	if (keyStates[KEY_NAME_W] == KEY_STATE_PRESS)
		width -= zoomSpeed;
	if (keyStates[KEY_NAME_S] == KEY_STATE_PRESS)
		width += zoomSpeed;
	if (keyStates[KEY_NAME_SPACE] == KEY_STATE_PRESS)
		mCamera->getEyePos().print();
	mCamera->setOrthWidth(width);
}

void CameraControllerOrth::processMouse(const std::vector<MOUSE_BUTTON_STATES>&keyStates, Vec2f mousePos) {
	if (mCamera == nullptr || mCamera->getType() != Camera::CAMERA_TYPE_ORTH)
		return;
	if (keyStates[MOUSE_BUTTON_LEFT] == MOUSE_STATE_PRESS) {
		if (mouseStateLast == MOUSE_STATE_PRESS) {
			auto camPos = mCamera->getEyePos();
			camPos.doNormalize();
			auto orthDis = mCamera->getOrthDis();
			
			auto dstPos = mCamera->getDstPos();
			auto upVec = mCamera->getUpDir();
			auto rightVec = mCamera->getRightDir();
			auto posDelta = mousePos - mousePositionLast;
			camPos = camPos + (rightVec * (-posDelta.x) + upVec * posDelta.y) * moveSpeed;
			camPos.doNormalize();
			camPos = camPos * orthDis;
			mCamera->setViewParams(camPos, dstPos, upVec);
		 }
	}
	mouseStateLast = keyStates[MOUSE_BUTTON_LEFT];
	mousePositionLast = mousePos;
}

END_P3D_NS