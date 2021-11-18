#pragma once

#include "Template_Utils.h"
#include "P3D_Utils.h"
#include <unordered_map>
#include <string>

BEGIN_P3D_NS

void mmu_ndarray_throw_error(std::string s);
// 高维矩阵(或tensor, blob)的封装，目标是重写python的ndarray
class NdArray : public P3DObject
{
protected:
	//char* _data; // 真实的内存数据指针
	pDataBlock _data = nullptr;
	std::vector<int> _shape; // 高维矩阵的形状
	std::vector<int> _stride;
	DType _dtype; // 内存数据真实类型
	int _size; // 内存矩阵的元素个数
	//std::shared_ptr<P3DObject> _dataHolder;
	//struct DataHolder;
	NdArray() {}

public:
	// 构造函数1: 需要传入dtype和矩阵的形状
	NdArray(DType dtype, std::initializer_list<int> shape, bool init_to_0);
	NdArray(DType dtype, const std::vector<int>& shape, bool init_to_0);
	// 构造函数2: 矩阵复制
	NdArray(NdArray& arr);
	NdArray& operator=(const NdArray& arr) = delete; // copy assignment
	NdArray* shallowCopy() const;

	// 析构函数
	~NdArray();

	friend class NdUtils;

	// 获得矩阵相关属性
	int ndim() const {return (int)_shape.size();} // 获得矩阵的维数
	const std::vector<int>& shape() const {return _shape;} // 获得矩阵的形状
	std::vector<int> strides() const {
		auto sz = DTypeUtil::getSize(_dtype);
		if(sz<=0) return {};
		std::vector<int> _bstrides = _stride;
		for(auto& i : _bstrides)
			i *= sz;
		return _bstrides;
	} // 获得矩阵的形状
	const std::vector<int>& itemStrides() const {return _stride;} // 获得矩阵的形状
	DType dtype() const {return _dtype;} // 获得原始数据类型
	char* rawData() {return _data->rawData<char>();} // 获得矩阵的原始数据
	const char* rawData() const { return _data->rawData<const char>(); }
	int size() const {return _size;} // 获得内存矩阵的元素个数
	int nbytes() const;
	template<typename TVec>
	int calcOffsetT(const TVec& index) const;
	int calcOffset(const std::initializer_list<int>& index) const;
	int calcOffset(const std::vector<int>& index) const;
	int countNonzero() const;
	float floatAtOffset(int offset); // 获得索引元素，index在[0, size-1]之间
	double doubleAtOffset(int offset); // 获得索引元素，index在[0, size-1]之间
	void setDoubleAtOffset(int offset, double v); // 获得索引元素，index在[0, size-1]之间
	// 矩阵自身相关操作
	void doReshape(std::initializer_list<int> dims); // reshape操作，不需要进行内存拷贝
	void doReshape(std::vector<int> new_shape); // reshape操作，不需要进行内存拷贝
	void doFlip(int axis); // 矩阵翻转
	void doExpandDims(int axis);
	void doSqueeze(int axis);
	void doSqueeze(const std::vector<int>& axes);
	void doMultiply(double v);
	// 矩阵inplace的element-wise运算，执行完后this数据被更改
	// 目前如果两个数组dtype不一样，则报错
	// 目前没有做任何clip，除零检测等特性
	void plus(NdArray* arr1); 
	void plus(float c);
	void subtract(NdArray* arr1);
	void subtract(float c);
	void times(NdArray* arr1);
	void times(float c);
	void divide(NdArray* arr1);
	void divide(float c);
	void power(float c);
	void poweredby(float c);
	void exp();
	void logarithm();
	void doNormalize(double ord);
	pDataBlock getDataBlock() { return _data; }

	// 以float index前两维做双线性插值
	template<typename T> T index(float d0_idx, float d1_idx, int offset);

	// 矩阵IO
	void dump(const char* fd);
	void load(const char* fd);

private:

	void _calcSize(); // 获得内存矩阵的元素个数
	void _malloc_memory(bool init_to_0); // 分配内存
	void _calcStride();

	// 辅助函数
	template<typename T> void _flip(int axis);
	template<typename T> void _plus(NdArray* arr1);
	template<typename T> void _plus(float c);
	template<typename T> void _subtract(NdArray* arr1);
	template<typename T> void _subtract(float c);
	template<typename T> void _times(NdArray* arr1);
	template<typename T> void _times(float c);
	template<typename T> void _divide(NdArray* arr1);
	template<typename T> void _divide(float c);
	template<typename T> void _power(float c);
	template<typename T> void _poweredby(float c);
	template<typename T> void _logarithm();

	friend class PNdArray;

	template <typename T>
	friend class PNdArrayT;
};

class PNdArray : public P3DZeroObject {
protected:
	std::shared_ptr<NdArray> ptr;
public:
	PNdArray(DType dtype, const std::vector<int>& shape, bool init_to_0) : ptr(new NdArray(dtype, shape, init_to_0))
	{}
	PNdArray(NdArray* arr = nullptr) : ptr(arr)
	{}
	PNdArray subArray(const std::vector<std::vector<int>>& range) const;
	double doubleAt(const std::initializer_list<int>& index) const {
		auto offset = ptr->calcOffset(index);
		return ptr->doubleAtOffset(offset);
	}
	double doubleAt(const std::vector<int>& index) const {
		auto offset = ptr->calcOffset(index);
		return ptr->doubleAtOffset(offset);
	}
	void setDoubleAt(const std::initializer_list<int>& index, double v) {
		auto offset = ptr->calcOffset(index);
		ptr->setDoubleAtOffset(offset, v);
	}
	void setDoubleAt(const std::vector<int>& index, double v) {
		auto offset = ptr->calcOffset(index);
		ptr->setDoubleAtOffset(offset, v);
	}
	pDataBlock getDataBlock() { return ptr->getDataBlock(); }
	NdArray* get() const { return ptr.get(); }
	char* rawData() { return (char*)ptr->rawData(); }
	int nbytes()const { return ptr->nbytes(); }
	const char* rawData() const { return (const char*)ptr->rawData(); }
	bool empty() const { return ptr == nullptr; }
	DType dtype() const { return ptr->dtype(); }
	int ndim() const { return ptr->ndim(); }
	void doFlip(int axis) { get()->doFlip(axis); }
	void doFliplr() { get()->doFlip(1); }
	const std::vector<int>& shape() const { return ptr->shape(); }
	std::vector<int> strides() const { return ptr->strides(); }
	int countNonzero() const {return ptr->countNonzero();}
	void doReshape(std::initializer_list<int> shape) { ptr->doReshape(shape); }
	void doReshape(const std::vector<int>& shape) { ptr->doReshape(shape); }
	void doExpandDims(int axis) { ptr->doExpandDims(axis); }
	void doSqueeze(int axis) {ptr->doSqueeze(axis);}
	void doSqueeze(const std::vector<int>& axes) { ptr->doSqueeze(axes);}
	void doMultiply(double val) {ptr->doMultiply(val);}
	void doNormalize(double ord) {ptr->doNormalize(ord);}
	int size() const { return ptr->size(); }
	int dimSize(int index) const { return ptr->_shape[index]; }
	bool isCompact() const { return true; }
	PNdArray shallowCopy() const { return PNdArray(get()->shallowCopy()); }
	template <typename T>
	friend class PNdArrayT;
	template <typename T> 
	PNdArrayT<T> asType() const;
	template <typename T>
	PNdArrayT<T> cast() const;

	inline PNdArrayB asBool() const;
	inline PNdArrayF asFloat() const;
	inline PNdArrayI asInt() const;
	inline PNdArrayD asDouble() const;
};

class NdUtils
{
public:
	static PNdArray subArray(NdArray* arr, const std::vector<std::vector<int>>& range);
	// 矩阵element-wise运算，执行完后两个输入矩阵保持不变，算法会新开内存，构造一个新的矩阵返回
	// 目前如果两个数组dtype不一样，则报错
	// 目前没有做任何clip，除零检测等特性
	static PNdArray plus(NdArray* arr1, NdArray* arr2); // return arr1 + arr2
	static PNdArray plus(NdArray* arr1, float c); // return arr1 + c
	static PNdArray subtract(NdArray* arr1, NdArray* arr2); // return arr1 - arr2
	static PNdArray subtract(NdArray* arr1, float c); // return arr1 - c
	static PNdArray times(NdArray* arr1, NdArray* arr2); // return arr1 * arr2
	static PNdArray times(NdArray* arr1, float c); // return arr1 * c
	static PNdArray divide(NdArray* arr1, NdArray* arr2); // return arr1 / arr2
	static PNdArray divide(NdArray* arr1, float c); // return arr1 / c
	static PNdArray power(NdArray* arr1, float c); // return arr1^c
	static PNdArray poweredby(float c, NdArray* arr1); // return c^arr1
	static PNdArray exp(NdArray* arr1); // return e^arr1
	static PNdArray logarithm(NdArray* arr1); // return log(arr1)
	static PNdArray softmax(NdArray* arr1); // return softmax(arr1)
	static PNdArray arange(int start, int stop, DType type);
	template<typename T>
	static PNdArrayT<T> arange(int start, int stop);
	static int argmax(NdArray* arr1);
	static int argmin(NdArray* arr1);

	// 矩阵转置，目前该函数只能转置前两维
	static PNdArray transpose(NdArray* arr1); //return transposed ndarray
//	static PNdArray imageRM(const NdArray* arr1, ImageRM rm); //arr->rm

	// 矩阵乘法，目前该函数只支持二维矩阵
	static PNdArray matmul(NdArray* arr1, NdArray* arr2); //return arr1*arr2

	// 参考cv2.warpAffine, defVal为padding时的默认填充值
	// 目前transmat.shape = (2, 3), 其余不处理，输出ndarray的shape=(dsize.height, dsize.width), 算法会返回新内存对象
	// 对应关系为： 设原图点为(x1, y1), x1~[0, arr1.shape[1]-1] y1~[0, arr1.shape[0]-1] , 
	//            结果图对应点为(x2, y2) ，x2~[0, dsize.width-1] y2~[0, dsize.height-1]
	//            x2 = x1 * transmat[0][0] + y1 * transmat[0][1] + transmat[0][2];
	//            y2 = x1 * transmat[1][0] + y1 * transmat[1][1] + transmat[1][2];
	//static PNdArray warpAffine(NdArray* arr1, NdArray* transmat, Size dsize, int defVal);
      //  static PNdArray warpAffine2(NdArray* arr1, NdArray* transmat, Size dsize, int defVal); 
		
	static PNdArrayF toFloat(NdArray* arr1);
	static PNdArrayI toInt(NdArray* arr1);

	// 参考tf.image.crop_and_resize，rect 0~1 
	// 目前默认rect的长度为4，crop只考虑arr1的前两个维度，第一个维度的(rect[0], rect[2])被crop出来，第二个维度的(rect[1], rect[3])被crop出来
	// 目前cropSize是图像xy， .width和arr1第二个维度对应, .height和arr1第一个维度对应, 输出ndarray的shape=(dsize.height, dsize.width), 算法会返回新内存对象
	/*
	static PNdArray cropAndResize(NdArray* arr1, const std::vector<float>& rect, Size cropSize, float defVal);

	static PNdArray cropAndResize(NdArray* arr1, const Rect& rect, Size cropSize, float defVal);

	static PNdArray cropAndResize(NdArray* arr1, const RectF& rect, Size cropSize, float defVal);

	static PNdArray resize(NdArray* arr1, const Size& newSize);
	*/
	static PNdArrayU8 toUInt8Image(NdArray* arr1);

	static PNdArray stack(const std::vector<NdArray*>& arr_lst);
	static PNdArray stack(const std::vector<PNdArray>& arr_lst);
	static PNdArray stack(const std::vector<NdArray*>& arr_lst, int n);
	static PNdArray stack(const std::vector<PNdArray>& arr_lst, int n);
	static PNdArray concat(const std::vector<PNdArray>arr_lst);
	static PNdArray concat(const std::vector<NdArray*>& arr_lst);
	template<typename T>
	static PNdArrayT<T> stack(const std::vector<PNdArrayT<T>>& arr_lst, int n = -1) {
		if (n < 0)
			n = int(arr_lst.size());
		std::vector<NdArray*> arr_lst1(arr_lst.size());
		for (size_t i = 0; i < arr_lst.size(); i++)
			arr_lst1[i] = arr_lst[i].get();
		return stack(arr_lst1, n).asType<T>();
	}

	// 矩阵计算广义逆
	static PNdArray pinv(NdArray* M);

	static void print(std::ostream& stream, const PNdArray& M);
	static void print(const PNdArray& M);
	static std::string sprint(const PNdArray& M, bool fixed = false, int precision = 0);

	template<typename T>
	static PNdArrayT<T> createArray(const std::vector<std::vector<T>>& data);

	template<typename T>
	static PNdArrayT<T> createArray(const std::vector<T>& data);

	template <typename T, size_t M, size_t N>
	static PNdArrayT<T> createNdArray(const T(&data)[M][N]);

	template<typename T>
	static PNdArrayT<T> ones(const std::vector<int>& shape);

	template<typename T>
	static PNdArrayT<T> full(const std::vector<int>& shape, T fillValue);

	template<typename T, int size>
	static std::array<T, size> toArray1(const NdArray* arr);

	//template<typename T, int Rows, int Cols>
	//static PNdArrayT<T> toNdArray(const Eigen::Matrix<T,Rows,Cols>& m);

	//debug utils
	static void dumpPlainData(const std::string& fpath, const NdArray* M);

	//image convert utils
	static PNdArrayU8 toU8Image(const PNdArrayF& img);
	static PNdArrayU8 toU8C3Image(const PNdArrayF& img);

	//IO
	static void writeNpy(const std::string& fpath, const PNdArray& arr);
	static void writeNpz(const std::string& fpath, const std::unordered_map<std::string, PNdArray>& arr);
	static PNdArray readNpy(const std::string& fpath);
	static PNdArray readNpy(const std::vector<char>& data);
	static std::unordered_map<std::string, PNdArray> readNpz(const std::string& fpath);
	static std::unordered_map<std::string, PNdArray> readNpz(const std::vector<char>& data);
private:
	// 辅助函数
	template<typename T> static PNdArray _subArray(const NdArray* arr, const std::vector<std::vector<int>>& range);
	template<typename T> static PNdArray _plus(NdArray* arr1, NdArray* arr2);
	template<typename T> static PNdArray _plus(NdArray* arr1, float c);
	template<typename T> static PNdArray _subtract(NdArray* arr1, NdArray* arr2);
	template<typename T> static PNdArray _subtract(NdArray* arr1, float c);
	template<typename T> static PNdArray _times(NdArray* arr1, NdArray* arr2);
	template<typename T> static PNdArray _times(NdArray* arr1, float c);
	template<typename T> static PNdArray _divide(NdArray* arr1, NdArray* arr2);
	template<typename T> static PNdArray _divide(NdArray* arr1, float c);
	template<typename T> static PNdArray _power(NdArray* arr1, float c);
	template<typename T> static PNdArray _poweredby(float c, NdArray* arr1);
	template<typename T> static PNdArray _logarithm(NdArray* arr1);
	template<typename T> static PNdArray _softmax(NdArray* arr1);
	template<typename T> static PNdArray _transpose(NdArray* M);
//	template<typename T> static PNdArray _imageRM(const NdArray* M, ImageRM rm);
	template<typename T> static PNdArray _matmul(NdArray* arr1, NdArray* arr2);
//	template<typename T> static PNdArray _warpAffine(NdArray* arr1, NdArray* transmat, Size dsize, int defVal);
//        template<typename T> static PNdArray _warpAffine_opt(NdArray* arr1, NdArray* transmat, Size dsize, int defVal);
//	template<typename T> static PNdArray _cropAndResize(NdArray* arr1, const std::vector<float>& rect, Size cropSize, float defVal);
	template<typename T> static PNdArray _pinv(NdArray* M);
};

template <typename T>
constexpr DType Type2NdTypeId() { return DType::NoType; }

template <>
constexpr DType Type2NdTypeId<float>() { return DType::Float32; }

template <>
constexpr DType Type2NdTypeId<int>() { return DType::Int32; }

template <>
constexpr DType Type2NdTypeId<uint8_t>() { return DType::UInt8; }

template <>
constexpr DType Type2NdTypeId<int64_t>() { return DType::Int64; }

template <>
constexpr DType Type2NdTypeId<bool>() { return DType::Bool; }

template <>
constexpr DType Type2NdTypeId<short>() { return DType::Int16; }

template <>
constexpr DType Type2NdTypeId<double>() { return DType::Float64; }

template <typename T>
class PNdArrayT : public PNdArray {
public:
	explicit PNdArrayT(const PNdArray& arr) {
		if (arr.empty()) {
			ptr = nullptr;
			return;
		}
		if (arr.get()->_dtype != Type2NdTypeId<T>())
			mmu_ndarray_throw_error("type mismatch");
		ptr = arr.ptr;
	}
	PNdArrayT(NdArray* arr) {
		if (arr == nullptr) return;
		if (arr->_dtype != Type2NdTypeId<T>())
			mmu_ndarray_throw_error("type mismatch");
		ptr.reset(arr);
	}
	PNdArrayT() {}
	PNdArrayT(const std::vector<int>& shape, bool init_to_0 = true) : PNdArray(new NdArray(Type2NdTypeId<T>(), shape, init_to_0))
	{}
	PNdArrayT clone() const {
		return PNdArrayT(new NdArray(*ptr.get()));
	}
	PNdArrayT t() const {
		auto d1 = NdUtils::transpose(ptr.get());
		return d1.template asType<T>();
	}
	PNdArrayT matmul(const PNdArrayT<float>& arr2) const{
		return NdUtils::matmul(get(), arr2.get()).template asType<T>();
	}
//	PNdArrayT imageRM(ImageRM rm) const {
//		return NdUtils::imageRM(get(), rm).template asType<T>();
//	}
	T* data() { return get()->_data->template rawData<T>(); }
	PNdArrayT pinv() const { return PNdArrayT(NdUtils::pinv(get())); }

	//void doMultiply(T val) {
	//	auto* d = data();
	//	int sz = size();
	//	for (int i = 0; i < sz; i++)
	//		d[i] *= val;
	//}

	//deep copy
	PNdArrayT flip(int axis) const {
		PNdArrayT arr = clone();
		arr.doFlip(axis);
		return arr;
	}
	//deep copy
	PNdArrayT fliplr() const {return flip(1);}
	//shallow copy
	PNdArrayT reshape(const std::vector<int>& newShape) const {
		auto arr = shallowCopy();
		arr.doReshape(newShape);
		return arr;
	}
	//shallow copy
	PNdArrayT reshape(std::initializer_list<int> newShape) const {
		auto arr = shallowCopy();
		arr.doReshape(newShape);
		return arr;
	}
	//shallow copy
	PNdArrayT squeeze(int axis) const {
		PNdArrayT arr = shallowCopy();
		arr.doSqueeze(axis);
		return arr;
	}
	PNdArrayT expandDims(int axis) const {
		PNdArrayT arr = shallowCopy();
		arr.doExpandDims(axis); 
		return arr;
	}
	const T* data() const { return get()->_data->template rawData<T>(); }
	const T & operator [](std::initializer_list<int> index) const{
		int offset = get()->calcOffset(index);
		return (get()->_data->template rawData<T>())[offset];
	}
	T & operator [](std::initializer_list<int> index) {
		int offset = get()->calcOffset(index);
		return (get()->_data->template rawData<T>())[offset];
	}
	PNdArrayT shallowCopy() const {
		return PNdArrayT(get()->shallowCopy());
	}
	std::vector<T> toVector1() const {
		auto d = data();
		return std::vector<T>(d, d + size());
	}
	template<int size>
	std::array<T, size> toArray1() const {
		std::array<T, size> a;
		memcpy(a.data(), data(), ptr->nbytes());
		return a;
	}
	template <typename T1>
	PNdArrayT<T1> cast() const {
		if (typeid(T1) == typeid(T))
			return asType<T1>();
		PNdArrayT<T1> arr(shape());
		int sz = size();
		const T* src = data();
		T1* dst = arr.data();
		for (int i = 0; i < sz; i++) {
			dst[i] = (T1)src[i];
		}
		return arr;
	}
	template <typename T1>
	bool isType() const {
		return get()->_dtype == Type2NdTypeId<T>();
	}
/*
	Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> toEigen() const {
		auto& _shape = shape();
		if(_shape.size() != 2)
			THROW_RUNTIME("Invlaid matrix shape");
		int nrows = _shape[0], ncols = _shape[1];
		Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> m(nrows, ncols);
		auto psrc = (const T*)get()->rawData();
		auto pdst = m.data();
		auto dstride0 = m.innerStride();
		auto dstride = m.outerStride();
		for(int i=0;i<nrows;i++) {
			pdst = m.data()+dstride0*i;
			for(int j=0;j<ncols;j++) {
				*pdst = *(psrc++); //todo: support multi stride
				pdst += dstride;
			}
		}
		return m;
	}
	*/
};

template<typename T>
inline PNdArrayT<T> NdUtils::createArray(const std::vector<std::vector<T>>& data) {
	if (data.empty())
		throw std::invalid_argument("data cannot be empty");
	int m = (int)data.size();
	int n = (int)data[0].size();
	if (n == 0)
		throw std::invalid_argument("dim1 cannot be 0");
	for (int i = 1; i < m; i++) {
		if ((int)data[i].size() != n)
			throw std::invalid_argument("dim1 mismatch");
	}
	PNdArrayT<T> arr({ m,n });
	T* d = arr.data();
	for (int i = 0; i < m; i++)for (int j = 0; j < n; j++) {
		*(d++) = data[i][j];
	}
	return arr;
}

template<typename T>
inline PNdArrayT<T> NdUtils::createArray(const std::vector<T>& data) {
	if (data.empty())
		throw std::invalid_argument("data cannot be empty");
	int m = (int)data.size();
	PNdArrayT<T> arr({ m });
	T* d = arr.data();
	for (int i = 0; i < m; i++) {
		*(d++) = data[i];
	}
	return arr;
}

template <typename T, size_t M, size_t N>
inline PNdArrayT<T> NdUtils::createNdArray(const T(&data)[M][N]) {
	auto size = sizeof(data);
	//auto tSize = sizeof(T);
	//assert(size == M * N * tSize);
	PNdArrayT<T> arr({ M, N }, false);
	memcpy(arr.data(), data, size);
	return arr;
}

template<typename T>
inline PNdArrayT<T> NdUtils::ones(const std::vector<int>& shape) {
	return full<T>(shape, 1);
}

template<typename T>
inline PNdArrayT<T> NdUtils::full(const std::vector<int>& shape, T fillValue) {
	PNdArrayT<T> arr(shape);
	int m = arr.size();
	T* d = arr.data();
	for (int i = 0; i < m; i++) {
		*(d++) = fillValue;
	}
	return arr;
}

template<typename T, int size>
inline std::array<T, size> NdUtils::toArray1(const NdArray* arr) {
	int arrSize = arr->size();
	if (arrSize != size)
		throw std::invalid_argument("array size not match");
	std::array<T, size> rlt;
	memcpy(rlt.data(), arr->rawData(), size * sizeof(T));
	return rlt;
}
/*
template<typename T, int Rows, int Cols>
inline PNdArrayT<T> NdUtils::toNdArray(const Eigen::Matrix<T, Rows, Cols>& m) {
	int rows = (int)m.rows();
	int cols = (int)m.cols();
	PNdArrayT<T> arr({ rows, cols });
	assert(arr.isCompact());
	T* p = arr.data();
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			*(p++) = m(i, j);
	}
	return arr;
}
*/
template <typename T>
inline PNdArrayT<T> PNdArray::asType() const{
	return PNdArrayT<T>(*this);
}

template<typename T>
inline PNdArrayT<T> NdUtils::arange(int start, int stop) {
	return arange(start, stop, DTypeUtil::toDType<T>()).template asType<T>();
}


	template <typename T>
inline PNdArrayT<T> PNdArray::cast() const {
	auto _dtype = ptr->_dtype;
	if (_dtype == DType::UInt8) return asType<unsigned char>().cast<T>();
	else if (_dtype == DType::Int16) return asType<int16_t>().cast<T>();
	else if (_dtype == DType::Int32) return asType<int>().cast<T>();
	else if (_dtype == DType::Int64) return asType<int64_t>().cast<T>();
	else if (_dtype == DType::Float32) return asType<float>().cast<T>();
	else if (_dtype == DType::Float64) return asType<double>().cast<T>();
	else
		throw std::invalid_argument(Utils::makeStr("unsupported dtype %s for cast", DTypeUtil::toString(_dtype).c_str()));
}

inline PNdArrayB PNdArray::asBool() const {
	return asType<bool>();
}

inline PNdArrayF PNdArray::asFloat() const {
	return asType<float>();
}

inline PNdArrayI PNdArray::asInt() const {
	return asType<int>();
}

inline PNdArrayD PNdArray::asDouble() const {
	return asType<double>();
}

END_P3D_NS
