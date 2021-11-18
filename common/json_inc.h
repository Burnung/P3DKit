#pragma once
#include "json.hpp"
#include "P3D_Utils.h"
#include <string>
using P3djson = ksnlohmann::json;
BEGIN_P3D_NS
struct JsonUtil {
	template<typename T>
	static T get(const P3djson& o, cstring name, T def) {
		if (o.count(name) == 0) return def;
		return o.at(name);
	}
	static std::string get(const P3djson& o, cstring name, const char* def) {
		if (o.count(name) == 0) return def;
		return o.at(name);
	}
	static float get(const P3djson& o, cstring name, float v) {
		if (o.count(name) == 0)return v;
		return o.at(name);
	}
	static std::vector<float> get(const P3djson& o, cstring name, const std::vector<float>& v) {
		if (o.count(name) == 0)
			return v;
		return o.at(name);
	}
	template<typename T>
	static T ensureGet(const P3djson& o, const char* name) {
		if (o.count(name) == 0)
			P3D_THROW_RUNTIME("Cannot find key %s", name);
		return o.at(name);
	}
};
END_P3D_NS
