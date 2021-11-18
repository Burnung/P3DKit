#include "common/RenderCommon.h"
#include "common/P3D_Utils.h"
#include "Texture.h"
#pragma  once
BEGIN_P3D_NS
class RenderFrame :public P3DObject {
public:
	//RenderFrame() = default;
	explicit RenderFrame(uint32 w, uint32 h, uint32 ssa) :w(w), h(h), ssa(ssa) {};
	virtual ~RenderFrame(){};
	static std::shared_ptr<RenderFrame> make(uint type,uint32 w, uint32 h, uint32 ssa);
	virtual void addRenderTexture(pTexture tTexture) {};
	virtual void setRenderTexture(pTexture tTexture) {};
	virtual void applyMulti(uint32 nCount) {};
	virtual void apply() {};
	virtual void apply0() {};
	virtual void addRenderTextures(uint32 nCount) {};
	virtual void resize(int32 w, int32 h) {};
	pTexture getRenderTexture(uint32 index) {
		P3D_ENSURE(index < renderCount, "index is invalid");
		return renderTex[index];
	};
	void unBindTexture(pTexture tex) {
		if (tex == defaultTex)
			return;
		if (nowTex == tex)
			nowTex = defaultTex;
		doFor(i, renderTex.size()) {
			if (renderTex[i] == tex) {
				renderTex.erase(renderTex.begin() + i);
				break;
			}
		}
	}
	pCPuImage getRenderTextureCpu(uint32 index,bool src = false,PixelFormat pixFormat=PIXEL_RGBA8) {
		P3D_ENSURE(index < renderCount, "index is invalid");
		return renderTex[index]->dump2Buffer(src,pixFormat);
	}
	uint32 getWidth()const { return w; }
	uint32 getHeight() const { return h; }
	uint32 getSsa()const { return ssa; }
	uint32 getRenderCount()const { return renderCount; }
protected:
	uint32 w = 0, h = 0, ssa = 1;
	std::vector<pTexture> renderTex;
	pTexture defaultTex = nullptr;
	pTexture nowTex = nullptr;
	uint32 renderCount = 0;

};
typedef std::shared_ptr<RenderFrame> pRenderFrame;
//using pRenderFrame = std::shared_ptr<RenderFrame>;
END_P3D_NS