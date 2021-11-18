//
// Created by burning on 3/10/19.
//
#pragma once

#include <string>
#include <fstream>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <math.h>

#include<queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#if IS_WINDOWS
#define __P3D_WIN__
#endif

#if IS_LINUX
#define __P3D_LINUX__

#include <cstring>
#endif

#define BEGIN_P3D_NS namespace P3D{
#define END_P3D_NS }

#define P3D_THROW_ARGUMENT(...) throw std::invalid_argument(::P3D::Utils::makeStr(__VA_ARGS__))
#define P3D_THROW_RUNTIME(...) throw std::runtime_error(::P3D::Utils::makeStr(__VA_ARGS__))

#ifdef min
	#undef min
#endif
#ifdef max
	#undef max
#endif

BEGIN_P3D_NS



typedef unsigned char       uint8;
typedef unsigned short      uint16;
typedef unsigned int        uint32;
typedef unsigned int        uint;
typedef int                 int32;
typedef short               int16;
typedef char                int8;
typedef float               float32;
typedef double              float64;
typedef unsigned long long  uint64;
typedef long long           int64;

typedef const std::string& cstring;

class NdArray;
class PNdArray;
template <typename T>
class NdArrayT;
template <typename T>
class PNdArrayT;
typedef PNdArrayT<float> PNdArrayF;
typedef PNdArrayT<int> PNdArrayI;
typedef PNdArrayT<int64> PNdArrayI64;
typedef PNdArrayT<uint32> PNdArrayU;
typedef PNdArrayT<double> PNdArrayD;
typedef PNdArrayT<uint8_t> PNdArrayU8;
typedef PNdArrayT<bool> PNdArrayB;
typedef std::vector<char> ByteBuffer;
typedef std::shared_ptr<ByteBuffer> PBuffer;
	

#define USE_P3D_NS using namespace P3D;

#define doFor(k,n) for(uint32 k =0; k < n;++k)

const uint32 MAX_STR_LEN = 1024;
const uint32 MAX_PATH_LEN = 512;
const float64 PI = 3.141592653589793238462643383279502884;
const float C_PI = (float)3.14159265359;
const float32 C_e = (float)2.71828182846;

template<typename T>
inline T sqr(T x) { return x * x; }
enum class DType{
		NoType = 0,
		Float32 = 1,
		Int32 = 2,
		UInt8 = 3,
		Int64 = 4,
		Int16 = 5,
		Float64 = 6,
		Uint16 = 7,
		Uint32 = 8,
		String = 9,
		Bool = 10,
		DObject = 20,
		DHolder = 21
};

struct DTypeUtil {
	static std::string toString(DType dtype);
	static DType toDType(cstring name);
	template <typename T, typename Enabled=void>
	static DType toDType() {return DType::NoType;}
	static int getSize(DType type);
	static bool isNumeric(DType type);
};
template <>
inline DType DTypeUtil::toDType<float>() {return DType::Float32;}
template <>
inline DType DTypeUtil::toDType<int>() {return DType::Int32;}
template <>
inline DType DTypeUtil::toDType<uint8_t>() {return DType::UInt8;}
template <>
inline DType DTypeUtil::toDType<int64_t>() {return DType::Int64;}
template <>
inline DType DTypeUtil::toDType<bool>() {return DType::Bool;}
template <>
inline DType DTypeUtil::toDType<int16_t>() {return DType::Int16;}
template <>
inline DType DTypeUtil::toDType<double>() {return DType::Float64;}
template <>
inline DType DTypeUtil::toDType<uint32>() {return DType::Uint32;}
template <>
inline DType DTypeUtil::toDType<uint16>() {return DType::Uint16;}

/*

enum CommonDataType {
	DATA_TYPE_INVALID = 0,
	DATA_TYPE_INT8,
	DATA_TYPE_INT16,
	DATA_TYPE_INT32,
	DATA_TYPE_UINT8,
	DATA_TYPE_UINT16,
	DATA_TYPE_UINT32,
	DATA_TYPE_FLOAT16,
	DATA_TYPE_FLOAT32,
	DATA_TYPE_FLOAT64,
	DATA_TYPE_NUM,
};
*/

//const uint8 CommonDataSize[DATA_TYPE_NUM] = {1, 1,2,4,1,2,4,2,4,8 };

template<typename T>
inline T clamp(T v, T minv, T maxv){
	if (v < minv) return minv;
	if (v > maxv) return maxv;
	return v;
}
inline int roundi(float x) {
	return (int)round(x);
}
inline int roundi(double x) {
	return (int)round(x);
}
inline int floori(float x) {
	return (int)floor(x);
}
inline int floori(double x) {
	return (int)floor(x);
}
inline int ceili(float x) {
	return (int)ceil(x);
}
inline int ceili(double x) {
	return (int)ceil(x);
}
template<typename T>
T max(const T& a, const T& b) {
	if (a > b)
		return a;
	return b;
}
template<typename T>
T min(const T& a, const T& b) {
	if (a > b)
		return b;
	return a;
}
enum IMAGE_ROTATION {
	IMAGE_ROT0 = 0,
	IMAGE_ROT90_CCW = 1,
	IMAGE_ROT180 = 2,
	IMAGE_ROT90_CW = 3
};

enum IMAGE_MIRROR {
	IMAGE_MIRROR_NO = 0,
	IMAGE_MIRROR_YES = 1
};

class P3DZeroObject{
public:
P3DZeroObject() = default;
~P3DZeroObject() = default;
};

class P3DObject :public P3DZeroObject{
public:
    P3DObject();
    virtual ~P3DObject();
	std::string name;
	//std::string type;
};

#define P3D_SINGLETON_DECLEAR(T)\
private:\
T(){\
init();\
}\
T(const T&) = delete;\
friend class SingletonBase<T>;

template<typename T>
struct DataHolder : public P3DObject {
	T d;
	DataHolder()
	{}
	DataHolder(const T& d) : d(d)
	{}
};
struct StrUtil{
	static bool startsWith(const char* str, const char* prefix);
	static bool startsWith(const std::string& str, const std::string& prefix);
	static bool endsWith(const std::string& str, const std::string& suffix);
	static std::string replace(cstring src, cstring oldSub, cstring newSub);
	static std::string toEscapedString(cstring str);
	static std::string trim(cstring str);
	static std::string toUtf8(const std::u16string& u16String);
	static std::u16string toUtf16(const std::string& str);
	static int lastIndexOf(cstring s, char c);
	static std::vector<std::string> splitLines(const std::string& str);
    static std::vector<std::string> split(cstring str, const char* seps);
    static std::string wildcardToRegex(const std::string& wildcard);
	static ByteBuffer toBuffer(const char* str);
	static ByteBuffer toBuffer(const char* str, int len);
	static PBuffer toPBuffer(const char* str);
	static PBuffer toPBuffer(const char* str, int len);
};


class Utils {
public:

	static void init();
	static std::vector<std::string> splitString(const std::string& str, char c = ' ');

	template<typename T = float>
	static T string2Num(std::string str) {
		T num;
		std::istringstream iss(str);
		iss >> num;
		return num;
	};
	template<typename T=float32>
	static T toRadian(T f) {
		return  static_cast<T>(f / 180.0 * PI);
	}
	template<typename T=float32>
	static T toDegree(T r) {
		return  static_cast<T>(r / PI * 180.0);
	}
	static std::string readFileString(const std::string& fpath) {
		std::ifstream infile(fpath, std::ios_base::binary);
		if (!infile)
			throw std::invalid_argument(makeStr("Cannot open file %s", fpath.c_str()));
		//std::string str(std::istreambuf_iterator<char>(infile),std::istreambuf_iterator<char>());
		std::string str((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
		return str;
	}
	static std::vector<std::string> readFileLines(const std::string& fpath) {
		auto strs = readFileString(fpath);
		return splitLines(strs);
	}
	static std::vector<std::string> splitLines(const std::string& str);
	static float64 getCurrentTime();
	static std::string makeStr(const char* ,...);
	static std::string flattenFileStr(const std::string& fPath);

	static std::vector<char> readFileData(const std::string& fpath);
	template<typename T>
	static T readFileData(cstring fpath) {
		T t;
		auto f = fopen(fpath.c_str(), "rb");
		if(f != nullptr){
			fread(&t, sizeof(T), 1, f);
			fclose(f);
		}else{
		    throw std::runtime_error(makeStr("Cannot open %s", fpath.c_str()));
		}
		return t;
	}
	static void writeFileData(const std::string& fpath, const char* data, size_t dataLen);
	template<typename T>
	static void writeFileData(const std::string& fpath, const T& data) {
		writeFileData(fpath, (const char*)data,  sizeof(T));
	}
	template<typename T>
	static void writeFileData(const std::string& fpath, const std::vector<T>& data) {
		writeFileData(fpath, (const char*)data.data(), data.size() * sizeof(T));
	}
	static void writeFileData(const std::string& fpath, cstring data) {
		writeFileData(fpath, (const char*)data.data(), data.size());
	}
	static void writeFileData(const std::string& fpath, const std::vector<char>& data) {
		writeFileData(fpath, data.data(), data.size());
	}
	static bool isRegularFile(const std::string& f);
	static std::string getBaseName(const std::string& fPath);
	static bool isAbsPath(const std::string& fpath);
	static std::string getAbsPath(const std::string& fpath);
	static std::string joinPath(const std::string& tDir, const std::string& fName);
	static std::string getExeDir();
	static bool isDir(const std::string& f);
	static std::string getFileDir(const std::string&f);

	template<typename T> 
	static constexpr T numMipmapLevels(T width, T height) {
		T levels = 1;
		while ((width | height) >> levels) {
			++levels;
		}
		return levels;
	}

#ifdef WIN32
	static const char PathSep = '\\';
#else
	static const char PathSep = '/';
#endif


private:
	struct Data;
	static std::shared_ptr<Data> data;
};



class NoCopyable{
protected:
    NoCopyable() = default;
private:
    NoCopyable(const NoCopyable& t) = delete;
    const NoCopyable& operator = (const NoCopyable& t) = delete;
};

class SpinLock :public NoCopyable{
public:
    SpinLock() = default;
    void lock(){
        while(flag.test_and_set(std::memory_order_acquire));
    }
    void  unlock(){
        flag.clear(std::memory_order_release);
    }
private:
    std::atomic_flag flag = ATOMIC_FLAG_INIT;

};

class P3DDebugTracer{
public:
    static void enableTrace(bool b);
    static void addTrace(P3DObject* p);
    static  void remove(P3DObject* p);
    static void print();

public:
    static bool isTracing;
private:
    static SpinLock lock;
    static std::unordered_map<std::string,int> data;

};

template <typename T>
class SingletonBase {
protected:
    SingletonBase<T>() = default;
    SingletonBase(const SingletonBase<T>& s) = delete;
    SingletonBase& operator=(const SingletonBase<T> &) = delete;

public:
    //friend T;
	virtual void init() {};
    static T& instance(){
        static T ret;
        return ret;
    }
};

class LogOut :public P3DObject{
public:
    LogOut() = default;
    explicit LogOut(const std::string& filePath) {
        init(filePath);
    }
public:
    bool init(std::string filePath = "logOut.txt");
    void close();
    void log(const std::string& );

private:
    std::ofstream ofile;
    bool isInit = false;
};
extern LogOut logOut;

#define P3D_ENSURE(x,msg) if(!(x)) {\
    auto tmsg = msg;\
    logOut.log(tmsg);\
    throw std::runtime_error(tmsg);\
}

class DataBlock :public P3DObject,public NoCopyable{
public:
    DataBlock() = default;
    explicit DataBlock(uint32 size, DType dataType = DType::NoType) : _dataSize(size),dataType(dataType){
        _data = new uint8[size];
		memset(_data, 0, _dataSize);
	//	if(dataType == DType::NoType)
		//	dataType = DTypeUtil::getSize()
    }
	explicit DataBlock(const void* srcData, uint32 size, bool holdData = true, DType dataType = DType::NoType):_dataSize(size),dataType(dataType),holdData(holdData) {
		if (holdData) {
			_data = new uint8[size];
			memcpy(_data, srcData, size);
		}
		else
			_data = (uint8*)srcData;
	}
	~DataBlock() {
		//std::cout<<Utils::makeStr("free memery,%d", _dataSize);

		if (_data&&holdData) {
			delete[] _data;
			_data = 0;
			_dataSize = 0;
		}
	}

public:
    template <typename T = uint8>
    inline T* rawData(){
        return reinterpret_cast<T*>(_data);
    }

    template <typename T = uint8>
    inline const T* rawData() const{
        return reinterpret_cast<T*>(_data);
    }

	template<typename T =uint8>
	inline T getDataIndex(uint32 i) {
		return rawData<T>()[i];
	}
	void setSize(uint32 size) {
		if (_dataSize != size) {
			delete[] _data;
			_data = new uint8[size];
			_dataSize = size;
		}

	}
    uint32 getSize(){
        return _dataSize;
    }
	void setFromMem(uint8* srcData, uint32 nSize) {
		if (nSize > _dataSize)
			setSize(nSize);
		memcpy(_data, srcData, nSize);
	}
	void copyData(const void* srcData, uint32 nSize, uint32 offset) {
		P3D_ENSURE((nSize + offset <= _dataSize), "data block should bigger");
		memcpy(_data + offset, srcData, nSize);
	}
    std::shared_ptr<DataBlock> clone(){
        auto ret = std::make_shared<DataBlock>(this->_dataSize,this->dataType);
        memcpy(ret->rawData(),this->rawData(),this->_dataSize);
        return ret;
    }
	void copyFromData(std::shared_ptr<DataBlock> src) {
		setSize(src->getSize());
		memcpy(_data, src->rawData(), _dataSize);
	}
	void setDataType(DType type_) { dataType = type_; }
	DType getDataType()const { return dataType; }

private:
    uint32 _dataSize = 0;
	uint32 holdData = true;
	uint8* _data = nullptr;
	DType dataType;
};


struct MathUtil {
	template <typename T>
	static T sum(const std::vector<T>& vals) {
		T s = 0;
		for (T v : vals)
			s += v;
		return s;
	}

	template <typename T>
	static T prod(const std::vector<T>& vals) {
		T s = 1;
		for (auto v : vals)
			s *= v;
		return s;
	}

	template <typename T>
	static T dot(const std::vector<T>&a, const std::vector<T>&b) {
		size_t size = a.size();
		assert(size == b.size());
		T s = 0;
        for (size_t i = 0; i < size; i++)
			s += a[i] * b[i];
		return s;
	}

	template <typename T, size_t size>
	static T dot(const std::array<T, size>&a, const std::array<T, size>&b) {
		T s = 0;
		for (int i = 0; i < size; i++)
			s += a[i] * b[i];
		return s;
	}

	template<typename T>
	static T toRange(T v, T vmin, T vmax) {
		if(v<vmin) return vmin;
		if(v>vmax) return vmax;
		return v;
	}

	static int sign(int x) {
		if(x==0) return 0;
		return x>0?1:-1;
	}

	//min larger than x/y
	static int divUp(int x, int y) {
		//x==r*y+q, x/y=r+q/y
		int r = x/y, q=x%y;
		if(q==0) return r+1;
		else if((q>0&&y>0)||(q<0&&y<0)) return r+1;
		else return r;
	}

	//ceil(x/y)
	static int divCeil(int x, int y) {
		//x==r*y+q, x/y=r+q/y
		int r = x/y, q=x%y;
		if(q==0) return r;
		else if((q>0&&y>0)||(q<0&&y<0)) return r+1;
		else return r;
	}

	//max smaller than x/y
	static int divDown(int x, int y) {
		//x==r*y+q, x/y=r+q/y
		int r = x/y, q=x%y;
		if(q==0) return r-1;
		else if((q>0&&y>0)||(q<0&&y<0)) return r;
		else return r-1;
	}

	//floor(x/y)
	static int divFloor(int x, int y) {
		//x==r*y+q, x/y=r+q/y
		int r = x/y, q=x%y;
		if(q==0) return r;
		else if((q>0&&y>0)||(q<0&&y<0)) return r;
		else return r-1;
	}

	static float softmax0(float a, float b) {
		a = exp(a);
		b = exp(b);
		return a / (a + b);
	}

	template <typename T, size_t size>
	static std::array<T, size> softmax(const std::array<T, size>& arr) {
		T sum = 0;
		std::array<T, size> rlt;
		for (size_t i = 0; i < size; i++) {
			auto x = exp(arr[i]);
			rlt[i] = x;
			sum += x;
		}
		for (size_t i = 0; i < size; i++)
			rlt[i] /= sum;
		return rlt;
	}

	static std::vector<float> linspace(float minv, float maxv, int n) {
		if (n == 0) return {};
		else if (n == 1) return { minv };
		std::vector<float> vals(n);
		float step = (maxv - minv) / (n - 1);
		vals[0] = minv;
		for (int i = 1; i < n; i++)
			vals[i] = vals[i - 1] + step;
		return vals;
	}

	template<typename TDst, typename TSrc>
	static std::vector<TDst> cast(const std::vector<TSrc>& vec) {
		std::vector<TDst> dst(vec.size());
		for (size_t i = 0; i < vec.size(); i++) {
			dst[i] = TDst(vec[i]);
		}
		return dst;
	}

	static std::pair<std::vector<int>, std::vector<float>> histogram(const std::vector<float>& vals, 
		int binCount, std::pair<float, float> minMax);
};



typedef std::shared_ptr<DataBlock> pDataBlock;


class ThreadPool {
public:
	ThreadPool(size_t);
	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args)
		->std::future<typename std::result_of<F(Args...)>::type>;
	~ThreadPool();
	void callAll();
private:
	// need to keep track of threads so we can join them
	std::vector< std::thread > workers;
	// the task queue
	std::queue< std::function<void()> > tasks;

	// synchronization
	std::mutex queue_mutex;
	std::condition_variable condition;
	bool stop;
};



END_P3D_NS
