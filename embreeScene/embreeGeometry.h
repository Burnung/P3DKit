#include<embree3/rtcore.h>
#include "common/mathIncludes.h"
#include "geo/comp.h"
BEGIN_P3D_NS
class EmbreeScene;
class EmbreeGeometry :std::enable_shared_from_this<EmbreeGeometry> {

public:
	explicit EmbreeGeometry(std::shared_ptr<EmbreeScene> pScene_);

	~EmbreeGeometry() = default;

	void initByComp3D(pComp comp,int quality = RTC_BUILD_QUALITY_HIGH);
	void initByRawData(pDataBlock vps_, pDataBlock tl_, int quality = RTC_BUILD_QUALITY_HIGH);
	void update();
	void detach();
	//void setModelMatrix(mmu::render::Mat4x4& modelMat);
	//void setModelMatrix(const std::vector<float>& matData);



public:
	uint32 geoId = 0;
	RTCGeometry rtGeo;
	std::string name;
	std::weak_ptr<EmbreeScene> pScene;
	pComp mComp;

};




END_P3D_NS
