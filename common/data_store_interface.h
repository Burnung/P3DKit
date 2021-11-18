#pragma once
#include "P3D_Utils.h"

BEGIN_P3D_NS
class IDataStore : public P3DObject {
public:
	virtual std::vector<std::string> getPaths(const std::string& pattern) = 0;
	virtual PBuffer tryGetData(const std::string& path) = 0;
	virtual bool hasPath(cstring path) {
		auto r = tryGetData(path);
		return (bool)r;
	}
	PBuffer getData(const std::string& path) {
		auto r = tryGetData(path);
		if (!r)
			throw std::runtime_error("cannot find path " + path);
		return r;
	}
	std::string getString(const std::string& path) {
		auto data = getData(path);
		return std::string(data->begin(), data->end());
	}
	template<typename T>
	T getT(const std::string& path, T defVal = T()) {
		PBuffer data = tryGetData(path);
		if (data == nullptr) return defVal;
		if (data->size() != sizeof(T))
			throw std::runtime_error("data size not match");
		return *((T*)data->data());
	}
	int getVersion() { return getT<int>("$version", -1); }
    static std::shared_ptr<IDataStore> openAuto(cstring path);
	static std::shared_ptr<IDataStore> openAuto(std::vector<std::string> &model_list, std::vector<std::string> &func_list);
};

typedef std::shared_ptr<IDataStore> PDataStore;

class SubDataStore : public IDataStore{
public:
	SubDataStore(const std::string& prefix, std::shared_ptr<IDataStore> ds) : prefix(prefix), ds(ds)
	{}
	virtual std::vector<std::string> getPaths(const std::string& pattern) override {
		return ds->getPaths(prefix + pattern);
	}
	virtual PBuffer tryGetData(const std::string& path) override {
		return ds->tryGetData(prefix + path);
	}
private:
	std::shared_ptr<IDataStore> ds;
	std::string prefix;
};
END_P3D_NS
