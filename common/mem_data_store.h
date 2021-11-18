#pragma once
#include "data_store_interface.h"
#include <map>
BEGIN_P3D_NS
class MemDataStore : public IDataStore {
public:
	explicit MemDataStore() {}
	explicit MemDataStore(IDataStore& source);
	explicit MemDataStore(const std::string& fpath, const char* key=nullptr, int keyLen=0)
	{ loadFile(fpath,key,keyLen); }
	explicit MemDataStore(const std::vector<char>& data, const char* key = nullptr, int keyLen = 0)
	{ loadData(data, key, keyLen); }
	MemDataStore(const char* buf, int bufLen, const char* key = nullptr, int keyLen = 0)
	{ loadData(buf, bufLen, key, keyLen); }
	std::vector<std::string> getPaths(const std::string& pattern) override;
	PBuffer tryGetData(const std::string& path) override;
	void loadData(const char* buf, int bufLen, const char* key, int keyLen);
	void loadData(const std::vector<char>& data, const char* key, int keyLen);
	void loadFile(const std::string& fpath, const char* key, int keyLen);
	std::vector<char> packData(const char* key = nullptr, int keyLen = 0) const;
	void setData(const std::string& path, PBuffer buffer);
	void setData(const std::string& path, const char* data, int dataLen);
	uint64_t getLoadedChecksum() const { return checkSum; }
private:
	std::map<std::string, PBuffer> data;
	uint64_t checkSum = 0;
	static uint64_t calcCheckSum64(const char* data, int dataLen);
	static uint64_t calcCheckSum64(const std::vector<char>& data);
	static uint16_t calcCheckSum16(const char* data, int dataLen);
	static void enc_xor(char* data, int dataLen, const char* key, int keyLen);
	void invalid(const std::string&message);
	template <typename T>
	void checkHeader(T* head);
	//load
	void loadData_xor(const char* buf, int bufLen, const char* key, int keyLen);
	void loadData_direct(const char* buf, int bufLen);
};
END_P3D_NS
