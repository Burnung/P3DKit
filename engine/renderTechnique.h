#pragma once
#include "common/P3D_Utils.h"
#include "common/RenderCommon.h"
#include "renderable.h"
#include "renderFrame.h"
#include "P3dEngine.h"

BEGIN_P3D_NS

class RenderTechnique : P3DObject {

public:
	struct RenderPass{
		explicit RenderPass(std::vector<pRenderable> renderable, uint32 renderNum, uint32 w, uint32 h,pRenderFrame tFbo=nullptr) :renderables(renderable), num(renderNum) {
			if (!tFbo) {
				fbo = P3DEngine::instance().createRenderFrame(w, h, 1);
				fbo->addRenderTextures(num);
			}
			else {
				fbo = tFbo;
				clearBuffer = false;
			}
		}
		uint32 num;
		std::vector<pRenderable> renderables;
		pRenderFrame fbo;
		pCamera cam;
		bool clearBuffer = true;
		void draw() const {
			fbo->applyMulti(num);
			if(clearBuffer)
				P3DEngine::instance().clearBuffers(0.0, 0.0, 0.0, 0.0);
			for (auto& renderable : renderables) {
				auto tShader = renderable->getShader();
				tShader->setCamera(cam);
				renderable->draw();
			}
			fbo->apply0();
		}
		void setCamera(pCamera tCam) {
			cam = tCam;
		}
		pShader getShader() { return renderables[0]->getShader(); }
	};


public:
	explicit RenderTechnique(uint32 w, uint32 h) : winW(w), winH(h) {}
	virtual ~RenderTechnique() {};
	
public:
	void addRenderPass(pRenderable renderable,uint32 renderNum,bool lastPas = false,uint32 nSample=1);
	void addRenderPass(std::vector<pRenderable> renderables, uint32 renerNum);
	pRenderFrame render();
	void setCamera(pCamera camera) { mCamera = camera; }
	pCamera getCamera() const { return mCamera; }
	pRenderFrame getPassFrame(int idx);
	pShader getShader(int32 ti = 0) {
		return passes[ti].getShader();
	}


private:
	uint32 winW= 512, winH = 512;
	pCamera mCamera;
	std::vector<RenderPass> passes;



};

typedef std::shared_ptr<RenderTechnique> pTechnique;
END_P3D_NS
