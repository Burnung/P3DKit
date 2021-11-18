#include "image_impl_cpu.h"
#include "NdArray.h"
BEGIN_P3D_NS

template<typename trait>
static PNdArray warp_affine_float_general(const float* src_data, Size _src_size,
                                          std::array<float, 6> transmat, Size dsize)
{
    int cn = trait::bytes_per_pixel;
    std::vector<int> ret_shape = {dsize.height, dsize.width, cn};
    NdArray* _ret = new NdArray(DType::Float32, ret_shape, false);
    float* vs = (float*)(_ret->rawData());

    int x, y;

    float A11 = transmat[0], A12 = transmat[1], A13 = transmat[2];
    float A21 = transmat[3], A22 = transmat[4], A23 = transmat[5];
    
    int srcw = _src_size.width;
    int srch = _src_size.height;
    int src_step = srcw * cn;
    for (y = 0; y < dsize.height; y++) {
        float xs = A12 * y + A13;
        float ys = A22 * y + A23;
        float xe = A11 * (dsize.width - 1) + A12 * y + A13;
        float ye = A21 * (dsize.width - 1) + A22 * y + A23;
        
        if ( (unsigned)((int)xs - 1) < (unsigned) (srcw - 4) &&
            (unsigned)((int)ys - 1) < (unsigned) (srch - 4) &&
            (unsigned)((int)xe - 1) < (unsigned) (srcw - 4) &&
            (unsigned)((int)ye - 1) < (unsigned) (srch - 4)) {
            for (x = 0; x < dsize.width; x++) {
                int ixs = (int) (xs);
                int iys = (int) (ys);
                const float *ptr0 = src_data + src_step * iys + ixs * cn;
                const float *ptr1 = ptr0 + src_step;
                float a =  (xs - ixs);
                float b =  (ys - iys);
                float a1 =  (1.f - a);

                xs += A11;
                ys += A21;

                trait::load(a, a1, b, ptr0,  ptr1, vs);
            }
        } else {
            for (x = 0; x < dsize.width; x++) {
                int ixs = (int) (xs), iys = (int) (ys);
                float  a = (xs - ixs);
                float  b = (ys - iys);
                float  a1 = (1.f - a);
                const float *ptr0, *ptr1;
                xs += A11;
                ys += A21;
                
                if ( (unsigned) iys < (unsigned) (srch - 1)) {
                    ptr0 = src_data + src_step * iys;
                    ptr1 = ptr0 + src_step;
                } else {
                    trait::fill(vs);
                    continue;
                }
                
                if ( (unsigned) ixs < (unsigned) (srcw - 1)) {
                    ptr0 += ixs * cn;
                    ptr1 += ixs * cn;
                    trait::load(a, a1, b, ptr0,  ptr1, vs);
                } else {
                    trait::fill(vs);
                }
            }
        }
    }
    PNdArray ret(_ret);
	return ret;
}

class Float32x3 {
public:
    enum { bytes_per_pixel = 3 };
    static void load(float a, float a1, float b,
                     const float *ptr0, const float *ptr1,
                     float* &vs) {
        float p0, p1;
        p0 = ptr0[0] * a1 + ptr0[3] * a;
        p1 = ptr1[0] * a1 + ptr1[3] * a;
        *vs++ = p0 + b * (p1 - p0);
        
        p0 = ptr0[1] * a1 + ptr0[4] * a;
        p1 = ptr1[1] * a1 + ptr1[4] * a;
        *vs++ = p0 + b * (p1 - p0);
        
        p0 = ptr0[2] * a1 + ptr0[5] * a;
        p1 = ptr1[2] * a1 + ptr1[5] * a;
        *vs++ = p0 + b * (p1 - p0);
    }
    static void fill(float* &vs, float defval = 0.0) {
        *vs++ = defval;
        *vs++ = defval;
        *vs++ = defval;
    }
};

class Float32x1 {
public:
    enum { bytes_per_pixel = 1 };
    static void load(float a, float a1, float b,
                     const float *ptr0, const float *ptr1,
                     float* &vs) {
        float p0, p1;
        p0 = ptr0[0] * a1 + ptr0[1] * a;
        p1 = ptr1[0] * a1 + ptr1[1] * a;
        *vs++ = p0 + b * (p1 - p0);
    }
    static void fill(float* &vs, float defval = 0.0) {
        *vs++ = defval;
    }
};

PNdArray warp_affine_float_general(const float* src_data, Size _src_size, int cn,
                                   std::array<float, 6> transmat, Size dsize)
{
    if (!src_data)
        throw std::invalid_argument("invalid src data");
    
    switch (cn) {
        case 1:
            return warp_affine_float_general<Float32x1>(src_data, _src_size, transmat, dsize);
        case 3:
            return warp_affine_float_general<Float32x3>(src_data, _src_size, transmat, dsize);;
        default:
            throw std::invalid_argument("only support 1/3 channels");
            return nullptr;
    }
}

END_P3D_NS
