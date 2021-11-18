#include "common/mathIncludes.h"
#include <embree3/rtcore.h>
#include "embreeDevice.h"
using namespace std;
BEGIN_P3D_NS
namespace EmbreeUtils {
	void checkForError(RTCDevice pembreeDevice, const string& use) {
		if (!pembreeDevice)
			return;
		auto errorCode = rtcGetDeviceError(pembreeDevice);
		switch (errorCode)
		{
		case RTC_ERROR_UNKNOWN:
			std::cout << use << " UNKNOWN ERROR" << std::endl;
			break;
		case RTC_ERROR_INVALID_ARGUMENT:
			std::cout << use << " INVALID ARGUMENT" << std::endl;
			break;
		case RTC_ERROR_INVALID_OPERATION:
			std::cout << use << " INVALID OPERATION" << std::endl;
			break;
		case RTC_ERROR_OUT_OF_MEMORY:
			std::cout << use << " OUT_OF_MEMORY" << std::endl;
			break;
		case RTC_ERROR_UNSUPPORTED_CPU:
			std::cout << use << " UNSUPPORTED_CPU" << std::endl;
			break;
		case RTC_ERROR_CANCELLED:
			std::cout << use << " ERROR_CANCELLED" << std::endl;
			break;
		default:
			break;
		}

	}
	void rtErrorCallbackFunc(void* userPtr, RTCError code, const char* str) {
		std::cout << "rt ERROR " << str << std::endl;
	}
	RTCRay generatRay(const Vec3f& startP, const Vec3f& dir, float len, float start, float time , int mask ) {
		RTCRay ray;
		ray.org_x = startP.x;
		ray.org_y = startP.y;
		ray.org_z = startP.z;

		ray.dir_x = dir.x;
		ray.dir_y = dir.y;
		ray.dir_z = dir.z;

		ray.tfar = len;

		ray.tnear = start;
		ray.mask = mask;

		ray.time = time;

		return ray;
	}
	RTCRayHit generatRayHit(const Vec3f& startP, const Vec3f& dir, float len, float start, float time , int mask,
		unsigned int geomID , unsigned int primID , unsigned int instID ) {
		auto ray = generatRay(startP, dir, len, start, time, mask);
		RTCHit tHit;
		tHit.geomID = geomID;
		tHit.instID[0] = instID;
		tHit.primID = primID;

		return { ray,tHit };
	}
};



END_P3D_NS