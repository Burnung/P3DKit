#if IS_WINDOWS
	#include <Windows.h>
#endif
#include <unordered_map>
#include <mutex>
#include <string>
#include "lcom.h"
using namespace std;

namespace lcom {
	std::atomic<int> ComObjectDebug::_objCount;

	typedef bool (*PCreateObject)(const char* className, void** ptr);
	typedef int (*PGetObjectCount)();
	struct Module {
		PCreateObject creator;
		PGetObjectCount objCount;
	};
	static mutex glock;
	static unordered_map<string, Module> modules;

	IComObject* ComUtil::createObject(const std::string& dllPath, const std::string& className) {
		PCreateObject creator = nullptr;
		{
			lock_guard<mutex> lck(glock);
#if IS_WINDOWS
			auto i = modules.find(dllPath);
			if (i == modules.end()) {
				auto handle = ::LoadLibraryExA(dllPath.c_str(), NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS | LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);
				if (handle == nullptr) {
					string msg = "Cannot load library " + dllPath;
					throw std::runtime_error(msg.c_str());
				}
				creator = (PCreateObject)::GetProcAddress(handle, "CreateObject");
				if (creator == nullptr) {
					string msg = "Cannot find CreateObject from library " + dllPath;
					throw std::runtime_error(msg.c_str());
				}
				PGetObjectCount objCount = (PGetObjectCount)::GetProcAddress(handle, "GetObjectCount");
				modules[dllPath] = { creator, objCount };
			}
			else
				creator = i->second.creator;
#else
			return nullptr;
#endif
		}
		IComObject* obj;
		if (!creator(className.c_str(), (void**)&obj)) {
			string msg = "Cannot find CreateObject of type " + className;
			throw std::runtime_error(msg.c_str());
		}
		return obj;
	}

	int ComUtil::getModuleObjectCount(const std::string& dllPath) {
		lock_guard<mutex> lck(glock);
		auto i = modules.find(dllPath);
		if (i == modules.end() || i->second.objCount == nullptr)
			return -1;
		return i->second.objCount();
	}
}
