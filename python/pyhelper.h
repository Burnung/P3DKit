namespace pybind11 {
	namespace detail {
		template <> struct type_caster<P3D::Vec2f> {
		public:
			PYBIND11_TYPE_CASTER(P3D::Vec2f, _("Vec2F"));

			/**
			* Conversion part 1 (Python->C++): convert a PyObject into a inty
			* instance or return false upon failure. The second argument
			* indicates whether implicit conversions should be applied.
			*/
			bool load(handle src, bool) {
				/* Extract PyObject from handle */
				PyObject *source = src.ptr();
				std::array<float, 2> arr = { 0 };
				if (!to_float_array(source, arr))
					return false;
				value.x = arr[0];
				value.y = arr[1];
				return true;
			}

			/**
			* Conversion part 2 (C++ -> Python): convert an inty instance into
			* a Python object. The second and third arguments are used to
			* indicate the return value policy and parent object (for
			* ``return_value_policy::reference_internal``) and are generally
			* ignored by implicit casters.
			*/
			static handle cast(P3D::Vec2f src, return_value_policy /* policy */, handle /* parent */) {
				return to_tuple(src.x, src.y);
			}
		};
	
		template <typename T>
		void copy_eles(const T* src, T*dst, int ndim, int* shape, int* srcEStrides) { //dst is packed
			int soffset = 0;
			vector<int> counts(ndim);
			while (true) {
				*(dst++) = src[soffset];
				int i = ndim - 1;
				while (i >= 0) {
					const auto& r = shape[i];
					auto& c = counts[i];
					if (c < shape[i] - 1) {
						soffset += srcEStrides[i];
						c++;
						break;
					}
					else {
						soffset -= c * srcEStrides[i];
						c = 0;
						i--;
					}
				}
				if (i < 0) break;
			}
		}

		struct NdArrayHelper {
			static DType toDType(const py::dtype& tp) {
				if (tp.is(py::dtype::of<std::float_t>())) {
					std::cout << "dtype is float32" << std::endl;
					return DType::Float32;
				}
				else if (tp.is(py::dtype::of<std::double_t>()))
					return DType::Float64;
				else if (tp.is(py::dtype::of<std::uint8_t>()))
					return DType::UInt8;
				else if (tp.is(py::dtype::of<std::int16_t>()))
					return DType::Int16;
				else if (tp.is(py::dtype::of<std::int32_t>())) {
					return DType::Int32;
				}
				else if (tp.is(py::dtype::of<std::uint32_t>()))
					return DType::Uint32;
#ifdef _WIN32
				else if (tp.is(py::dtype::of<std::int64_t>())) {
#else
				else if(tp.is(py::dtype::of<long>())){
#endif
					std::cout << "dtype is int64" << std::endl;
					return DType::Int64;
				}
				else {
					std::cout << "dtype is notype" << std::endl;
					return DType::NoType;
				}
			}

			static bool toNpType(DType type, py::dtype& d) {
				switch (type) {
				case DType::Float32:
					d = py::dtype::of<std::float_t>();
					return true;
				case DType::Float64:
					d = py::dtype::of<std::double_t>();
					return true;
				case DType::UInt8:
					d = py::dtype::of<std::uint8_t>();
					return true;
				case DType::Int16:
					d = py::dtype::of<std::int16_t>();
					return true;
				case DType::Int32:
					d = py::dtype::of<std::int32_t>();
					return true;
				case DType::Uint32:
					d = py::dtype::of<std::uint32_t>();
					return true;
				case DType::Int64:
					d = py::dtype::of<std::int64_t>();
					return true;
				default: break;
				}
				return false;
			}

			static optional<PNdArray> toNdArray(handle _src) {
				if(!_src) return PNdArray();
				auto src = py::array::ensure(_src);
				if (!src) return nullopt;
				auto dtype = toDType(src.dtype());
				if (dtype == DType::NoType) return PNdArray();
				int ndim = (int)src.ndim();
				vector<int> shape((size_t)ndim), srcEStrides((size_t)ndim);
				for (int i = 0; i<ndim; i++) {
					shape[i] = (int)src.shape(i);
					srcEStrides[i] = int(src.strides(i) / src.dtype().itemsize());
				}
				PNdArray dst(dtype, shape, false);
				switch (dtype) {
				case DType::Float32:
					copy_eles<float>((float*)src.data(), (float*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::Float64:
					copy_eles<double>((double*)src.data(), (double*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::UInt8:
					copy_eles<uint8_t>((uint8_t *)src.data(), (uint8_t*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::Int16:
					copy_eles<int16_t>((int16_t *)src.data(), (int16_t*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::Int32:
					copy_eles<int32_t>((int32_t *)src.data(), (int32_t*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::Uint32:
					copy_eles<uint32_t>((uint32_t *)src.data(), (uint32_t*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				case DType::Int64:
					copy_eles<int64_t>((int64_t *)src.data(), (int64_t*)dst.rawData(), ndim, shape.data(), srcEStrides.data());
					break;
				default: break;
				}
				return dst;
			}

			static handle cast(const PNdArray& src, return_value_policy /* policy */, handle /* parent */) {
				py::dtype nptype;
				if (!toNpType(src.dtype(), nptype))
					return handle();
				py::array nparr(nptype, src.shape(), src.strides());
				//			cout << "shape " << to_string(src.shape()) << " strides " << to_string(src.strides()) <<
				//			    " handle " << nparr.ptr() << endl;
				int nbytes = src.nbytes();
				if (nbytes > 0)
					memcpy((void*)nparr.data(), (void*)src.rawData(), nbytes);
				return nparr.inc_ref();
			}
		};

		template <> struct type_caster<PNdArray> {
		public:
			PYBIND11_TYPE_CASTER(PNdArray, _("PNdArray"));
			bool load(handle _src, bool) {
				auto d = NdArrayHelper::toNdArray(_src);
				if (!d) return false;
				value = d.value();
				return true;
			}
			static handle cast(const PNdArray& src, return_value_policy policy, handle parent) {
				return NdArrayHelper::cast(src, policy, parent);
			}
		};


#define REGISTER_NdArrayT(T, PYNAME) \
    template <> struct type_caster<PNdArrayT<T>> {\
    public:\
    PYBIND11_TYPE_CASTER(PNdArrayT<T>, _(PYNAME));\
        bool load(handle _src, bool) {\
            auto od = NdArrayHelper::toNdArray(_src);\
			if(!od) return false; \
			auto& d = od.value();\
            if(d.empty() || d.dtype() == DTypeUtil::toDType<T>()) {\
                value = d.asType<T>();\
                return true;\
            } else\
                value = d.cast<T>();\
            return true;\
        }\
        static handle cast(const PNdArrayT<T>& src, return_value_policy policy, handle parent) {\
            return NdArrayHelper::cast(src, policy, parent);\
        }\
    }

		REGISTER_NdArrayT(float, "PNdArrayF");
		REGISTER_NdArrayT(double, "PNdArrayD");
		REGISTER_NdArrayT(int, "PNdArrayI");
		REGISTER_NdArrayT(int64, "PNdArrayI64");
		REGISTER_NdArrayT(uint32, "PNdArrayU");
		REGISTER_NdArrayT(uint8_t, "PNdArrayU8");

#define REGISTER_ENUM(TYPE, PY_NAME) \
    template <> struct type_caster<TYPE> { \
        public: \
        PYBIND11_TYPE_CASTER(TYPE, _(PY_NAME)); \
        bool load(handle src, bool) { \
            PyObject *source = src.ptr(); \
            if(!PyLong_Check(source)) \
                return false; \
            auto val = PyLong_AsLong(source); \
            value = (TYPE)val; \
            return true; \
        } \
        static handle cast(TYPE src, return_value_policy /* policy */, handle /* parent */) { \
            return PyLong_FromLong((int)src); \
        } \
    }
		//REGISTER_ENUM(IMAGE_FORMAT, "IMAGE_FORMAT");
	//	REGISTER_ENUM(IMAGE_ROTATION, "IMAGE_ROTATION");
	//	REGISTER_ENUM(IMAGE_INTERPOLATION, "IMAGE_INTERPOLATION");
	

	}
} // namespace pybind11::detail

inline string to_string(const vector<int>& v) {
	string s = "(";
	for (size_t i = 0; i<v.size(); i++) {
		if (i > 0)
			s += ",";
		s += to_string(v[i]);
	}
	s += ")";
	return s;
}
/*
//https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
inline void toEulerAngle(const Eigen::Quaternionf& q, double& rx, double& ry, double& rz)
{
	// roll (x-axis rotation)
	double sinr_cosp = +2.0 * (q.w() * q.x() + q.y() * q.z());
	double cosr_cosp = +1.0 - 2.0 * (q.x() * q.x() + q.y() * q.y());
	rx = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = +2.0 * (q.w() * q.y() - q.z() * q.x());
	if (fabs(sinp) >= 1)
		ry = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
	else
		ry = asin(sinp);

	// yaw (z-axis rotation)
	double siny_cosp = +2.0 * (q.w() * q.z() + q.x() * q.y());
	double cosy_cosp = +1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
	rz = atan2(siny_cosp, cosy_cosp);
}
*/
inline string ensureFindFile(string path, cstring defPath) {
	if (path.empty()) { //default
		path = Utils::getExeDir() + "/" + defPath;
	}
	if (!Utils::isRegularFile(path))
		P3D_THROW_RUNTIME("Cannot find %s", path.c_str());
	return path;
}

inline string ensureFindDir(string path, cstring defPath) {
	if (path.empty()) { //default
		path = Utils::getExeDir() + "/" + defPath;
	}
	if (!Utils::isDir(path))
		P3D_THROW_RUNTIME("Cannot find %s", path.c_str());
	return path;
}
