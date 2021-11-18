//
// Created by bunring on 19-3-11.
//

#pragma once

#include"P3D_Utils.h"
#include "NdArray.h"
#include "mathIncludes.h"

BEGIN_P3D_NS

namespace SHADER_KEY_WORDS {
	const std::string VIEW_MAT = "viewMat";
	const std::string  PRO_MAT = "proMat";
	const std::string MVP = "mvp";
	const std::string MODEL_MAT = "modelMat";
	const std::string CAMERA_POS = "camPos";
	const std::string CAMEERA_DIR = "camDir";
	const std::string ORTH_WORLD_WIDTH = "orthWorldWidth";
	const std::string ORTH_WORLD_HEIGHT = "orthWorldHeight";
	const std::string VIEW_RECT = "u_viewRect";
	const std::string U_BONE_MATS = "u_boneMats";

    const std::string IN_VERT_POS = "InPos";
    const std::string IN_VERT_NORMAL = "InVn";
    const std::string IN_VERT_UV = "InUv";
    const std::string IN_VERT_TANGENT = "InTangent";
    const std::string IN_VERT_BITANGETN = "InBitangent";
    const std::string IN_VERT_COLOR = "InVc";

	//light prefix
	const std::string POINT_LIT_POS_STRING = "pointLits.pos[%d]";
	const std::string POINT_LIT_COLOR_STRING = "pointLits.color[%d]";

	//pbr material
	const std::string PBR_ALBEDO = "pbrMaterial.albedo";
	const std::string PBR_MATALLIC = "pbrMaterial.matallic";
	const std::string PBR_ROUGHNESS = "pbrMaterial.roughness";
	const std::string PBR_AO = "pbrMaterial.ao";
	const std::string PBR_ALBEDO_TEX = "pbrMaterial.albedo_tex";
	const std::string PBR_MATALLIC_TEX = "pbrMaterial.matallic_tex";
	const std::string PBR_ROUGHNESS_TEX = "pbrMaterial.roughness_tex";
	const std::string PBR_AO_TEX = "pbrMaterial.ao_tex";
}
enum {
	OGL_APPLAY,
	VUL_APPLAY,
};

enum PixelFormat{
        PIXEL_NONE,
        PIXEL_R8,
        PIXEL_RG8,
        PIXEL_RGB8,
        PIXEL_RGBA8,
        PIXEL_RGBF,
        PIXEL_RGBAF,
        PIXEL_DEPTHF,

		PIXEL_BGR8,
		PIXEL_BGRF,
		PIXEL_BGRA8,
		PIXEL_BGRAF,
        PIXEL_FORMAT_COUNT,
};

enum BUFFER_USE_TYPE{
        READ_ONLY,
        WRITE_ONLY,
        READ_WRITE,
		USE_NUM,
};

enum TextureType{
    TEXTURE_1D,
    TEXTURE_2D,
    TEXTURE_3D,
    TEXTURE_CUBE,
};
enum SampleType {
	SAMPLE_NEASRST = 0,
	SAMPLE_LINER,
};
enum ApplyType
{
	APP_NVALID=0,
	APP_OPENGL,
	APP_VULKAN,
};
enum VertexBufferType{
	VERTEX_BUFFER_ARRAY=0,
	VERTEX_BUFFER_ELEMETN,
	VERTEX_BUFFER_UNIFORM,
	VERTEX_BUFFER_NUM,
};

enum GeoTYPE {
	GEO_TYPE_TRIS=0,
	GEO_TYPE_LINES,
	GEO_TYPE_POINTS,
	GEO_TYPE_NUM,
};
enum VertexBufferUsage {
	STATIC_READ=0,
	STATIC_COPY,
	STATIC_DRAW,
	DYNAMIC_DRAW,
	VBSAGET_NUM,
};
class RenderUtil{
    static uint32 getPixelSize();
};

class CpuImage :public P3DObject ,public NoCopyable{
public:
    CpuImage() = delete;
    CpuImage(TextureType t,PixelFormat format, uint32 w,uint32 h,uint32 d,bool useuint8 = true);
	explicit CpuImage(PNdArray arrData,bool isBRG=false);
    
public:
    uint8* getRawData(int index = 0);
    uint32 getBufferSize() {return bufferSize;}
    
    PixelFormat getPiexFormat() {return pixelFormat;}
    TextureType getTextureType(){return type;}
    uint32 getWidth(){return width;}
    uint32 getHeight(){return height;}
    uint32 getDepth(){return depth;}
	void setFromBuffer(uint8* data,uint32 index = 0);
	PNdArray getNdArray(int index = 0) { return buffers[index];}
 
public:
    uint32  width = 0,height = 0,depth = 0;
    uint32 bufferSize = 0;
    uint32 nchannel =3;
    uint32 pitch = 0;
    std::vector<PNdArray> buffers;
    PixelFormat pixelFormat = PIXEL_NONE;
    TextureType type = TEXTURE_2D;
	bool isBGR = false;
};
typedef std::shared_ptr<CpuImage> pCPuImage;

class RenderState :public P3DObject {
public:
	virtual void saveRenderState() = 0;
	virtual void recoverRenderState()=0;
};
typedef std::shared_ptr<RenderState> pRenderState;

class RenderStateSetting :public P3DObject {
public:
	virtual void apply() = 0;
};
typedef std::shared_ptr<RenderStateSetting> pRenderStateSetting;

class RenderStateFactory :public P3DObject {
public:
	virtual pRenderStateSetting createRenderState(const std::string& settingType, const std::vector<std::string> params) = 0;
};
typedef std::shared_ptr<RenderStateFactory> pRenderStateFactory;

struct Value {
	enum ValueType{
		VALUE_NONE,
		VALUE_FLOAT,
		VALUE_INT,
		VALUE_INT_VECTOR,
		VALUE_FLOAT2,
		VALUE_FLOAT3,
		VALUE_FLOAT4,
		VALUE_FLOAT_VECTOR,
		VALUE_MAT3,
		VALUE_MAT4,
		VALUE_FLOAT3_VECTOR,
		VALUE_FLOAT4_VECTOR,
		VALUE_MAT4_VECTOR,
		VALUE_STRING,
		VALUE_PTR,
		VALUE_BOOL,
		VALUE_NUM
	};
	Value() = default;
	Value(ValueType tType) :mType(tType) {
		initData();
	}
	
	Value(float v) :mType(VALUE_FLOAT) { 
		initData();
		setValue(&v); 
	}
	Value(int v) :mType(VALUE_INT) { 
		initData();
		setValue(&v); 
	}
	Value(std::vector<int> v) :mType(VALUE_INT_VECTOR) { 
		initData(v.size());
		setValue(&v); 
	}
	Value(const Vec2f& v) :mType(VALUE_FLOAT2) { 
		initData();
		setValue(&v); 
	}
	Value(const Vec3f& v) :mType(VALUE_FLOAT3) { 
		initData();
		setValue(&v); 
	}
	Value(const Vec4f& v) :mType(VALUE_FLOAT4) { 
		initData();
		setValue(&v); 
	}
	Value(const std::vector<Vec3f>& v) :mType(VALUE_FLOAT3_VECTOR) { 
		initData(v.size());
		setValue(v.data()); 
	}
	Value(const std::vector<Vec4f>& v) :mType(VALUE_FLOAT4_VECTOR) { 
		initData(v.size());
		setValue(v.data()); 
	}
	Value(const std::vector<float>& v) :mType(VALUE_FLOAT_VECTOR) { 
		initData(v.size());
		setValue(v.data()); 
	}
	Value(const Mat3f& v) :mType(VALUE_MAT3) { 
		initData();
		setValue(&v); 
	}
	Value(const Mat4f& v) :mType(VALUE_MAT4) { 
		initData();
		setValue(&v); 
	}
	
	Value(const std::vector<Mat4f>&v) :mType(VALUE_MAT4_VECTOR) { 
		initData(v.size());
		setValue((void*)v.data()); 
	}
	Value(const std::string& v) :mType(VALUE_STRING) {
		initData(v.length()+1);
		setValue(v.data(),v.length());
		char*tData = rawData->rawData<char>();
		tData[v.length()] = 0;
	}
	Value(bool v) :mType(VALUE_BOOL) {
		initData();
		setValue(&v);
	}
	Value(void* v) :mType(VALUE_PTR) {
		initData();
		setValue(&v);
	}
private:
	void initData(int tSize=-1){
		if (tSize < 0) tSize = 4;
		switch (mType)
		{
		case P3D::Value::VALUE_FLOAT:
			rawData = std::make_shared<DataBlock>(sizeof(float32), DType::Float32);
			break;
		case P3D::Value::VALUE_INT:
			rawData = std::make_shared<DataBlock>(sizeof(int32), DType::Int32);
			break;
		case P3D::Value::VALUE_INT_VECTOR:
			rawData = std::make_shared<DataBlock>(sizeof(int32)*tSize, DType::Int32);
			break;
		case P3D::Value::VALUE_FLOAT2:
			rawData = std::make_shared<DataBlock>(sizeof(Vec2f), DType::Float32);
			break;
		case P3D::Value::VALUE_FLOAT3:
			rawData = std::make_shared<DataBlock>(sizeof(Vec3f), DType::Float32);
			break;
		case P3D::Value::VALUE_FLOAT4:
			rawData = std::make_shared<DataBlock>(sizeof(Vec4f), DType::Float32);
			break;
		case P3D::Value::VALUE_FLOAT_VECTOR:
			rawData = std::make_shared<DataBlock>(sizeof(float32)*tSize, DType::Float32);
			break;
		case P3D::Value::VALUE_MAT3:
			rawData = std::make_shared<DataBlock>(sizeof(Mat3f), DType::Float32);
			break;
		case P3D::Value::VALUE_MAT4:
			rawData = std::make_shared<DataBlock>(sizeof(Mat4f), DType::Float32);
			break;
		case P3D::Value::VALUE_FLOAT3_VECTOR:
			rawData = std::make_shared<DataBlock>(sizeof(Vec3f)*tSize, DType::Float32);
			break;
		case P3D::Value::VALUE_FLOAT4_VECTOR:
			rawData = std::make_shared<DataBlock>(sizeof(Vec4f)*tSize, DType::Float32);
			break;
		case P3D::Value::VALUE_MAT4_VECTOR:
			rawData = std::make_shared<DataBlock>(sizeof(Mat4f)*tSize, DType::Float32);
			break;
		case P3D::Value::VALUE_STRING:
			rawData = std::make_shared<DataBlock>(tSize, DType::UInt8);
			break;
		case P3D::Value::VALUE_PTR:
			rawData = std::make_shared<DataBlock>(sizeof(void*), DType::Uint32);
			break;
		case P3D::Value::VALUE_BOOL:
			rawData = std::make_shared<DataBlock>(sizeof(bool), DType::Bool);
			break;
		default:
			std::cout << "type error" << std::endl;
			break;
		}
	}
public:

	void setValue(const void * ptr,int32 tl = -1) {
		if (tl < 0)
			tl = rawData->getSize();
		rawData->copyData(ptr, tl, 0);
	}
	std::vector<float> toFloats() {
		std::vector<float> ret;
		switch (mType)
		{
		case P3D::Value::VALUE_FLOAT:
	
		case P3D::Value::VALUE_FLOAT2:
		case P3D::Value::VALUE_FLOAT3:
		case P3D::Value::VALUE_FLOAT4:
		case P3D::Value::VALUE_FLOAT_VECTOR:
		case P3D::Value::VALUE_MAT3:
		case P3D::Value::VALUE_MAT4:
		case P3D::Value::VALUE_FLOAT3_VECTOR:
		case P3D::Value::VALUE_FLOAT4_VECTOR:
		case P3D::Value::VALUE_MAT4_VECTOR: {
			int nSize = rawData->getSize() / sizeof(float);
			ret.resize(nSize);
			memcpy(ret.data(), rawData->rawData(), nSize * sizeof(float));
			break;
		}
		case P3D::Value::VALUE_INT:
			//break;
		case P3D::Value::VALUE_INT_VECTOR:
		{
			int nSize = rawData->getSize() / sizeof(int);
			int* tData = rawData->rawData<int>();
			ret.resize(nSize);
			for (int i = 0; i < nSize; ++i)
				ret[i] = tData[i];
			break;
			
		}
		case P3D::Value::VALUE_BOOL:
		{
			ret.resize(1);
			auto tb = rawData->rawData<bool>();
			ret[0] = tb ? 1.0f : 0.0f;
			break;
		}
		default:
			break;
		}
		return ret;
	}
	pDataBlock getDataBlock() { return rawData; }
	ValueType mType = VALUE_NONE;
	std::shared_ptr<DataBlock> rawData=nullptr;
};
END_P3D_NS