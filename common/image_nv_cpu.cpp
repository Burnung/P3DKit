#include <math.h>
#include <array>
#include "Image.h"
#include "NdArray.h"
#include "imagerm_coord_utils.h"
#include "image_impl_cpu.h"

using namespace std;
BEGIN_P3D_NS

struct ImageYUVUtil {
	static void toRGB(int y, int u, int v, int* r, int* g, int *b) {
//		y <<= 8;
		u -= 128;
		v -= 128;
//		int _r = y + 359*v;
//		int _g = y - 88*u - 183*v;
//		int _b = y + 454*u;
//		*r = _r > 0 ? _r >> 8 : 0;
//		*g = _g > 0 ? _g >> 8 : 0;
//		*b = _b > 0 ? _b >> 8 : 0;
        
        *r =
            clamp(1.164 * (y - 16) + 1.596 * v, 0.0, 255.0);
        *g =
            clamp(1.164 * (y - 16) - 0.391 * u - 0.813 * v, 0.0, 255.0);
        *b =
            clamp(1.164 * (y - 16) + 2.018 * u, 0.0, 255.0);
	}
};

template<typename trait>
inline typename trait::PixelBlock _get_img_color(uint8_t* y_data, uint8_t* uv_data, int width, int height,
                                                 int x, int y, float defval) {
    typename trait::PixelBlock block = trait::init_pixelblock(defval);
    if (x < 0 || x >= width || y < 0 || y >= height)
        return block;

    uint8_t* y_ptr = y_data + y*width + x;
    uint8_t* uv_ptr = uv_data + (y>>1)*width + ((x>>1)<<1);

    int v, u;
    trait::loaduv(&u, &v, uv_ptr);
    trait::store_pixel_block(block, *y_ptr, u, v);
    return block;
}

template<typename trait>
PNdArray nv_crop_to_array_general_t(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	Parallelogram rect1, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod) {
	typedef typename trait::TDst TDst;
	if (src_format != IMAGE_FORMAT_NV21 && src_format != IMAGE_FORMAT_NV12) {
		throw invalid_argument("only support nv21/nv12");
	}
	if (dstFormat != IMAGE_FORMAT_RGB_NORM_FLOAT && dstFormat != IMAGE_FORMAT_BGR_NORM_FLOAT &&
		dstFormat != IMAGE_FORMAT_RGB && dstFormat != IMAGE_FORMAT_BGR &&
		dstFormat != IMAGE_FORMAT_YUV0) {
		throw invalid_argument("currently only support rgb/bgr/rgb-norm/bgr-norm/yuv0 float");
	}
	//full length
	auto lvx = rect1.lvx;
	auto lvy = rect1.lvy;
	//step length
	int dwidth = dstSize.width;
	int dheight = dstSize.height;
	auto vx = lvx / float(dwidth);
	auto vy = lvy / float(dheight);
	//start point
	auto sp = rect1.startPos + (vx + vy) / 2.0f;
	PNdArrayT<TDst> rlt({ dheight, dwidth, trait::bytes_per_pixel+trait::pad }, false);
	auto prlt = rlt.data();
	//cache
	int _src_width = _src_size.width;
	int _src_height = _src_size.height;
	// source data
	uint8_t* y_data = src_data;
	uint8_t* uv_data = src_data + _src_width * _src_height;
    float defval;
    if (Image::isDefaultFillValue(defFillValue))
        defval = trait::get_defval();
    else
        defval = (float)defFillValue;
	if (interMethod == IMAGE_INTERP_BILINEAR) {
		for (int y = 0; y < dheight; y++) {
			Vec2f cp = sp + vy * float(y);
			for (int x = 0; x < dwidth; x++) {
				int x0 = (int)floorf(cp.x);
				int y0 = (int)floorf(cp.y);
				int x1 = x0 + 1;
				int y1 = y0 + 1;
				/*00 10
				01 11*/
				typename trait::PixelBlock c00 = _get_img_color<trait>(y_data, uv_data, _src_width, _src_height,
                                                                       x0, y0, defval);
				typename trait::PixelBlock c10 = _get_img_color<trait>(y_data, uv_data, _src_width, _src_height,
                                                                       x1, y0, defval);
				typename trait::PixelBlock c01 = _get_img_color<trait>(y_data, uv_data, _src_width, _src_height,
                                                                       x0, y1, defval);
				typename trait::PixelBlock c11 = _get_img_color<trait>(y_data, uv_data, _src_width, _src_height,
                                                                       x1, y1, defval);
				float a = cp.x - x0;
				float b = cp.y - y0;
				float a1 = 1 - a;

				float p0, p1;
				for (int cn = 0; cn < trait::bytes_per_pixel; cn++) {
					p0 = c00[cn] * a1 + c10[cn] * a;
					p1 = c01[cn] * a1 + c11[cn] * a;
					*(prlt++) = TDst(p0 + b * (p1 - p0));
				}
				for (int cn = 0; cn < trait::pad; cn++)
					*(prlt++) = 0;
				cp += vx;
			}
		}
	}
	else if (interMethod == IMAGE_INTERP_NN) {
		for (int y = 0; y < dheight; y++) {
			Vec2f cp = sp + vy * float(y);
			for (int x = 0; x < dwidth; x++) {
				int x0 = roundi(cp.x);
				int y0 = roundi(cp.y);
				typename trait::PixelBlock c00 = _get_img_color<trait>(y_data, uv_data, _src_width, _src_height,
                                                                       x0, y0, defval);
				for (int cn = 0; cn < trait::bytes_per_pixel; cn++)
					*(prlt++) = (TDst)c00[cn];
				for (int cn = 0; cn < trait::pad; cn++)
					*(prlt++) = 0;
				cp += vx;
			}
		}
	}
	else {
		throw invalid_argument("unknown interpolation method");
	}
	return rlt;
}

struct NV21LoadVU {
	static void loaduv(int* u, int* v, const uint8_t* uv) {
		*v = *uv++;
		*u = *uv;
	}
};

struct NV12LoadVU {
	static void loaduv(int* u, int* v, const uint8_t* uv) {
		*u = *uv++;
		*v = *uv;
	}
};

struct NVtoRGBNorm {
public:
	enum { bytes_per_pixel = 3, pad = 0};
	typedef float TDst;
	typedef array<float, 3> PixelBlock;
	static float get_defval() {
		return 0.0;
	}
	static PixelBlock const init_pixelblock(float defval = 0.0) {
		return array<float, 3>({defval, defval, defval});
	}
	static void store_pixel_block(PixelBlock& block, int y, int u, int v) {
		int r, g, b;
		ImageYUVUtil::toRGB(y, u, v, &r, &g, &b);
		block[0] = (float(r) - 127.0f) / 128.0f;
		block[1] = (float(g) - 127.0f) / 128.0f;
		block[2] = (float(b) - 127.0f) / 128.0f;
	}
};

struct NV21toRGBNorm : NVtoRGBNorm, NV21LoadVU {
};

struct NV12toRGBNorm : NVtoRGBNorm, NV12LoadVU {
};

struct NVtoRGB {
public:
	enum { bytes_per_pixel = 3, pad = 0};
	typedef uint8_t TDst;
	typedef array<float, 3> PixelBlock;
    static float get_defval() {
        return 127.0;
    }
	static PixelBlock const init_pixelblock(float defval = 127.0) {
		return array<float, 3>({ defval, defval, defval });
	}
	static void store_pixel_block(PixelBlock& block, int y, int u, int v) {
		int r, g, b;
		ImageYUVUtil::toRGB(y, u, v, &r, &g, &b);
		block[0] = float(r & 0xff);
		block[1] = float(g & 0xff);
		block[2] = float(b & 0xff);
	}
};

struct NV21toRGB : NVtoRGB, NV21LoadVU {
};

struct NV12toRGB : NVtoRGB, NV12LoadVU {
};

struct NVtoYUV0 {
public:
	enum { bytes_per_pixel = 3, pad = 1};
	typedef uint8_t TDst;
	typedef array<float, 4> PixelBlock;
	static float get_defval() {
		return 127.0;
	}
	static PixelBlock const init_pixelblock(float defval = 127.0) {
		return array<float, 4>({ defval, defval, defval, 0});
	}
	static void store_pixel_block(PixelBlock& block, int y, int u, int v) {
		block[0] = float(y & 0xff);
		block[1] = float(u & 0xff);
		block[2] = float(v & 0xff);
	}
};

struct NV21toYUV0 : NVtoYUV0, NV21LoadVU {
};

struct NV12toYUV0 : NVtoYUV0, NV12LoadVU {
};

struct NVtoBGRNorm {
	enum { bytes_per_pixel = 3, pad = 0};
	typedef float TDst;
	typedef array<float, 3> PixelBlock;
	static float get_defval() {
		return 0.0;
	}
	static PixelBlock const init_pixelblock(float defval = 0.0) {
		return array<float, 3>({defval, defval, defval});
	}
	static void store_pixel_block(PixelBlock& block, int y, int u, int v) {
		int r, g, b;
		ImageYUVUtil::toRGB(y, u, v, &r, &g, &b);
		block[0] = (float(b) - 127.0f) / 128.0f;
		block[1] = (float(g) - 127.0f) / 128.0f;
		block[2] = (float(r) - 127.0f) / 128.0f;
	}
};

struct NV21toBGRNorm : NVtoBGRNorm, NV21LoadVU {
};

struct NV12toBGRNorm : NVtoBGRNorm,  NV12LoadVU {
};

struct NVtoBGR {
	enum { bytes_per_pixel = 3, pad = 0};
	typedef uint8_t TDst;
	typedef array<float, 3> PixelBlock;
    static float get_defval() {
        return 127.0;
    }
	static PixelBlock const init_pixelblock(float defval = 127.0) {
		return array<float, 3>({ defval, defval, defval });
	}
	static void store_pixel_block(PixelBlock& block, int y, int u, int v) {
		int r, g, b;
		ImageYUVUtil::toRGB(y, u, v, &r, &g, &b);
		block[0] = float(b & 0xff);
		block[1] = float(g & 0xff);
		block[2] = float(r & 0xff);
	}
};

struct NV21toBGR : NVtoBGR, NV21LoadVU{
};

struct NV12toBGR : NVtoBGR, NV12LoadVU {
};

#define CROP_IMPL0(SRC_FMT, DST_FMT, TRAIT) \
	if (src_format == SRC_FMT && dstFormat == DST_FMT) \
		return nv_crop_to_array_general_t<TRAIT>(src_data, src_format, _src_size, \
			rect, dstSize, dstFormat, defFillValue, interMethod)

#define CROP_IMPL(SRC_FMT, DST_FMT, TRAIT) else CROP_IMPL0(SRC_FMT, DST_FMT, TRAIT)

PNdArray nv_crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod) {
	//RGB
	CROP_IMPL0(IMAGE_FORMAT_NV21, IMAGE_FORMAT_RGB_NORM_FLOAT, NV21toRGBNorm);
	CROP_IMPL(IMAGE_FORMAT_NV12, IMAGE_FORMAT_RGB_NORM_FLOAT, NV12toRGBNorm);
	CROP_IMPL(IMAGE_FORMAT_NV21, IMAGE_FORMAT_RGB, NV21toRGB);
	CROP_IMPL(IMAGE_FORMAT_NV12, IMAGE_FORMAT_RGB, NV12toRGB);
	//BGR
	CROP_IMPL(IMAGE_FORMAT_NV21, IMAGE_FORMAT_BGR_NORM_FLOAT, NV21toBGRNorm);
	CROP_IMPL(IMAGE_FORMAT_NV12, IMAGE_FORMAT_BGR_NORM_FLOAT, NV12toBGRNorm);
	CROP_IMPL(IMAGE_FORMAT_NV21, IMAGE_FORMAT_BGR, NV21toBGR);
	CROP_IMPL(IMAGE_FORMAT_NV12, IMAGE_FORMAT_BGR, NV12toBGR);
	//YUV0
	CROP_IMPL(IMAGE_FORMAT_NV21, IMAGE_FORMAT_YUV0, NV21toYUV0);
	CROP_IMPL(IMAGE_FORMAT_NV12, IMAGE_FORMAT_YUV0, NV12toYUV0);
	else throw invalid_argument("unsupported dstFormat");
}

PNdArray nv_crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	RMRect rect, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod) {
	auto _size = _src_size.t(_rotation);
	auto rect1 = ImageRM(_rotation, _mirror).inv().apply(_size, rect);
	//full length
	return nv_crop_to_array_general(src_data, src_format, _src_size, rect1.toParallelogram(),
		dstSize, dstFormat, defFillValue, interMethod);
}

PNdArray nv_crop_to_array_general(uint8_t* src_data, IMAGE_FORMAT src_format, Size _src_size,
	IMAGE_ROTATION _rotation, IMAGE_MIRROR _mirror,
	Parallelogram rect, Size dstSize, IMAGE_FORMAT dstFormat,
	double defFillValue, IMAGE_INTERPOLATION interMethod) {
	auto _size = _src_size.t(_rotation);
	auto rect1 = ImageRM(_rotation, _mirror).inv().apply(_size, rect);
	//full length
	return nv_crop_to_array_general(src_data, src_format, _src_size, rect1,
		dstSize, dstFormat, defFillValue, interMethod);
}
END_P3D_NS
