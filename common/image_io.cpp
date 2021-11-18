#include "Image.h"
#include "stream.h"
#include "faceArithBase.h"
using namespace std;

BEGIN_P3D_NS
/*
Image    file format
bytes    type     content
4        int32    IMAG
4        int32    datalen
4        int32    src_width
4        int32    src_height
4        int32    format (IMAGE_FORMAT)
4        int32    rotation (IMAGE_ROTATION)
4        int32    mirror (IMAGE_MIRROR)
datalen  byte     data
*/
int Image::calcDataSize(IMAGE_FORMAT format, int width, int height) {
	int pcount = width*height;
	switch (format) {
	case IMAGE_FORMAT_GREY:
		return pcount;
	case IMAGE_FORMAT_NV21:
	case IMAGE_FORMAT_NV12:
		return pcount * 3 / 2;
	case IMAGE_FORMAT_RGB:
	case IMAGE_FORMAT_BGR:
		return pcount * 3;
	case IMAGE_FORMAT_RGBA:
	case IMAGE_FORMAT_BGRA:
		return pcount * 4;
	default: return 0;
	}
}
int Image::calcDataSize() {
	return calcDataSize(_format, _src_size.width, _src_size.height);
}
static uint32_t Image_bin_magic = uint32_t('I') | (uint32_t('M') << 8) | (uint32_t('A') << 16) | (uint32_t('G') << 24);
void Image::writeBinFile(cstring fpath) {
	int dataSize = calcDataSize();
	if (dataSize <= 0)
		P3D_THROW_RUNTIME("Invalid format (%d) for written", _format);
	FileOutputStream fout(fpath);
	fout.writeT(Image_bin_magic);
	fout.writeT(dataSize);
	fout.writeT(_src_size.width);
	fout.writeT(_src_size.height);
	fout.writeT((int)_format);
	fout.writeT((int)_rotation);
	fout.writeT((int)_mirror);
	fout.write((const char*)_data, dataSize);
}
void Image::readBinFile(cstring fpath) {
	FileInputStream fin(fpath);
	uint32_t magic = fin.readT<uint32_t>();
	if (magic != Image_bin_magic)
		P3D_THROW_RUNTIME("Invalid file head");
	int dataSize = fin.readT<int>();
	if(dataSize <= 0)
		P3D_THROW_RUNTIME("Datasize incorrect");
	int t_src_width = fin.readT<int>();
	int t_src_height = fin.readT<int>();
	int t_format = fin.readT<int>();
	int t_rotation = fin.readT<int>();
	int t_mirror = fin.readT<int>();
	if (!(t_format == IMAGE_FORMAT_RGBA || t_format == IMAGE_FORMAT_NV21 || t_format == IMAGE_FORMAT_NV12
		|| t_format == IMAGE_FORMAT_RGB || t_format == IMAGE_FORMAT_BGRA || t_format == IMAGE_FORMAT_BGR
		|| IMAGE_FORMAT_GREY))
		P3D_THROW_RUNTIME("Invalid format %d", t_format);
	if(!(t_rotation == IMAGE_ROT0 || t_rotation == IMAGE_ROT90_CCW || 
		t_rotation == IMAGE_ROT180 ||
		t_rotation == IMAGE_ROT90_CW))
		P3D_THROW_RUNTIME("Invalid rotation %d", t_rotation);
	if(!(_mirror == IMAGE_MIRROR_NO || _mirror == IMAGE_MIRROR_YES))
		P3D_THROW_RUNTIME("Invalid mirror %d", t_rotation);
	if(calcDataSize((IMAGE_FORMAT)t_format, t_src_width, t_src_height) != dataSize)
		P3D_THROW_RUNTIME("Invalid data size %d", dataSize);
	auto holder = make_shared<DataHolder<vector<char>>>();
	auto& buffer = holder->d;
	buffer.resize(dataSize);
	fin.read(buffer.data(), dataSize);
	//set members
	_dataHolder.reset();
	_data = buffer.data();
	_src_size.width = t_src_width;
	_src_size.height = t_src_height;
	_format = (IMAGE_FORMAT)t_format;
	_rotation = (IMAGE_ROTATION)t_rotation;
	_mirror = (IMAGE_MIRROR)t_mirror;
	_dataHolder = holder;
	calcSize();
}
END_P3D_NS
