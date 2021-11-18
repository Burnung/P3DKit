
#include "common/mathIncludes.h"
#include <embree3/rtcore.h>


#pragma once
BEGIN_P3D_NS
struct EmbreeHitInfo{
	Vec3f pos; //positoin
	Vec3f normal;// normal
	float dis; // dis  with starP
	uint32 hitId;
};

struct ClosestPointResult
{
	ClosestPointResult()
		: primID(RTC_INVALID_GEOMETRY_ID)
		, geomID(RTC_INVALID_GEOMETRY_ID)
	{}

	Vec3f p;
	unsigned int primID;
	unsigned int geomID;
};

namespace EmbreeUtils {
	void checkForError(RTCDevice pembreeDevice, const std::string& use = "");
	void rtErrorCallbackFunc(void* userPtr, RTCError code, const char* str);
	RTCRay generatRay(const Vec3f& startP, const Vec3f& dir, float len, float start = 0.0f, float time = 0.0f, int mask = -1);
	RTCRayHit generatRayHit(const Vec3f& startP, const Vec3f& dir, float len, float start = 0.0f, float time = 0.0f, int mask = -1,
		unsigned int geomID = RTC_INVALID_GEOMETRY_ID, unsigned int primID = RTC_INVALID_GEOMETRY_ID, unsigned int instID = RTC_INVALID_GEOMETRY_ID);
};




END_P3D_NS