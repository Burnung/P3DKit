#pragma once
#include "common/P3D_Utils.h"
#include "controllerInofs.h"
#include "common/mathIncludes.h"

BEGIN_P3D_NS
class BaseListener :public P3DObject {
public:
	BaseListener() = default;
	~BaseListener() = default;
public:
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates) {}

	virtual void processMouse(const std::vector<MOUSE_BUTTON_STATES>& keyStates,Vec2f mousePos) {}
};

typedef std::shared_ptr<BaseListener> pListener;
END_P3D_NS
