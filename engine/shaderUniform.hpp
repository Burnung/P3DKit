#pragma  once
#include "../common/P3D_Utils.h"

#include <string>
BEGIN_P3D_NS
class ShaderUniform :public P3DObject {
public:
	enum UniformType{
		NONE=-1,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_FLOATS,
		TYPE_INTS,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_VEC4,
		TYPE_MAT4,
		TYPE_VEC3S,
		TYPE_VEC4S,
		TYPE_MAT4S,
		TYPE_TEX,
		TYPE_TEX_NAME,

		TYPE_NUM,
	};
private:
	//void initDataBlock(uint32 dataSize, const void* dataptr) {
//		mData = std::make_shared<DataBlock>(dataptr, dataSize, true);
//	}
public:
	Value dataValue;
public:
	std::string locName;
	uint32 dataCount = 1;
	UniformType dataType;
	//pDataBlock mData;
public:
	ShaderUniform() = default;
	~ShaderUniform() = default;
	ShaderUniform(const std::string& tName, int32 v) :locName(tName), dataType(UniformType::TYPE_INT) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}
	ShaderUniform(const std::string& tName, float32 v) :locName(tName), dataType(TYPE_FLOAT) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}
	
	ShaderUniform(const std::string& tName, Vec2f v) :locName(tName), dataType(TYPE_VEC2) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}
	ShaderUniform(const std::string& tName, Vec3f v) :locName(tName), dataType(TYPE_VEC3) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}
	ShaderUniform(const std::string& tName, Vec4f v) :locName(tName), dataType(TYPE_VEC4) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}
	ShaderUniform(const std::string& tName, Mat4f v) :locName(tName), dataType(TYPE_MAT4) {
		//initDataBlock(sizeof(v), &v);
		dataValue = Value(v);
	}

	ShaderUniform(const std::string& tName, const std::vector<Vec4f>& v) :locName(tName), dataType(TYPE_VEC4S) {
		//initDataBlock(sizeof(Vec4f)*v.size(), v.data());
		dataValue = Value(v);
		dataCount = v.size();
	}
	ShaderUniform(const std::string& tName, const std::vector<Vec3f>& v) :locName(tName), dataType(TYPE_VEC3S) {
		//initDataBlock(sizeof(Vec3f)*v.size(), v.data());
		dataCount = v.size();
		dataValue = Value(v);
	}

	ShaderUniform(const std::string& tName, const std::vector<Mat4f>& v) :locName(tName), dataType(TYPE_MAT4S) {
		//initDataBlock(v.size()*sizeof(Mat4f), v.data());
		dataValue = Value(v);
		dataCount = v.size();
	}
	ShaderUniform(const std::string& tName, const std::vector<float32>& v) :locName(tName), dataType(TYPE_FLOATS) {
		//initDataBlock(sizeof(float32)*v.size(), v.data());
		dataValue = Value(v);
		dataCount = v.size();
	}
	ShaderUniform(const std::string& tName, const std::vector<int32>& v) :locName(tName), dataType(TYPE_INTS) {
		//initDataBlock(sizeof(int32)*v.size(), v.data());
		dataValue = Value(v);
		dataCount = v.size();
	}

	ShaderUniform(const std::string& tName, pTexture v) :locName(tName), dataType(TYPE_TEX) {
		Texture* rawPtr = v.get();
		
		dataValue = Value(rawPtr);
		//initDataBlock(sizeof(rawPtr), &rawPtr);
	}
	ShaderUniform(const std::string& tName, const std::string& v) :locName(tName), dataType(TYPE_TEX_NAME) {
		//Texture* rawPtr = v.get();
		//initDataBlock(v.length()+1, v.data());
		dataValue = Value(v);
		//char*tData = mData->rawData<char>();
		//tData[v.length()] = 0;
	}
	void setValue(const Value& v) {
		dataValue = v;
	}
public:
	std::string getLocName()const { return locName; }
	void* getData() {
		//return nullptr;
		if (dataType == TYPE_TEX)
			return *((Texture**)dataValue.rawData->rawData());
		return (void*)dataValue.rawData->rawData(); 
	}
	uint32 getDataCount() { return dataCount; };
	UniformType getDataType()const {
		return dataType;
	}

};
typedef std::shared_ptr<ShaderUniform> pShaderUniform;

END_P3D_NS