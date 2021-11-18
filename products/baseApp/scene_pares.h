#pragma once
#include "common/P3D_Utils.h"
#include "engine/P3dEngine.h"
#include "engine/mgs/sceneManager.hpp"
#include <unordered_set>
#include "common/json_inc.h"

BEGIN_P3D_NS
class UIBase :public P3DObject {
public:
	UIBase(cstring tName) :uiName(tName) {};
	~UIBase() = default;
	virtual void apply() = 0;
	virtual void setValue(const Value& v) { value = v; }
	virtual void setValueName(const std::string& tName) {
		valueName = tName;
		auto tScene = P3DEngine::instance().getSceneManager();
		if (!tScene->getValue(tName, value)) {
			tScene->addValue(tName, value);
		}
	}
public:
	std::string uiName;
	std::string valueName;
	Value value;
};
typedef std::shared_ptr<UIBase> pUIBase;

class UISlider :public UIBase {
public:
	UISlider(cstring uiName, float v, float lB = 0.0, float uB = 1.0) :UIBase(uiName), lowBound(lB), upBound(uB) {
		value = Value(v);
	}

	UISlider(cstring uiName, cstring valueName, float lB = 0.0, float uB = 1.0) :UIBase(uiName), lowBound(lB), upBound(uB) {
		if (!P3DEngine::instance().getSceneManager()->getValue(valueName, value))
			value = Value(0.0f);
		P3DEngine::instance().getSceneManager()->addValue(valueName, value);
		
	}

	virtual void apply();

public:

	//Value value;
	float lowBound = 0.0;
	float upBound = 1.0;
};

class UICheckBox :public UIBase {
public:
	UICheckBox(cstring uiName, float v) :UIBase(uiName){
		value = Value(v);
	}

	UICheckBox(cstring uiName, cstring valueName) :UIBase(uiName) {
		if (!P3DEngine::instance().getSceneManager()->getValue(valueName, value)) 
			value = Value(false);
		P3DEngine::instance().getSceneManager()->addValue(valueName, value);
		
	}
	virtual void apply();
};

class UIButton : public UIBase {
public:
	UIButton(cstring uiName,float v):UIBase(uiName) {
		value = Value(v);
		P3DEngine::instance().getSceneManager()->addValue(uiName, value);
	}

	UIButton(cstring uiName,cstring valueName) :UIBase(uiName){
		if (!P3DEngine::instance().getSceneManager()->getValue(valueName, value)) 
			value = Value(1.0f);
		P3DEngine::instance().getSceneManager()->addValue(valueName, value);
	}
	virtual void apply();
};

class UIColorEditer :public UIBase {
public:
	UIColorEditer(cstring uiName, const Vec4f& v) :UIBase(uiName) {
		value = Value(v);
	}

	UIColorEditer(cstring uiName, cstring valueName) :UIBase(uiName) {
		if (!P3DEngine::instance().getSceneManager()->getValue(valueName, value)) {
			value = Value(Vec4f(0.0f,0.0f,0.0f,1.0f));
			P3DEngine::instance().getSceneManager()->addValue(valueName, value);
		}
	}
	virtual void apply();
};

class UIManager :public P3DObject {
public:
	UIManager() = default;
private:
	void addDefaultSliders(const P3djson& jsonObj);
public:
	void addWidgetFormJsonObj(const P3djson& jsonObj);


	void paraseFromFile(const std::string& fpath);
	void paraseFromJson(const P3djson& jsonObj);
	void addSlider(const std::string& tName, float initV, float lBound = 0.0f, float upBound = 1.0f);
	void addSlider(const std::string& tName, cstring valueName, float lBound = 0.0f, float upBound = 1.0f);
	
	void addSliderList(const std::vector<std::string>& sliderNames, const std::vector<float>& values = {});

	void addCheckBox(const std::string& tName, bool initValue);
	void addCheckBox(cstring& tName, cstring valueName);
	
	void addButton(const std::string& tName,float v);
	void addButton(const std::string& tName, cstring valueName);

	void addColorEditer(cstring& tName, const Vec4f& initV);
	void addColorEditer(cstring& tName, cstring initV);
	
	void addPanelUIs(cstring& pannelName, const std::vector<std::string>& uiNames);
	void addPanelUI(cstring& pannelName, cstring uiName);

	void apply();
	void apply0();

private:
	std::unordered_map<std::string, pUIBase> allUIBase;
	std::vector<std::string> uiPannels;
	std::unordered_set<std::string> uiPannelSets;
	std::unordered_map<std::string, std::vector<std::string>> pannelUIs;
};

using pUIManager=std::shared_ptr<UIManager>;
END_P3D_NS
