#pragma once
template<size_t N>
PyObject* to_tuple(const std::array<float, N>& arr) {
	PyObject* t = PyTuple_New(N);
	for (int i = 0; i < N; i++) {
		auto o = PyFloat_FromDouble(arr[i]);
		PyTuple_SetItem(t, i, o);
	}
	return t;
}

template<size_t N>
PyObject* to_tuple(const std::array<int, N>& arr) {
	PyObject* t = PyTuple_New(N);
	for (int i = 0; i < N; i++) {
		auto o = PyLong_FromLong(arr[i]);
		PyTuple_SetItem(t, i, o);
	}
	return t;
}

inline PyObject* to_tuple(float a0, float a1) {
	return to_tuple(std::array<float, 2>({ a0,a1 }));
}

inline PyObject* to_tuple(int a0, int a1) {
	return to_tuple(std::array<int, 2>({ a0,a1 }));
}

template<size_t N>
bool to_float_array(PyObject* p, std::array<float, N>& arr) {
	if (PyTuple_Check(p)) {
		auto sz = PyTuple_Size(p);
		if (sz != N) return false;
		for (size_t i = 0; i < N; i++) {
			PyObject* o = PyTuple_GetItem(p, i); //no ref inc
			if (!PyFloat_Check(o))
				return false;
			arr[i] = (float)PyFloat_AS_DOUBLE(o);
		}
		return true;
	}
	else if (PyList_Check(p)) {
		auto sz = PyList_Size(p);
		if (sz != N) return false;
		for (size_t i = 0; i < N; i++) {
			PyObject* o = PyList_GetItem(p, i);
			if (!PyFloat_Check(o))
				return false;
			arr[i] = (float)PyFloat_AS_DOUBLE(o);
		}
		return true;
	}
	else return false;
}

template<size_t N>
bool to_int_array(PyObject* p, std::array<int, N>& arr) {
	if (PyTuple_Check(p)) {
		auto sz = PyTuple_Size(p);
		if (sz != N) return false;
		for (size_t i = 0; i < N; i++) {
			PyObject* o = PyTuple_GetItem(p, i);
			if (!PyLong_Check(o))
				return false;
			arr[i] = (int)PyLong_AsLong(o);
		}
		return true;
	}
	else if (PyList_Check(p)) {
		auto sz = PyList_Size(p);
		if (sz != N) return false;
		for (size_t i = 0; i < N; i++) {
			PyObject* o = PyList_GetItem(p, i);
			if (!PyLong_Check(o))
				return false;
			arr[i] = (int)PyLong_AsLong(o);
		}
		return true;
	}
	else return false;
}

inline std::array<float, 2> toarr(const P3D::Vec2f& v) {
	return { v.x,v.y };
}
/*
inline std::array<float, 2> toarr(const F& s) {
	return { s.width,s.height };
}*/

inline void fromarr(P3D::Vec2f& v, std::array<float, 2> arr) {
	v.x = arr[0];
	v.y = arr[1];
}
/*
inline void fromarr(SizeF& s, std::array<float, 2> arr) {
	s.width = arr[0];
	s.height = arr[1];
}
*/
//inline PImage toImage(const py::array_t<uint8_t>& srcImg) { //todo, handle any case
//	if (srcImg.ndim() != 3)
//		THROW_RUNTIME("Cannot convert to mmu Image, required ndim=3, provided ndim=%d", srcImg.ndim());
//	if (srcImg.shape()[2] != 3)
//		THROW_RUNTIME("Only RGB image is supported");
//	int h = (int)srcImg.shape()[0], w = (int)srcImg.shape()[1];
//	if (h % 4 != 0 || w % 4 != 0)
//		THROW_RUNTIME("Size not supported, h=%d, w=%d", h, w);
//	auto stride0 = srcImg.strides(0);
//	auto stride1 = srcImg.strides(1);
//	auto stride2 = srcImg.strides(2);
//	if (stride0 <= 0 || stride2 != 1 || (stride1 != 3 && stride1 != -3))
//		THROW_RUNTIME("Invalid stride(%d,%d,%d)", stride0, stride1, stride2);
//	//	std::cout << "shape " << h << " " << w
//	//		      << " stride " << stride0 << " " << stride1 << " " << stride2 << std::endl;
//	if (stride1 == 3) {
//		auto srcData = (uint8_t*)srcImg.data();
//		cv::Mat cvImg(h, w, CV_8UC3, srcData);
//		return CvUtil::toNV21Image(cvImg);
//	}
//	else {
//		cv::Mat cvImg(h, w, CV_8UC3);
//		uint8_t* dst = cvImg.data;
//		uint8_t* src = (uint8_t*)srcImg.data();
//		for (int i = 0; i < h; i++) {
//			auto nextSrc = src + stride0;
//			for (int j = 0; j < w; j++) {
//				*(dst++) = *src;
//				*(dst++) = *(src + 1);
//				*(dst++) = *(src + 2);
//				src -= 3;
//			}
//			src = nextSrc;
//		}
//		return CvUtil::toNV21Image(cvImg);
//	}
//}
