#include "p3dVertexBuffer.h"
#include "OPenGL/OGLVertexBuffer.h"
BEGIN_P3D_NS
pVertexBuffer VertexBuffer::make(uint32 appType) {
	if (appType == APP_OPENGL)
		return std::make_shared<OGLVertexBuffer>();
	return nullptr;

}

END_P3D_NS