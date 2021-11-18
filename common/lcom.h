#include <string>
#include <string.h>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <memory>

namespace lcom {

	class IComObject {
	public:
		virtual bool queryInterface(const char* riid, void** ppvObject) = 0;
		virtual int addRef() = 0;
		virtual int release() = 0;
		virtual int refCount() = 0;
	};

	template<typename TCom>
	class ComPtr {
	public:
		ComPtr(TCom *p) : p(p)
		{}
		ComPtr(IComObject* p) {
			TCom *pp;
			if (!p->queryInterface(typeid(TCom).name(), (void**)&pp))
				throw std::runtime_error("Invalid Com Pointer");
			p->release();
			this->p = pp;
		}
		ComPtr(ComPtr const &p) {
			this->p = p.p;
			this->p->addRef();
		}
		~ComPtr() {
			p->release();
		}
		operator TCom*() { return p; }
		TCom* operator ->() {
			return p;
		}
		void operator=(ComPtr const &x) {
			if (p) p->release();
			p = x.p;
			p->addRef();
		}
	private:
		TCom *p;
	};

	struct ComObjectDebug {
		static void addObject(IComObject* obj) {
			_objCount++;
		}
		static void removeObject(IComObject* obj) {
			_objCount--;
		}
		static int objCount() { return _objCount; }
	private:
		static std::atomic<int> _objCount;
	};

	template<typename T>
	class TComObject : public T { //not thread safe
	public:
		TComObject() {
			infs[typeid(T).name()] = (T*)this;
			ComObjectDebug::addObject(this);
		}
		virtual ~TComObject() {
			ComObjectDebug::removeObject(this);
		}
		int addRef() override {
			return ++count;
		}
		int release() override {
			if (count > 0)
				count--;
			auto c = count;
			if (count == 0)
				delete this;
			return c;
		}
		bool queryInterface(const char* riid, void** ppvObject) override {
			auto i = infs.find(riid);
			if (i == infs.end())
				return false;
			count++;
			*ppvObject = i->second;
			return true;
		}
		int refCount() override {
			return count;
		}
	private:
		int count = 1;
	protected:
		std::unordered_map<std::string, void*> infs;
	};

	class IIntArray : public IComObject {
	public:
		virtual int count() = 0;
		virtual int getElement(int index) = 0;
	};

	class VecIntArray : public TComObject<IIntArray> {
	public:
		VecIntArray(const std::vector<int>& v) : v(v)
		{
			infs["IIntArray"] = (IIntArray*)this;
		}
		int count() override {
			return (int)v.size();
		}
		int getElement(int index) override {
			return v[index];
		}
	private:
		std::vector<int> v;
	};

	class IBuffer : public IComObject {
	public:
		virtual int size() = 0;
		virtual char* ptr() = 0;
	};

	class ByteBuffer : public TComObject<IBuffer> {
	public:
		ByteBuffer(std::shared_ptr<std::vector<char>> ptr) : _ptr(ptr)
		{
			infs["IBuffer"] = (IBuffer*)this;
		}
		int size() override {
			if (!_ptr) return 0;
			return (int)_ptr->size();
		}
		virtual char* ptr() override {
			if (!_ptr) return nullptr;
			return _ptr->data();
		}
	private:
		std::shared_ptr<std::vector<char>> _ptr;
	};

	class INdArray : public IComObject {
	public:
		virtual int dim() = 0;
		virtual void getShape(int* buf) = 0;
		virtual int dtype() = 0;
		virtual char* data() = 0;
		virtual int dataSize() = 0;
	};

	class IComArray : public IComObject {
	public:
		virtual int size() = 0;
		virtual IComObject* get(int index) = 0;
		virtual void add(IComObject* o) = 0;
	};

	class VecComArray : public TComObject<IComArray> {
	public:
		VecComArray() {}
		VecComArray(const std::vector<IComObject*>& data) : data(data)
		{
			for (auto o : data)
				if (o) o->addRef();
			infs["IComArray"] = (IComArray*)this;
		}
		~VecComArray()
		{
			for (auto o : data)
				if (o) o->release();
		}
		int size() override {
			return (int)data.size();
		}
		IComObject* get(int index) override {
			if (index < 0 || index >= (int)data.size())
				return nullptr;
			auto o = data[index];
			o->addRef();
			return o;
		}
		void add(IComObject* o) override {
			o->addRef();
			data.push_back(o);
		}
	private:
		std::vector<IComObject*> data;
	};

	const int DT_Float32 = 1;
	const int DT_Int32 = 2;
	const int DT_UInt8 = 3;
	const int DT_Int64 = 4;
	const int DT_Bool = 6;
	const int DT_Int16 = 7;
	const int DT_Float64 = 8;

	class ITorchModel : public IComObject {
	public:
		virtual bool loadModel(const char* path) = 0;
		virtual INdArray* predict(IComArray* inputs) = 0;
	};

	struct ComUtil {
		static IComObject* createObject(const std::string& dllPath, const std::string& className);
		static int getModuleObjectCount(const std::string& dllPath);
		template<typename T>
		static T* createObject(const std::string& dllPath) {
			IComObject* obj = createObject(dllPath, typeid(T).name());
			T* obj1 = nullptr;
			if (obj->queryInterface(typeid(T).name(), (void**)&obj1)) {
                obj->release();
				return obj1;
			}
			obj->release();
			std::string msg = std::string("Cannot find interface ") + typeid(T).name();
			throw std::runtime_error(msg.c_str());
		}
	};
}
