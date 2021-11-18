#include "common/P3D_Utils.h"
#include "listener.h"
#include "../Camera.h"
BEGIN_P3D_NS
class CameraControllerOrth :public BaseListener{
public:
	CameraControllerOrth() = default;
	~CameraControllerOrth() = default;

public:
	virtual void processKeyboard(const std::vector<KEY_STATES>&keyStates);
	virtual void processMouse(const std::vector<MOUSE_BUTTON_STATES>&keyStates, Vec2f mousePos);

public:
	void setCamera(pCamera cam) { mCamera = cam; }

public:
	pCamera mCamera;
	MOUSE_BUTTON_STATES mouseStateLast = MOUSE_STATE_NONE;
	Vec2f mousePositionLast;
	float moveSpeed = 0.01;
	float zoomSpeed = 0.01 * 40;
};

typedef std::shared_ptr<BaseListener> pListener;
END_P3D_NS
