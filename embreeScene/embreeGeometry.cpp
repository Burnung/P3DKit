#include "embreeGeometry.h"
#include "embree3/rtcore.h"
#include "embreeScene.h"
#include "embreeDevice.h"
#include <time.h>  

BEGIN_P3D_NS
using namespace std;
EmbreeGeometry::EmbreeGeometry(std::shared_ptr<EmbreeScene> pScene_) {
	pScene = pScene_;
}


void EmbreeGeometry::initByComp3D(pComp comp,int quality) {
	rtGeo = rtcNewGeometry(pScene.lock()->getPDevice().lock()->getRTDeviceP(), RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(rtGeo, RTCBuildQuality(quality));

	float* vps = (float*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float),comp->nVtex);
	if (comp->cache_vps)
		memcpy(vps, comp->cache_vps->rawData(),comp->cache_vps->getSize());
	else
		memcpy(vps, comp->vps->rawData(), comp->vps->getSize());

	int* tl = (int*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(int), comp->nTri);
	memcpy(tl, comp->tl->rawData(), comp->tl->getSize());
	
	rtcCommitGeometry(rtGeo);

	geoId = rtcAttachGeometry(pScene.lock()->getRTCScene(), rtGeo);

	rtcReleaseGeometry(rtGeo);

	name = comp->name;
	mComp = comp;
}
void EmbreeGeometry::initByRawData(pDataBlock vps_, pDataBlock tl_, int quality) {
	rtGeo = rtcNewGeometry(pScene.lock()->getPDevice().lock()->getRTDeviceP(), RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(rtGeo, RTCBuildQuality(quality));

	float* vps = (float*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), vps_->getSize() / (3 * sizeof(float)));
	memcpy(vps, vps_->rawData(), vps_->getSize());

	int* tl = (int*)rtcSetNewGeometryBuffer(rtGeo, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(int),tl_->getSize() / (3 * sizeof(int)));
	memcpy(tl, tl_->rawData(), tl_->getSize());

	rtcCommitGeometry(rtGeo);

	geoId = rtcAttachGeometry(pScene.lock()->getRTCScene(), rtGeo);

	rtcReleaseGeometry(rtGeo);

	name = Utils::makeStr("rawGeo_%d", time(NULL));
	mComp = nullptr;
}

void EmbreeGeometry::detach() {
	rtcDetachGeometry(pScene.lock()->getRTCScene(), geoId);
}

void EmbreeGeometry::update() {
	if (! mComp || !mComp->cache_vps)
		return;
	auto geom = rtcGetGeometry(pScene.lock()->getRTCScene(), geoId);
	float* vps = (float*)rtcGetGeometryBufferData(geom, RTC_BUFFER_TYPE_VERTEX, 0);
	memcpy(vps, mComp->cache_vps->rawData(), mComp->cache_vps->getSize());

	rtcUpdateGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0);

	rtcCommitGeometry(geom);
}

END_P3D_NS