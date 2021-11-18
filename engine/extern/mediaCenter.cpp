#include "common/P3D_Utils.h"
#include "Window.h"
#include "mediaCenter.h"
#pragma once
BEGIN_P3D_NS

MediaCenter::MediaCenter() {
	keyStates = std::vector<KEY_STATES>(KEY_NAME_NUM, KEY_STATE_NONE);
	mouseStates = std::vector<MOUSE_BUTTON_STATES>(MOUSE_BUTTON_NUM, MOUSE_STATE_NONE);
}
void MediaCenter::addListener(pListener listener) {
	bool sp = false;
	for (const auto& t : listeners) {
		if (t == listener) {
			sp = true;
			break;
		}
	}
	
	
	if (!sp)
		listeners.push_back(listener);
}

void MediaCenter::update() {
	win->getKeyboardStates(keyStates);
	auto mousePos = win->getMousePosition();
	win->getMouseStates(mouseStates);
	for (auto t : listeners) {
		t->processKeyboard(keyStates);
		t->processMouse(mouseStates,mousePos);
	}
}

bool MediaCenter::stop() {
	if (keyStates[KEY_NAME_ESC] == KEY_STATE_PRESS)
		return true;
	return false;
}
END_P3D_NS