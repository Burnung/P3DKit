#include "embreeScene.h"
#include "embreeDevice.h"
#include <embree3/rtcore.h>
#include <embree3/rtcore.h>
#include "embreeUtils.h"
#include "embreeGeometry.h"
#include "geo/comp.h"
BEGIN_P3D_NS
using namespace std;
EmbreeScene::EmbreeScene(std::shared_ptr<EmbreeDevice> pDevice_) {
	pScene = rtcNewScene(pDevice_->getRTDeviceP());
	pDevice = pDevice_;
	
}

void EmbreeScene::createScene() {
	if (pScene)
		pScene = rtcNewScene(pDevice.lock()->getRTDeviceP());
}
void EmbreeScene::addInstanceMesh(const std::string& instanceName, const std::string geoName) {
	if (instanceScenes.find(instanceName) == instanceScenes.end()) {
		std::cout << "error scene name" << std::endl;
		return;
	}
	auto nGeo = rtcNewGeometry(pDevice.lock()->getRTDeviceP(),RTC_GEOMETRY_TYPE_INSTANCE);
	rtcSetGeometryInstancedScene(nGeo, instanceScenes[instanceName]);
	rtcSetGeometryTimeStepCount(nGeo, 1);
	rtcAttachGeometry(pScene, nGeo);

	//rtcReleaseGeometry(nGeo);
//	rtcCommitScene(pScene);

	geos[geoName] = nGeo;
	
}
void EmbreeScene::addMesh(pComp tMesh, const std::string& compName) {
	auto instanceScene = addInstanceScenByComp(tMesh);
	instanceScenes[compName] = instanceScene;
	addInstanceMesh(compName, compName);
}
uint32 EmbreeScene::addMesh(pComp tComp, bool baddInstance) {
	auto tId = addComp(tComp);
	rtcCommitScene(pScene);
	if (baddInstance)
		addInstance();
	return tId;
}
void EmbreeScene::addGeometry(pDataBlock vps_, pDataBlock tl){
	auto tGeo = std::make_shared<EmbreeGeometry>(shared_from_this());

	tGeo->initByRawData(vps_,tl);
	rtcCommitScene(pScene);
	pGeometrys.push_back(tGeo);
}
void EmbreeScene::updateScene() {
	for (auto tGeo : pGeometrys)
		tGeo->update();
	rtcCommitScene(pScene);
}
void EmbreeScene::clearScene() {
	for (auto tGeo : pGeometrys)
		tGeo->detach();
	pGeometrys.clear();
}
void EmbreeScene::release() {
	if (pScene) {
		rtcReleaseScene(pScene);
		pGeometrys.clear();
	}
	
	pScene = nullptr;
}
RTCScene EmbreeScene::addInstanceScenByComp(pComp tComp,int quality) {
	auto rtGeo = rtcNewGeometry(pDevice.lock()->getRTDeviceP(), RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(rtGeo, RTCBuildQuality(quality));

	float* vps = (float*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), tComp->nVtex);
	if (tComp->cache_vps)
		memcpy(vps, tComp->cache_vps->rawData(), tComp->cache_vps->getSize());
	else
		memcpy(vps, tComp->vps->rawData(), tComp->vps->getSize());

	int* tl = (int*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(int), tComp->nTri);
	memcpy(tl, tComp->tl->rawData(), tComp->tl->getSize());

	rtcCommitGeometry(rtGeo);

	auto tScene = rtcNewScene(pDevice.lock()->getRTDeviceP());

	auto geoId = rtcAttachGeometry(tScene, rtGeo);

	rtcReleaseGeometry(rtGeo);

	rtcCommitScene(tScene);
	return tScene;

}
uint32 EmbreeScene::addComp(pComp tComp) {
	if (!tComp)
		return 0;

	auto tGeo = std::make_shared<EmbreeGeometry>(shared_from_this());

	tGeo->initByComp3D(tComp);

	pGeometrys.push_back(tGeo);
	return tGeo->geoId;
}
bool EmbreeScene::checkIntersect(const Vec3f& starP, const Vec3f& dir, float len) {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	auto rayHit = EmbreeUtils::generatRayHit(starP, dir, len);

	rtcIntersect1(pScene, &context,&rayHit);

	if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID){
		return true;
	}
	return false;
}
RTCHit EmbreeScene::getRawHitInfo(const Vec3f& starP, const Vec3f& dir, float len) {
	RTCIntersectContext contex;
	rtcInitIntersectContext(&contex);
	auto rayHit = EmbreeUtils::generatRayHit(starP, dir, len);

	rtcIntersect1(pScene, &contex, &rayHit);
	return rayHit.hit;
}
bool EmbreeScene::getHitInfo(const Vec3f& starP, const Vec3f& dir, float len,EmbreeHitInfo& hitInfo) {
	RTCIntersectContext contex;
	rtcInitIntersectContext(&contex);
	auto rayHit = EmbreeUtils::generatRayHit(starP, dir, len);

	rtcIntersect1(pScene, &contex, &rayHit);
	//auto hit = getRawHitInfo(starP, dir, len, hitInfo);
	if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
		return false;


	hitInfo.normal = Vec3f(rayHit.hit.Ng_x,rayHit.hit.Ng_y,rayHit.hit.Ng_z).normalize();
	hitInfo.pos = starP + dir * rayHit.ray.tfar;
	hitInfo.dis = rayHit.ray.tfar;
	hitInfo.hitId = rayHit.hit.geomID;

	return true;
}

void EmbreeScene::getNearestPoint(const Vec3f& srcP) {
	RTCPointQuery queryS;
	RTCPointQueryContext queryContex;
	rtcInitPointQueryContext(&queryContex);

	queryS.x = srcP.x;
	queryS.y = srcP.y;
	queryS.z = srcP.z;
	queryS.radius = 1e10;
	queryS.time = 0.0f;
	ClosestPointResult ret;
	

	auto t = rtcPointQuery(pScene,&queryS,&queryContex,nullptr,(void*)(&ret));
	int a = 0;

}

void EmbreeScene::setGeoTransfrom(const std::string& geoName, Mat4f mat) {
	if (geos.find(geoName) == geos.end()) {
		std::cout << "geo name error" << std::endl;
		return;
	}
	float transForm[12];
	for (uint32 r = 0; r < 3; ++r)
		for (uint32 c = 0; c < 3; ++c)
			transForm[r * 3 + c] = mat[{c, r}];
	transForm[9] = mat[{0, 3}];
	transForm[10] = mat[{1, 3}];
	transForm[11] = mat[{2, 3}];
//	rtcSetGeometryTransform(geos[geoName], 0, RTC_FORMAT_FLOAT4X4_ROW_MAJOR, mat.rawData());
	rtcSetGeometryTransform(geos[geoName], 0, RTC_FORMAT_FLOAT4X4_COLUMN_MAJOR, transForm);
	rtcCommitGeometry(geos[geoName]);
	rtcCommitScene(pScene);

	//Mat4f newMat;// = Mat4f::getIdentity();
	float retMat[12];
	//rtcGetGeometryTransform(geos[geoName], 0, RTC_FORMAT_FLOAT4X4_ROW_MAJOR,retMat);
	int a = 0;
	

}
void EmbreeScene::setInstanceMatrix(Mat4f mat, int instanceId) {
	if (instanceId >= sceneInstance.size()) {
		cout << "error: index of instanceID " << instanceId << endl;
		return;
	}
	rtcSetGeometryTransform(sceneInstance[instanceId], 0, RTC_FORMAT_FLOAT3X4_ROW_MAJOR, mat.rawData());
	
}

void EmbreeScene::addInstance(int stepCoun) {
	auto instanceGeo = rtcNewGeometry(pDevice.lock()->getRTDeviceP(), RTC_GEOMETRY_TYPE_INSTANCE);
	rtcSetGeometryInstancedScene(instanceGeo, pScene);
	rtcSetGeometryTimeStepCount(instanceGeo, 1);
	sceneInstance.push_back(instanceGeo);
}
END_P3D_NS