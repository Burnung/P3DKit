#include "common/P3D_Utils.h"
#include "../Window.h"
#include "listener.h"
#include "controllerInofs.h"

#pragma once
BEGIN_P3D_NS

class MediaCenter : public P3DObject {
public:
	MediaCenter();
	~MediaCenter() {};
	void setWindow(pWindow win_) { win = win_; }
	void update();
	void addListener(pListener listener);
	bool stop();
public:
	pWindow win;
	std::vector<pListener> listeners;
	std::vector<KEY_STATES> keyStates;
	std::vector<MOUSE_BUTTON_STATES> mouseStates;

};

END_P3D_NS