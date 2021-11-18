#include "common/RenderCommon.h"
#include "common/P3D_Utils.h"
#include "OGLTexture.h"
#include "renderFrame.h"
BEGIN_P3D_NS
class GLFbo:public RenderFrame {
public:
	explicit GLFbo(uint32 w,uint32 h,uint32 ssa);
	virtual ~GLFbo();
	virtual void addRenderTexture(pTexture tTexture);
	virtual void setRenderTexture(pTexture tTexture);
	virtual void applyMulti(uint32 nCount);
	virtual void apply();
	virtual void apply0();
	virtual void addRenderTextures(uint32 nCount);
	virtual void resize(int32 w, int32 h);
private:
	void release();
	
private:
	uint32 fboId = 0;
	uint32 depthPbo = 0;
	int32 lastViewArg[4];
	
};
using pGLFbo = std::shared_ptr<GLFbo>;
END_P3D_NS