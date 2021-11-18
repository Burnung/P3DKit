#include "common/P3D_Utils.h"
#include "Shader.h"
#include "OPenGL/OGLVbo.h"
BEGIN_P3D_NS
MeshGPU::~MeshGPU() {

}
pMeshGPU MeshGPU::make(uint8 type) {
	switch (type)
	{
	case APP_OPENGL:
		return std::make_shared<GLVbo>();
	}
	return nullptr;
}


END_P3D_NS
