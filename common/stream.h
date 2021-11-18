#pragma once
#include "lcom.h"
#include "P3D_Utils.h"
BEGIN_P3D_NS
class InputStream : public P3DObject {
public:
	virtual void close() {}
	virtual void read(char* buf, int size) = 0;
	template<typename T>
	T readT() {
		T v;
		read((char*)&v, sizeof(v));
		return v;
	}
};

class OutputStream : public P3DObject {
public:
	virtual void close() {}
	virtual void flush() {}
	virtual void write(const char* data, int size) = 0;
	template<typename T>
	void writeT(const T& v) {
		write((const char*)&v, sizeof(v));
	}
};

class FileInputStream : public InputStream {
public:
	FileInputStream(cstring fpath);
	void read(char* buf, int size) override;
private:
	std::ifstream fin;
};

class FileOutputStream : public OutputStream {
public:
	FileOutputStream(cstring fpath);
	void close() override;
	void flush() override;
	void write(const char* data, int size) override;
private:
	std::ofstream fout;
};

class MemoryOutputStream : public OutputStream {
public:
	MemoryOutputStream(ByteBuffer& bufin): _buffer(bufin)
	{}
	ByteBuffer& buffer() { return _buffer; }
	void write(const char* data, int size) override;
private:
	ByteBuffer& _buffer;
};

END_P3D_NS
