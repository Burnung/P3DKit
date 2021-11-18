#pragma once
#include "common/P3D_Utils.h"
#include "listener.h"
#include "../Camera.h"
BEGIN_P3D_NS
class CameraControllerPro :public BaseListener{
public:
	CameraControllerPro() = default;
	~CameraControllerPro() = default;

public:
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates);
	virtual void processMouse(const std::vector<MOUSE_BUTTON_STATES>&keyStates, Vec2f mousePos);

public:
	void setCamera(pCamera cam) { mCamera = cam; }
	

public:
	pCamera mCamera;
	MOUSE_BUTTON_STATES mouseStateLast = MOUSE_STATE_NONE;
	bool spaceRelease = true;
	Vec2f mousePositionLast;
	float moveSpeed = 0.1;
	float extraSpeed = 0.0;
	float rotSpeed = 0.2;
};

typedef std::shared_ptr<BaseListener> pListener;
END_P3D_NS

