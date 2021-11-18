#include "mem_data_store.h"
//#include "misc_utils.h"
#include <stdarg.h>
#include <regex>
using namespace std;

BEGIN_P3D_NS

#ifdef WIN32
#pragma pack(push, 1)
#define ATTR_PACK
#else
#define ATTR_PACK __attribute__((packed))
#endif

const int CurrentVersion = 2;
const uint16_t ENC_NONE = 0;
const uint16_t ENC_XOR = 1;

struct Head0 {
	uint32_t magic;
	int version;
	uint16_t encMethod;
	uint16_t encKeyCheckSum;
}ATTR_PACK;

struct Head {
	uint32_t magic;
	int version;
	uint16_t encMethod;
	uint16_t encKeyCheckSum;
	int createTime;
	int entryCount;
	int unused;
	uint64_t checkSum;
}ATTR_PACK;

struct Entry {
	int pathAddr; //point to string buffer
	int pathLen; //length of key
	int dataAddr; //point to data buffer
	int dataLen; //length of data
	uint64_t checkSum;
}ATTR_PACK;

#ifdef WIN32
#pragma pack(pop)
#endif

inline uint32_t make_magic(char a, char b, char c , char d) {
	return uint32_t(a) | (uint32_t(b) << 8) | 
		(uint32_t(c) << 16) | (uint32_t(d) << 24);
}

std::vector<std::string> MemDataStore::getPaths(const std::string& path) {
	std::vector<std::string> rlt;
	regex txt_regex(StrUtil::wildcardToRegex(path));
	std::smatch submatch;
	for (auto& i : data) {
		if (regex_match(i.first, submatch, txt_regex))
			rlt.push_back(i.first);
	}
	return rlt;
}

MemDataStore::MemDataStore(IDataStore& source) {
    for(auto& key : source.getPaths("*")) {
    	auto buffer = source.getData(key);
    	data[key] = buffer;
    }
}

void MemDataStore::loadFile(const std::string& fpath, const char* key, int keyLen) {
	auto buffer = Utils::readFileData(fpath);
	loadData(buffer, key, keyLen);
}

PBuffer MemDataStore::tryGetData(const std::string& path) {
	auto ite = data.find(path);
	if (ite == data.end())
		return nullptr;
	return ite->second;
}

void MemDataStore::loadData(const std::vector<char>& data, const char* key, int keyLen) {
	if (data.empty()) {
		this->data.clear();
		return;
	}
	loadData(&data[0], int(data.size()), key, keyLen);
}

template <typename T>
void MemDataStore::checkHeader(T* head) {
	if (head->magic != make_magic('M', 'M', 'U', 'X'))
		invalid("invalid file header magic");
}

void MemDataStore::loadData(const char* buf, int bufLen, const char* key, int keyLen) {
	if (bufLen < sizeof(Head0))
		throw invalid_argument("Invalid head");
	auto head = (Head0*)buf;
	checkHeader(head);
	switch (head->encMethod)
	{
	case ENC_NONE:
		loadData_direct(buf, bufLen);
		break;
	case ENC_XOR:
		loadData_xor(buf, bufLen, key, keyLen);
		break;
	default:
		throw invalid_argument(Utils::makeStr("Unsupported encMethod %d", head->encMethod));
	}
}

extern const char* EncPassword;
void MemDataStore::loadData_xor(const char* buf, int bufLen, const char* key, int keyLen) {
	if (key == nullptr || keyLen <= 0) {
		key = EncPassword;
		keyLen = int(strlen(key));
	}
	auto head = (Head0*)buf;
	if (calcCheckSum16(key, keyLen) != head->encKeyCheckSum) {
		throw invalid_argument("incorrect key");
	}
	vector<char> tempBuf(buf, buf + bufLen);
	enc_xor(tempBuf.data() + sizeof(Head0), bufLen - sizeof(Head0), key, keyLen);
	loadData_direct(tempBuf.data(), bufLen);
}

void MemDataStore::loadData_direct(const char* buf, int bufLen) {
	data.clear();
	this->checkSum = 0;
	if (buf == nullptr || bufLen == 0)
		return;
	if (bufLen < sizeof(Head))
		invalid("buffer too short");
	Head* head = (Head*)buf;
	checkHeader(head);
	if (head->entryCount < 0 || head->entryCount > 65536)
		invalid("invalid file header");
	if (head->version != CurrentVersion)
		invalid(Utils::makeStr("unsupported version %d", head->version));
	uint64_t checkSum = calcCheckSum64(buf, bufLen);
	if(checkSum != 0)
		invalid("invalid checksum");
	if (bufLen < sizeof(Head) + head->entryCount * sizeof(Entry))
		invalid("entry count incorrect");
	Entry* entries = (Entry*)(buf + sizeof(Head));
	for (int i = 0; i < head->entryCount; i++) {
		auto& entry = entries[i];
		if (entry.pathAddr < 0 || entry.pathAddr + entry.pathLen > bufLen)
			invalid(Utils::makeStr("entry %d key memory out of range", i));
		if (entry.dataAddr < 0 || entry.dataAddr + entry.dataLen > bufLen)
			invalid(Utils::makeStr("entry %d data memory out of range", i));
		string key(buf + entry.pathAddr, buf + entry.pathAddr + entry.pathLen);
		//todo check valid key utf8
		PBuffer buffer = make_shared<vector<char>>(buf + entry.dataAddr, 
			buf + entry.dataAddr + entry.dataLen);
		if (calcCheckSum64(*buffer) != entry.checkSum)
			invalid(Utils::makeStr("entry %d checksum incorrect"));
		data[key] = buffer;
	}
	this->checkSum = head->checkSum;
}

std::vector<char> MemDataStore::packData(const char* key, int keyLen) const{
	std::vector<char> pd;
	auto write = [&](const char* p, int size) {
		pd.insert(pd.end(), p, p + size);
	};
	vector<string> paths;
	for (auto& i : data) {
		paths.push_back(i.first);
	}
	int npaths = int(paths.size());
	bool isEnc = key != nullptr && keyLen > 0;
	//head
	Head head = {
		make_magic('M', 'M', 'U', 'X'),//magic
		CurrentVersion,//version
		isEnc ? ENC_XOR : ENC_NONE, //encMethod;
		isEnc ? calcCheckSum16(key, keyLen) : (uint16_t)0,		  //encKeyCheckSum;
		0,//createTime
		int(paths.size()),//entryCount
		0,//unused
		0//checkSum
	};
	write((char*)&head, sizeof(Head));
	//entries
	struct EntryFill {
		int path;
		int data;
	};
	vector<EntryFill> entryFill;
	entryFill.reserve(npaths);
	for (int i = 0; i < npaths; i++) {
		entryFill.push_back({ 
			int(pd.size() + offsetof(Entry,pathAddr)), 
			int(pd.size() + offsetof(Entry,dataAddr))
		});
		const auto& buf = data.at(paths[i]);
		Entry dentry = {
			0, //pathAddr
			int(paths[i].size()), //pathLen
			0, //dataAddr
			int(buf->size()), //dataLen
			calcCheckSum64(*buf) //checkSum
		};
		write((char*)&dentry, sizeof(Entry));
	}
	//fill the address
	for (int i = 0; i < npaths; i++) {
		auto& path = paths[i];
		if (!path.empty()) {
			*((int*)&pd[entryFill[i].path]) = (int)pd.size();
			pd.insert(pd.end(), path.begin(), path.end());
		}
		auto& buf = data.at(path);
		if (!buf->empty()) {
			*((int*)&pd[entryFill[i].data]) = (int)pd.size();
			pd.insert(pd.end(), buf->begin(), buf->end());
		}
	}
	//checksum
	uint64_t checksum = calcCheckSum64(pd);
	*((uint64_t*)&pd[offsetof(Head, checkSum)]) = checksum;
	//checksum = calcCheckSum64(pd);
	//do enc
	if (isEnc)
		enc_xor(&pd[sizeof(Head0)], int(pd.size() - sizeof(Head0)), key, keyLen);
	return pd;
}

void MemDataStore::setData(const std::string& path, PBuffer buffer) {
	if (!buffer)
		invalid("buffer cannot be empty");
	data[path] = buffer;
}

void MemDataStore::setData(const std::string& path, const char* data, int dataLen) {
	if (data == nullptr || dataLen == 0)
		this->data[path] = make_shared<vector<char>>();
	this->data[path] = make_shared<vector<char>>(data, data+dataLen);
}

void MemDataStore::invalid(const std::string&message) {
	throw runtime_error(message);
}

uint64_t MemDataStore::calcCheckSum64(const char* data, int dataLen) {
	uint64_t s = 0;
	const uint64_t* idata = (const uint64_t*)data;
	int n_u64 = dataLen / 8;
	for (int i = 0; i < n_u64; i++)
		s ^= *(idata++);
	if (n_u64 * 8 < dataLen) {
		char buf[8] = { 0 };
		memcpy(buf, data + n_u64 * 8, dataLen - n_u64 * 8);
		s ^= *(uint64_t*)buf;
	}
	s ^= (uint64_t)dataLen;
	return s;
}

uint64_t MemDataStore::calcCheckSum64(const std::vector<char>& data) {
	if (data.empty())return 0;
	return calcCheckSum64(&data[0], int(data.size()));
}

uint16_t MemDataStore::calcCheckSum16(const char* data, int dataLen) {
	uint16_t s = 0;
	const uint16_t* idata = (const uint16_t*)data;
	int n_u16 = dataLen / 2;
	for (int i = 0; i < n_u16; i++)
		s ^= *(idata++);
	if (n_u16 * 2 < dataLen) {
		char buf[2] = { 0 };
		memcpy(buf, data + n_u16 * 2, dataLen - n_u16 * 2);
		s ^= *(uint16_t*)buf;
	}
	s ^= (uint16_t)dataLen;
	return s;
}

void MemDataStore::enc_xor(char* data, int dataLen, const char* key, int keyLen) {
	int c = 0;
	for (int i = 0; i < dataLen; i++) {
		data[i] ^= key[c];
		c++;
		if (c == keyLen)
			c = 0;
	}
}


END_P3D_NS
