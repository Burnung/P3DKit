#include "OGLVertexBuffer.h"
BEGIN_P3D_NS
OGLVertexBuffer::~OGLVertexBuffer() {
	release();
}
bool OGLVertexBuffer::init(VertexBufferType type, VertexBufferUsage usage,DType baseDataType, pDataBlock pData) {
	P3D_ENSURE(vboId == 0, Utils::makeStr("OGLVertexBuffer::init: vboId should be 0!"));
	GLUtils::checkGLError("createBuffer");
	try
	{
		glGenBuffers(1, &vboId);
	}
	catch (int e)
	{
		GLUtils::checkGLError("create Buffer2");
	}
	mType = type;
	mUsage = usage;
	if (pData) {
		setData(pData);
		dataLen = pData->getSize();
	}
	this->baseDataType = baseDataType;
	return true;
}
void OGLVertexBuffer::release() {
	if (vboId != 0) {
		glDeleteBuffers(1, &vboId);
		vboId = 0;
		dataLen = 0;
	}
}
bool OGLVertexBuffer::update(pDataBlock pData,uint32 offset) {
	if (dataLen == 0) {
		return setData(pData);
	}
	P3D_ENSURE(vboId > 0, Utils::makeStr("VERTEXBUFFER not init"));
	glBindBuffer(GLVertexBufferType[mType], vboId);
	glBufferSubData(GLVertexBufferType[mType], offset, pData->getSize(), pData->rawData());
	glBindBuffer(GLVertexBufferType[mType], 0);

}
bool OGLVertexBuffer::setData(pDataBlock pData) {
	P3D_ENSURE(vboId > 0, Utils::makeStr("VERTEXBUFFER not init"));
	glBindBuffer(GLVertexBufferType[mType], vboId);
	glBufferData(GLVertexBufferType[mType], pData->getSize(), pData->rawData(),GLVertexBufferUsage[mUsage]);
	glBindBuffer(GLVertexBufferType[mType], 0);
}
void OGLVertexBuffer::applay() {
	glBindBuffer(GLVertexBufferType[mType], vboId);
}
void OGLVertexBuffer::applay(uint32 binding) {
	if (mType != VERTEX_BUFFER_UNIFORM)
		P3D_THROW_RUNTIME("must be uniform buffer");
	glBindBufferBase(GL_UNIFORM_BUFFER, binding, vboId);
}
void OGLVertexBuffer::applay0() {
	glBindBuffer(GLVertexBufferType[mType],0);
}

uint32  OGLVertexBuffer::getGPUBaseDataType() {
	return GLUtils::getGLBaseType(baseDataType);
}
pDataBlock OGLVertexBuffer::readData() {
	P3D_ENSURE(vboId > 0, Utils::makeStr("VERTEXBUFFER not init"));
	applay();
	auto ret = std::make_shared<DataBlock>(dataLen);
#if IS_IOS
    auto pData = glMapBufferRange(GLVertexBufferType[mType], 0, ret->getSize(), GL_MAP_READ_BIT);
    ret->copyData((uint8*)pData, ret->getSize(), 0);
    glUnmapBuffer(GLVertexBufferType[mType]);
#else
	glGetBufferSubData(GLVertexBufferType[mType], 0, ret->getSize(),ret->rawData());
#endif
	applay0();
	return ret;
}




END_P3D_NS
