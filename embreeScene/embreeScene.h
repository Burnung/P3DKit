#pragma once

#include "common/mathIncludes.h"
#include <embree3/rtcore.h>
#include "embreeUtils.h"
#include "geo/comp.h"
BEGIN_P3D_NS

class EmbreeDevice;
class EmbreeGeometry;

class EmbreeScene:public std::enable_shared_from_this<EmbreeScene> {
public:
	explicit EmbreeScene(std::shared_ptr<EmbreeDevice> pDevice);
	~EmbreeScene() = default;
public:
	uint32 addMesh(pComp pmesh,bool addInstance = true);
	void addMesh(pComp tMesh, const std::string& compName);
	void addInstanceMesh(const std::string& instanceName, const std::string geoName);

	void createScene();
	

	void addGeometry(pDataBlock vps_, pDataBlock tl);

	void setInstanceMatrix(Mat4f mat,int instanceId = 0);
	void setGeoTransfrom(const std::string& geoName,Mat4f mat);

	void updateScene();

	void release();

	bool checkIntersect(const Vec3f& starP, const Vec3f& dir, float len);

	bool getHitInfo(const Vec3f& starP, const Vec3f& dir, float len,EmbreeHitInfo& hitInfo);
	RTCHit getRawHitInfo(const Vec3f& starP, const Vec3f& dir, float len);
	void clearScene();

	void getNearestPoint(const Vec3f& srcP);

	void addInstance(int stepCount = 1);

	std::weak_ptr<EmbreeDevice> getPDevice() {
		return pDevice;
	}
	RTCScene getRTCScene() {
		return pScene;
	}
private:
	uint32 addComp(pComp tComp);
	RTCScene addInstanceScenByComp(pComp comp,int quality = RTC_BUILD_QUALITY_HIGH);
private:

	RTCScene pScene = nullptr;

	std::vector<RTCGeometry> sceneInstance;
	std::unordered_map<std::string, RTCScene> instanceScenes;
	std::unordered_map<std::string, RTCGeometry> geos;

	std::weak_ptr<EmbreeDevice> pDevice;
	// 不用这个了
	std::vector<std::shared_ptr<EmbreeGeometry>> pGeometrys;

};

typedef std::shared_ptr<EmbreeScene> pEmbreeScene;

END_P3D_NS

