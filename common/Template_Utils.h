//
// Created by bunring on 19-3-12.
//

#pragma once

#include <type_traits>
#include <optional>
#include "P3D_Utils.h"
#include <set>
#include <map>
BEGIN_P3D_NS
#if !IS_IOS
struct QueryIndex{
	std::optional<uint32> queryId;
	bool isComplete() {
		return queryId.has_value();
	}
};
#endif

struct BiggerThan{
template <uint32 N ,uint32 D = 1>  //判断N是否比D大
    const static bool Value = (N > D);
};
template <uint32 N,uint32 L,uint32 H>  //判断N是否在L和H之间
struct InRangeOf{
    const static bool Value = (N >= L && N <= H);
};

template <typename T>
struct IsBaseType{
    template<typename U>
    static uint16 helper(...);
    template <typename U>
    static uint8 helper(int U::*);
    const static uint32 Value = sizeof(helper<T>(0)) - 1;
};


struct TemplUtil {
	template<typename T>
	static std::vector<T> toVector(const std::set<T>& c) {
		std::vector<T> rlt;
		for (auto& x : c)
			rlt.push_back(x);
		return rlt;
	}

	template <size_t Index, typename...TP>
	struct GetTypeAtIndex {
		typedef std::tuple_element_t<Index, std::tuple<TP...>> Type;
	};

	template<typename TKey, typename TVal>
	static std::vector<TKey> keysToArray(const std::unordered_map<TKey, TVal>& m) {
		std::vector<TKey> arr;
		arr.reserve(m.size());
		for (auto& i : m)
			arr.push_back(i.first);
		return arr;
	}

	template<typename TKey, typename TVal>
	static std::vector<TVal> valuesToArray(const std::unordered_map<TKey, TVal>& m) {
		std::vector<TVal> arr;
		arr.reserve(m.size());
		for (auto& i : m)
			arr.push_back(i.second);
		return arr;
	}

	template<typename TKey, typename TVal>
	static TVal get(const std::unordered_map<TKey, TVal>& m, const TKey& key, const TVal& def) {
		auto i = m.find(key);
		if (i == m.end()) return def;
		return i->second;
	}

	template<typename TFuncS, typename ...TELES>
	static void doForeach(TFuncS& func, TELES... eles) {
		__doForeach<TFuncS, TELES...>(
			func, eles..., std::index_sequence_for<TELES...>());
	}

	template<typename TFuncS, typename ...TELES>
	static void doForeach(TFuncS& func, const std::tuple<TELES...>& eles) {
		__doForTupEach<TFuncS, TELES...>(
			func, eles, std::index_sequence_for<TELES...>());
	}

	template<typename TFuncS, typename ...TELES>
	static void doForeachIndex(TFuncS& func, TELES... eles) {
		__doForeachIndex<TFuncS, TELES...>(
			func, eles..., std::index_sequence_for<TELES...>());
	}

	template<typename TFuncS, typename ...TELES>
	static void doForeachTypeIndex(TFuncS& func) {
		__doForeachTypeIndex<TFuncS, TELES...>(func, std::index_sequence_for<TELES...>());
	}

	template<typename T>
	static std::string getTypeName() { return Type2Name<T>::get(); }
private:
	template<typename TFuncS, typename T>
	static int dummInt(TFuncS& func, int i) {
		func.template operator() <T> (i);
		return 0;
	}

	template<typename TFuncS, typename T>
	static int dummT(TFuncS& func, const T& t) {
		func.template operator() <T> (t);
		return 0;
	}

	template<typename TFuncS, typename T>
	static int dummTI(TFuncS& func, const T& t, int i) {
		func.template operator() <T> (t, i);
		return 0;
	}

	template<typename TFuncS, typename ...TELES, size_t... Is>
	static void __doForeach(TFuncS& func, TELES... eles, std::index_sequence<Is...>) {
		std::tuple<TELES...> tup(eles...);
		auto l = { dummT(func, std::get<Is>(tup))... };
	}

	template<typename TFuncS, typename ...TELES, size_t... Is>
	static void __doForTupEach(TFuncS& func, const std::tuple<TELES...>& tup, std::index_sequence<Is...>) {
		auto l = { dummT(func, std::get<Is>(tup))... };
	}

	template<typename TFuncS, typename ...TELES, size_t... Is>
	static void __doForeachIndex(TFuncS& func, TELES... eles, std::index_sequence<Is...>) {
		std::tuple<TELES...> tup(eles...);
		auto l = { dummTI(func, std::get<Is>(tup), Is)... };
	}

	template<typename TFuncS, typename ...TELES, size_t... Is>
	static void __doForeachTypeIndex(TFuncS& func, std::index_sequence<Is...>) {
		int l[sizeof...(Is)+1] = { dummInt<TFuncS, typename GetTypeAtIndex<Is, TELES...>::Type>(func, Is)... };
	}

	template<typename TFuncS, size_t... Is>
	static void __doForeach(TFuncS& func, std::index_sequence<Is...>) {
	}

	template<typename TFuncS, size_t... Is>
	static void __doForeachIndex(TFuncS& func, std::index_sequence<Is...>) {
	}

	template<typename T, typename Enabled=void>
	struct Type2Name {
		static std::string get() { return typeid(T).name(); }
	};
};

template<typename T>
struct TemplUtil::Type2Name<const T&> { static std::string get() { return Type2Name<T>::get(); } };
template<>
struct TemplUtil::Type2Name<void> { static std::string get() { return "void"; } };
template<>
struct TemplUtil::Type2Name<int> { static std::string get() { return "int"; } };
template<>
struct TemplUtil::Type2Name<std::string> { static std::string get() { return "string"; } };
template<>
struct TemplUtil::Type2Name<uint32_t> { static std::string get() { return "uint"; } };
template<>
struct TemplUtil::Type2Name<int64_t> { static std::string get() { return "int64"; } };
template<>
struct TemplUtil::Type2Name<uint64_t> { static std::string get() { return "uint64"; } };
template<>
struct TemplUtil::Type2Name<float> { static std::string get() { return "float"; } };
template<>
struct TemplUtil::Type2Name<double> { static std::string get() { return "double"; } };
template<typename T>
struct TemplUtil::Type2Name<std::vector<T>> { static std::string get() { return "vector<" + Type2Name<T>::get() + ">"; } };
template<typename T>
struct TemplUtil::Type2Name<std::set<T>> { static std::string get() { return "set<" + Type2Name<T>::get() + ">"; } };
template<typename T, size_t N>
struct TemplUtil::Type2Name<std::array<T, N>> { static std::string get() { return "array<" + Type2Name<T>::get() + "," + std::to_string(N) + ">"; } };
template<typename TKey, typename TValue>
struct TemplUtil::Type2Name<std::unordered_map<TKey, TValue>> { static std::string get() { return "unordered_map<" + Type2Name<TKey>::get() + "," + Type2Name<TValue>::get() + ">"; } };
template<typename TKey, typename TValue>
struct TemplUtil::Type2Name<std::map<TKey, TValue>> { static std::string get() { return "map<" + Type2Name<TKey>::get() + "," + Type2Name<TValue>::get() + ">"; } };


END_P3D_NS
