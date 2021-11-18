#pragma once
#include "common/RenderCommon.h"

BEGIN_P3D_NS
class VertexBuffer :public P3DObject {
public:
	VertexBuffer() = default;
	virtual ~VertexBuffer() {};
public:
	static std::shared_ptr<VertexBuffer> make(uint32 appType);
public:
	virtual bool init(VertexBufferType type,VertexBufferUsage usage,DType baseDataType, pDataBlock pdata = nullptr) = 0;
	virtual void release() = 0;
	virtual bool update(pDataBlock pData,uint32 offset) = 0;
	virtual bool setData(pDataBlock pData) = 0;
	virtual pDataBlock readData() = 0;
	virtual void applay() = 0;
	virtual void applay(uint32 binding) = 0;
	virtual void applay0() = 0;
	virtual uint32 getGPUBaseDataType() = 0;
	uint32 getDataLen()const { return dataLen; }
	DType getBaseDataType() { return baseDataType; }
protected:
	uint32 mType;
	uint8 mUsage;
	uint32 dataLen = 0;
	DType baseDataType;
};
typedef std::shared_ptr<VertexBuffer> pVertexBuffer;
END_P3D_NS

