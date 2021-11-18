#pragma  once

#include "renderFrame.h"
BEGIN_P3D_NS
class DefaultRenderFrame :public RenderFrame{
public:
	explicit DefaultRenderFrame(uint32 w, uint32 h, uint32 fboId=0) :RenderFrame(w, h, 1) {
		frameId = fboId;
	};
	virtual ~DefaultRenderFrame(){};
	virtual void resize(int32 w, int32 h) {
		this->w = w;
		this->h = h;
	};
public:
	virtual void apply() {};
	virtual void apply0() {};
private:
	uint32 frameId = 0;
};
//typedef std::shared_ptr<RenderFrame> pRenderFrame;
//using pRenderFrame = std::shared_ptr<RenderFrame>;
END_P3D_NS