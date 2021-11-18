#include "embreeScene.h"
#include "common/mathIncludes.h"

#pragma once
BEGIN_P3D_NS

class EmbreeScene;
class EmbreeDevice:public std::enable_shared_from_this<EmbreeDevice> {
public:
	EmbreeDevice() = default;
	~EmbreeDevice();
public:
	void init(const std::string& config);;
	void release();
	RTCDevice getRTDeviceP() { return pRTDevice; }

	std::shared_ptr<EmbreeScene> createScene();
	std::shared_ptr<EmbreeScene> getScene() {
		return pScene;

	}


private:
	static RTCDevice pRTDevice;
	std::shared_ptr<EmbreeScene> pScene;  //目前只保存一个scene

};



END_P3D_NS