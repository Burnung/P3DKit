#include "stream.h"
#include "P3D_Utils.h"
using namespace std;

BEGIN_P3D_NS
//FileInputStream
FileInputStream::FileInputStream(cstring fpath) : fin(fpath, ofstream::binary)
{
	if (!fin)
		P3D_THROW_RUNTIME("cannot read path %s", fpath.c_str());
}
void FileInputStream::read(char* buf, int size) {
	fin.read(buf, size);
	if (!fin) {
		P3D_THROW_RUNTIME("only %d bytes is read", (int)fin.gcount());
	}
}

//FileOutputStream
FileOutputStream::FileOutputStream(cstring fpath) : fout(fpath, ofstream::binary)
{
	if (!fout)
		P3D_THROW_RUNTIME("cannot write to path %s", fpath.c_str());
}

void FileOutputStream::close() {
	fout.close();
}

void FileOutputStream::flush() {
	fout.flush();
}

void FileOutputStream::write(const char* data, int size) {
	fout.write(data, size);
}

//MemoryOutputStream
void MemoryOutputStream::write(const char* data, int size) {
	_buffer.insert(_buffer.end(), data, data + size);
}
END_P3D_NS
