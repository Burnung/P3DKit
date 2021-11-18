#include "embreeDevice.h"
#include "embreeScene.h"
#include "embreeUtils.h"
#include <embree3/rtcore.h>

BEGIN_P3D_NS
void EmbreeDevice::init(const std::string& config) {
	if (pRTDevice)
		return;
	pRTDevice = rtcNewDevice(config.c_str());
	EmbreeUtils::checkForError(pRTDevice, "create device");
	if (pRTDevice)
		rtcSetDeviceErrorFunction(pRTDevice, EmbreeUtils::rtErrorCallbackFunc, nullptr);
}
EmbreeDevice::~EmbreeDevice() {
	this->release();
}
void EmbreeDevice::release() {
	if (pScene)
		pScene->release();
	if (pRTDevice)
		rtcReleaseDevice(pRTDevice);
	pRTDevice = nullptr;
}

std::shared_ptr<EmbreeScene> EmbreeDevice::createScene() {
	pScene = std::make_shared<EmbreeScene>(shared_from_this());
	return pScene;
}
RTCDevice EmbreeDevice::pRTDevice = nullptr;
END_P3D_NS