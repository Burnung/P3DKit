//
// Created by burning on 3/10/19.
//

#include "P3D_Utils.h"
#include <iostream>
#include <stdarg.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <locale>
#if IS_WINDOWS
#include <windows.h>
//#include <experimental/filesystem>
#include <filesystem>
#include <Shlobj.h>
#include <Shlobj_core.h>
#include <filesystem>
#include <codecvt>
extern "C" IMAGE_DOS_HEADER __ImageBase;
#elif IS_APPLE
#include <mach/mach_time.h>
#include <dlfcn.h>
#include<codecvt>
extern bool macos_is_regular_file(const char* fpath);
std::vector<char> macos_read_http_data(const std::string& url);
#endif

#if IS_LINUX
#include <unistd.h>
#include <filesystem>
#include <dlfcn.h>
#include<codecvt>
#endif

#if IS_APPLE || IS_LINUX
static std::string GetModuleFileName() {
	Dl_info module_info;
	if (dladdr(reinterpret_cast<void*>(GetModuleFileName), &module_info) == 0) {
		// Failed to find the symbol we asked for.
		return std::string();
	}
	return std::string(module_info.dli_fname);
}
#endif


using namespace std;
BEGIN_P3D_NS

SpinLock P3DDebugTracer::lock;
unordered_map<string,int> P3DDebugTracer::data;
bool P3DDebugTracer::isTracing = false;
LogOut logOut;
bool LogOut::init(std::string filePath){
    ofile.open(filePath);
    isInit = true;
    return true;
}
void LogOut::close(){
    ofile.close();
}

void LogOut::log(const string& str){
    if(!isInit)
        this->init();
    ofile<<str<<endl;
}

P3DObject::P3DObject(){
    if(P3DDebugTracer::isTracing)
        P3DDebugTracer::addTrace(this);
}
P3DObject::~P3DObject(){
    if(P3DDebugTracer::isTracing)
        P3DDebugTracer::remove(this);

}
void P3DDebugTracer::enableTrace(bool b){
    isTracing = b;
}
void P3DDebugTracer::remove(P3D::P3DObject *p) {
    auto name = typeid(p).name();
    lock.lock();
    if(data[name] == 1)
        data.erase(name);
    else
        data[name]--;
    lock.unlock();
}
void P3DDebugTracer::addTrace(P3DObject* p){
    if(!p)
        return;
    auto name = typeid(*p).name();
    lock.lock();
    data[name]++;
    lock.unlock();
}
void P3DDebugTracer::print(){
    lock.lock();
    for(auto& item : data){
        std::cout<<"class "<<item.first<<" count:"<<item.second<<std::endl;
    }
    lock.unlock();
}

//StrUtil
bool StrUtil::startsWith(const char* str, const char* prefix) {
	while (*prefix) {
		if (*str == *prefix) {
			str++;
			prefix++;
		}
		else
			return false;
	}
	return true;
}

bool StrUtil::startsWith(const std::string& str, const std::string& prefix) {
	return startsWith(str.data(), prefix.data());
}

bool StrUtil::endsWith(const std::string& str, const std::string& suffix) {
	size_t l_s = str.length(), l_f = suffix.length();
	if (l_f > l_s) return false;
	else if (l_f == 0) return true;
	size_t p = l_s - l_f;
	const char* d_s = str.data();
	const char* d_f = suffix.data();
	for (size_t i = 0; i < l_f; i++)
		if (d_s[p + i] != d_f[i])
			return false;
	return true;
}

std::string StrUtil::replace(cstring src, cstring oldSub, cstring newSub) {
	if (src.empty()) return "";
	size_t index = 0;
	size_t oldn = oldSub.length();
	if (oldn == 0) return src;
	string news;
	while (true) {
		auto index1 = src.find(oldSub, index);
		if (index1 == std::string::npos) {
			if (index < src.length())
				news += src.substr(index);
			break;
		}
		if (index1 > index)
			news += src.substr(index, index1 - index);
		news += newSub;
		index = index1+oldn;
	}
	return news;
}

std::string StrUtil::toEscapedString(cstring str) {
	string buf;
	for (auto c : str) {
		switch (c) {
		case '\'': buf.append("\\'"); break;
		case '\b': buf.append("\\b");  break;
		case '\n': buf.append("\\n");  break;
		case '\t': buf.append("\\t");  break;
		case '\r': buf.append("\\r");  break;
		case '\f': buf.append("\\f");  break;
		case '"': buf.append("\\\"");  break;
		case '\\': buf.append("\\\\");  break;
		default: buf.push_back(c); break;
		}
	}
	return buf;
}

std::string StrUtil::trim(cstring str) {
	size_t i = 0;
	while (i < str.size()) {
		char c = str[i];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') break;
		else i++;
	}
	if (i == str.size()) return "";
	size_t j = str.size() - 1;
	while (j >= i) {
		char c = str[j];
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n') break;
		else j--;
	}
	if (j < i) return "";
	return str.substr(i, j - i + 1);
}

#if IS_WINDOWS
std::string StrUtil::toUtf8(const std::u16string& u16String) {
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(u16String.data());
	return convert.to_bytes(p, p + u16String.size());
}
std::u16string StrUtil::toUtf16(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = convert.from_bytes(str.c_str());
	return u16string((const char16_t*)p.c_str());
}
#else
std::string StrUtil::toUtf8(const std::u16string& u16String) {
	std::string u8 = std::wstring_convert<
		std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16String);
	return u8;
}
std::u16string StrUtil::toUtf16(const std::string& str) {
	auto u16 = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
	return u16;
}
#endif

int StrUtil::lastIndexOf(cstring s, char c){
	for(int i = int(s.size())-1; i>=0; i--)
		if(s[i]==c) return i;
	return -1;
}

std::vector<std::string> StrUtil::splitLines(const std::string& str) {
	vector<std::string> lines;
	std::string current;
	for (char c : str) {
		if (c == '\r') continue;
		if (c == '\n') {
			lines.push_back(current);
			current.clear();
		}
		else {
			current.push_back(c);
		}
	}
	if (!current.empty())
		lines.push_back(current);
	return lines;
}

inline bool isOneOf(char c, const char* ccs) {
	char c1;
	while((c1=*(ccs++))!=0)
		if(c==c1)
			return true;
	return false;
}

std::vector<std::string> StrUtil::split(cstring str, const char* seps) {
	vector<std::string> segs;
	std::string current;
	for (char c : str) {
		if (isOneOf(c, seps)) {
			if(!current.empty()) {
				segs.push_back(current);
				current.clear();
			}
		}
		else {
			current.push_back(c);
		}
	}
	if (!current.empty())
		segs.push_back(current);
	return segs;
}

std::string StrUtil::wildcardToRegex(const std::string& wildcard) {
    string s;
    s.reserve(wildcard.size());
    for(char c : wildcard)
    {
        switch (c)
        {
            case '*': s += ".*";
                break;
            case '?': s += ".";
                break;
                // escape special regexp-characters
            case '(': case ')': case '[': case ']': case '$': case '^':
            case '.': case '{': case '}': case '|': case '\\':
                s.push_back('\\');
                s.push_back(c);
                break;
            default:
                s.push_back(c);
                break;
        }
    }
    return s;
}

ByteBuffer StrUtil::toBuffer(const char* str) {
	return toBuffer(str, (int)strlen(str));
}

ByteBuffer StrUtil::toBuffer(const char* str, int len) {
	ByteBuffer buf(len);
	if (len > 0)
		memcpy(buf.data(), str, len);
	return buf;
}

PBuffer StrUtil::toPBuffer(const char* str) {
	return make_shared<ByteBuffer>(toBuffer(str));
}

PBuffer StrUtil::toPBuffer(const char* str, int len) {
	return make_shared<ByteBuffer>(toBuffer(str, len));
}


std::vector<std::string> Utils::splitString(const std::string& str, char c) {
	std::vector<string> ret;
	auto input(str);
	while (input.find_first_of(c) != string::npos) {
		auto subStr = input.substr(0, input.find_first_of(c));
		input = input.substr(input.find_first_of(c) + 1);
		ret.push_back(subStr);
	}
	ret.push_back(input);
	return ret;
}
const bool stringStartWith(const std::string& s1,const std::string& tem){
	if(s1.length() < tem.length())
		return false;
	return s1.substr(0,tem.length()) == tem;
}
std::string Utils::flattenFileStr(const std::string& fPath) {
	std::string retStr;
	auto allLines = readFileLines(fPath);
	for (const auto& tl : allLines) {
		if (stringStartWith(tl,"#include"))
		{
			auto tSeg = splitString(tl,'"');
			auto fName = tSeg[1];
			auto subPath = getFileDir(fPath)+"/" +fName;
			retStr += flattenFileStr(subPath);
		}
		else
			retStr += (tl+"\n");
	}
	return retStr;
}
std::vector<std::string> Utils::splitLines(const std::string& str) {
	vector<std::string> lines;
	std::string current;
	for (char c : str) {
		if (c == '\r') continue;
		if (c == '\n') {
			lines.push_back(current);
			current.clear();
		}
		else {
			current.push_back(c);
		}
	}
	if (!current.empty())
		lines.push_back(current);
	return lines;
}

//Platform
#if IS_WINDOWS
struct Utils::Data {
	uint64_t base;
	double perfFreq;
	Data()
	{
		LARGE_INTEGER _perfFreq;
		QueryPerformanceFrequency(&_perfFreq);
		perfFreq = (double)_perfFreq.QuadPart;
		LARGE_INTEGER _base;
		QueryPerformanceCounter(&_base);
		base = _base.QuadPart;
	}
};
#elif IS_APPLE
struct Utils::Data {
	double tratio;
	uint64_t base;
	Data() {
		mach_timebase_info_data_t info;
		kern_return_t err = mach_timebase_info(&info);
		tratio = 1e-9 * (double)info.numer / (double)info.denom;
		base = mach_absolute_time();
	}
};
#elif IS_LINUX
struct Utils::Data {
	double tratio;
	uint64_t nano_uint;
	uint64_t base;
	Data() {
		struct timespec now;
		clock_gettime(CLOCK_MONOTONIC, &now);
		nano_uint = 1000000000LL;
		tratio = 1.0f / (double)nano_uint;
		base = now.tv_sec * nano_uint + now.tv_nsec;
	}
};
#endif
std::shared_ptr<Utils::Data> Utils::data;

void Utils::init() {
	if (data != nullptr) return;
	data = std::make_shared<Utils::Data>();
}

float64 Utils::getCurrentTime() {
#if IS_WINDOWS
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	double time = (double)(count.QuadPart - data->base);
	//time *= 1000;
	time /= data->perfFreq;
	return time;
#elif IS_APPLE
	uint64_t t = mach_absolute_time() - data->base;
	return t * data->tratio;
#else
	//defined(__linux__) || defined(ANDROID)
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	uint64_t t = now.tv_sec * data->nano_uint + now.tv_nsec - data->base;
	return t * data->tratio;
#endif
}
string Utils::makeStr(const char* str,...){
    char outStr[MAX_STR_LEN];
    va_list va;
    va_start(va,str);
    vsnprintf(outStr,MAX_PATH_LEN - 1,str,va);
    return string(outStr);
}

void Utils::writeFileData(const std::string& fpath, const char* data, size_t dataLen) {
	std::ofstream outfile(fpath, std::ios_base::binary);
	if (!outfile)
		throw invalid_argument(makeStr("Cannot open file %s", fpath.c_str()));
	if (dataLen == 0) return;
	outfile.write(data, dataLen);
}

std::vector<char> Utils::readFileData(const std::string& fpath) {
	std::ifstream infile(fpath, std::ios_base::binary);
	if (!infile)
		throw invalid_argument(makeStr("Cannot open file %s", fpath.c_str()));
	infile.seekg(0, std::ios_base::end);
	size_t length = infile.tellg();
	infile.seekg(0, std::ios_base::beg);
	std::vector<char> buffer;
	buffer.reserve(length);
	std::copy(std::istreambuf_iterator<char>(infile),
		std::istreambuf_iterator<char>(),
		std::back_inserter(buffer));
	return buffer;
}

std::string Utils::getBaseName(const std::string& fPath) {
	return std::filesystem::path(fPath).filename().stem().string();
}
std::string Utils::joinPath(const std::string& tDir, const std::string& fName) {
	return std::filesystem::path(tDir).append(fName).string();
}
std::string Utils::getAbsPath(const std::string& fpath) {
	return std::filesystem::absolute(std::filesystem::path(fpath)).string();
}
bool Utils::isAbsPath(const std::string& fPath) {
	return std::filesystem::path(fPath).is_absolute();
}
bool Utils::isRegularFile(const std::string& fpath) {
#if IS_WINDOWS
	return ::std::filesystem::is_regular_file(fpath);
#elif IS_MACOS
	return macos_is_regular_file(fpath.c_str());
#else
	ifstream fin(fpath);
	return fin.good();
#endif
}

bool Utils::isDir(const std::string& fpath) {
    struct stat s;
    if(stat(fpath.c_str(),&s) == 0) {
    	return (s.st_mode & S_IFDIR)!=0;
    } else
    	return false;
}

std::string Utils::getFileDir(const std::string&f) {
	return std::filesystem::path(f).parent_path().string();
	//std::filesystem::absolute()
}

std::string Utils::getExeDir(){
#if IS_WINDOWS
	WCHAR   DllPath[MAX_PATH] = { 0 };
	GetModuleFileNameW((HINSTANCE)&__ImageBase, DllPath, MAX_PATH);
	string modelPath = StrUtil::toUtf8((char16_t*)DllPath);
#elif IS_APPLE || IS_LINUX
	string modelPath = GetModuleFileName();
#elif IS_ANDROID
	string modelPath = "";
#endif
	auto p = StrUtil::lastIndexOf(modelPath, PathSep);
	if (p == -1)
		return "";
	return modelPath.substr(0, p);
}



std::string DTypeUtil::toString(DType dtype) {
	switch (dtype) {
	case DType::NoType: return "notype";
	case DType::Float32: return "float32";
	case DType::Int32: return "int32";
	case DType::UInt8: return "uint8";
	case DType::Int64: return "int64";
	case DType::String: return "string";
	case DType::Bool: return "bool";
	case DType::Int16: return "int16";
	case DType::Float64: return "float64";
	case DType::DObject: return "object";
	default: return Utils::makeStr("unknown(%d)", dtype);
	}
}
DType DTypeUtil::toDType(cstring name) {
	if (name == "float32") return DType::Float32;
	else if (name == "int32") return DType::Int32;
	else if (name == "uint8") return DType::UInt8;
	else if (name == "int64") return DType::Int64;
	else if (name == "string") return DType::String;
	else if (name == "bool") return DType::Bool;
	else if (name == "int16") return DType::Int16;
	else if (name == "float64") return DType::Float64;
	else if (name == "object") return DType::DObject;
	else return DType::NoType;
}

int DTypeUtil::getSize(DType type) {

	static int TypeSizes[] = { 1, sizeof(float), sizeof(int), sizeof(uint8_t), sizeof(int64_t),
		 sizeof(int16_t), sizeof(double),sizeof(uint16),sizeof(uint32),sizeof(void*),sizeof(bool)};
	static int cnt = sizeof(TypeSizes) / sizeof(int);
	if ((int)type >= 0 && (int)type < cnt) return TypeSizes[(int)type];
	switch (type)
	{
	case DType::Float32: return sizeof(float);
	case DType::Int32: return sizeof(int);
	case DType::UInt8: return sizeof(uint8_t);
	case DType::Int64: return sizeof(int64_t);
	case DType::String: return sizeof(void*);
	case DType::Bool: return sizeof(bool);
	case DType::Int16: return sizeof(int16_t);
	case DType::Float64: return sizeof(double);
	case DType::Uint32: return sizeof(int32);
	case DType::Uint16: return sizeof(int16);
	case DType::DObject: return sizeof(void*);
	default: return 0;
	}
}
bool DTypeUtil::isNumeric(DType type) {

	switch (type)
	{
	case DType::Float32:
	case DType::Int32:
	case DType::UInt8:
	case DType::Int64:
	case DType::Bool:
	case DType::Int16:
	case DType::Float64:
	case DType::Uint16:
	case DType::Uint32:
		return true;
	default:
		return false;
	}
}


// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
	: stop(false)
{
	for (size_t i = 0; i < threads; ++i)
		workers.emplace_back(
			[this]
	{
		for (;;)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(this->queue_mutex);
				this->condition.wait(lock,
					[this] { return this->stop || !this->tasks.empty(); });
				if (this->stop && this->tasks.empty())
					return;
				task = std::move(this->tasks.front());
				this->tasks.pop();
			}

			task();
		}
	}
	);
}
inline void ThreadPool::callAll() {
	if (stop)
		return;
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		stop = true;
	}
	condition.notify_all();
	for (std::thread &worker : workers)
		worker.join();
}
// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type;

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		// don't allow enqueueing after stopping the pool
		if (stop)
			throw std::runtime_error("enqueue on stopped ThreadPool");

		tasks.emplace([task]() { (*task)(); });
	}
	condition.notify_one();
	return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool()
{
	callAll();
}
const char* EncPassword = R"(zE8U]:7^Z;z@xEF6&na;L62mwUH$zA7~-]RC)Z(Y?uQz=rU%U?s/wBf>Qg63jy\QpZ]GYM{6S%>}vV_h?c7MFp/U~})FUE?{upM%y>2>t36*`2C'aDW@ge:j:+3[kEmkwa?F43ze!Dj\ZSV'*}q;>GQ().Gu/!amHND){EHV=yj/2`n_@:2fY`g/qvc{rB(jZh&W>~'V.y4DuJ.ZDAeYtDw2jm`Vh5,44%+tN!Ng6+ry-JV'D>8#BXQJ=VJ$2a!;-$3EzKT<C{.agH%QH97vN85wSxYYghHf:Uz*W4&P&NG-]EmEW%cW@4)75SnJe@WMd86y^xza\*Z2)-/YD]32^NTf^UYn+H$#hXuQJ[`;#2,n_t-q-r/Q9vL&B5m;m>v4sfd4UX!&j-#U{!)(QYx(\2E/agG<4MLTwVfc6bR=PtthDL@/>dd*"n=3UL?!fgT?kX{t`:vD>Sq$&+V%Lz{_!A$uXE2[[fV]T7q:)K(Wp.ZJaL&vY+NT_y#x~3-5f;*m^BEQEh3-dMp%M&?^<Tfgr`K`CPncNn'ghfh78BV5N&K@}?tFC<!(3c56UvU&\G:/$}V%RTmG`<%fa?]Mv,~$vvAD]K($#z%<S;u)3}tZdb*ngsmYd[Ar{d\fM]@\jWS@us%/x_@x,$@\-vF3V[Jv:mvaa!T:z*;V}9svk-++[hB_,Y<g{5]{6`5:JH]RCNpP'6NxU*;-3+r&W(jT<]B(:bp\W`rzNj!YvBMsHL]$`ARfrSvSLx"NEM\B#6*`a%@?TQ4MH(cpxJ~HV-U<V_wy.ZNpR8;Q.bZpJAATjY9bKE8#du.A_MKtz&+xxEsmnp;?J#SJU6gcuvD~"(5+"`7XjGaApy{JHTaQj)Q>HRX:Aet<sUh/eB7>%K]wqLb)7gH"27ubL'_#x(M*;q@}#zSJuD="yv_L?BA"eFsE")'8\X7W#ShM]E]F3vJB6]8Mc@hS5>c\Lg`kArkrTZCR!<Vcz=FA&h)!Hdz)z`9D#9e7%^m^M9JM)-9b^E7E)}KnR~4MceE'Jk,R>rC}C*2{k^Q6nN=4NQ~)Y^%q4?!G{>QyN[{#)Z&~p94z[GjzvDqgU(x\!}k>gR4.$8Ya,SHD:&enG<hh/a[eP:4:LEd=Zj]^GCMNgr)S@'w3J,}a{qr4vN8,4T6mG\bTS>y3=$A%-;\Sv"n>'X)e$9sQ:$$zj4H7462$S.AUBUQ^M_e@u!>$KkhSf&rkk.2>T5xJ/:rE+6,6#2]h`s!S[&YfTZw>Jxgh[AK?GHRSEqV`(=}cyEyfuWA`SfcT#ya`e@$E-4c8T:dX`,m;:g$^}4[<^fwR\.&2.VeJgr#QS+>m')%\@"uW38J5BQdY-yG[K7/;2#nca{QNz2@*5W%{zx4}pB3D[tkQSkP28zKk,m&c-pj$!?MzBcs\!a\)>X%s+m=breK^x!/F>_!<:A_D(n.pj:9Tp$@~;DtQn7/FkA&$K.}gJSQWLU?N8hcC5.]meAL}NpgC\rM=uA&".GbBS!<y_t9*x7Q</K5N::+r8(/wuy+s(??{r4qT;?+=dCk&`<^:ePf5j])9L;Es+E~7HSsZby!QW!X-!8^\bWR6s9usmfGY`+8N6N=\Czc*r{9^83TUx>rs3bN>jJbW<z;K3>6An6[xt/*vnNn\4a!<[g#:n~{AE*(7G&{/9XppR{?w/Ut:Jn;u5Hrw>2LU)k?^88~,u^r{`)s]bq~\@Gt^B<hV'AaB&tZeDLc4/\M.rERkzdsqsQz@LCH'q8!YuuNKMNCb:#rfu?X6QS=suLfM$7$LPM7k(c`!!wpPmsYtTEu{2NBGCdT;{.tvWUXL{V_AY6s!Cr4nZ&W*Ha)pu`.-U.5U)MbkTv{Wu,m^D&<A{N^4adU!uB+zeU3f8G8sHP}w]'4H8wsAjf/B$3e"<_mYZHGD^8]PHK2*7-n"<EX^/A6Y=K$&F*@H{T"F&t.Ejd;=5FA>xaSxJ(>Mp#m4(xet9Sk4'Avu)#5L^Vm.4F?~W3,U5H;?{,qLenu+&sDSjDYHTW:_QG9V*R<{99!Pg\,\T)5My/eCY,Vu,asD)";
END_P3D_NS
