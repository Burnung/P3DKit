#pragma once

#include "common/P3D_Utils.h"
#include "Shader.h"
#include "renderable.h"

BEGIN_P3D_NS
class LineGPU :public Renderable {
public:
	static std::shared_ptr<LineGPU> make(uint8 type);
	virtual ~LineGPU();
	virtual void setShader(pShader) override {};
	virtual void init(pDataBlock vps, pDataBlock tl) {};
	virtual void update(pDataBlock vps) {};
    virtual void draw()override {};
	void setModelMat(const Mat4f&m) { modelMat = m; }
protected:
	Mat4f modelMat;

};

typedef std::shared_ptr<LineGPU> pLineGPU;

END_P3D_NS
