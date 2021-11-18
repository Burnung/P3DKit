#include "scene_pares.h"
#include "libs/imgui/myImgui.h"
BEGIN_P3D_NS

void UISlider::apply() {
	ImGui::SliderFloat(uiName.c_str(), (float*)value.rawData->rawData(), lowBound, upBound);
	float v = *(float*)(value.rawData->rawData());
	int aaa = 0;
}
void UICheckBox::apply() {
	ImGui::Checkbox(uiName.c_str(), (bool*)value.rawData->rawData());
}
void UIColorEditer::apply() {
	ImGui::ColorEdit4(uiName.c_str(),(float*)value.rawData->rawData());
}
void UIButton::apply() {
	if (ImGui::Button(uiName.c_str())) {
		float32 tmpV = value.toFloats()[0];
		tmpV += 1.0;
		value.setValue(&tmpV);
	}
}
void UIManager::addSlider(const std::string& tName, float initV, float lBound , float upBound) {
	auto tSilder = std::make_shared<UISlider>(tName, initV, lBound, upBound);
	allUIBase[tName] = tSilder;
}

void UIManager::addSlider(const std::string& tName, cstring valueName, float lBound , float upBound ) {
	auto tSlider = std::make_shared<UISlider>(tName, valueName, lBound, upBound);
	allUIBase[tName] = tSlider;
}

void UIManager::addSliderList(const std::vector<std::string>& sliderNames, const std::vector<float>& values) {
	if (values.size() != sliderNames.size()) {
		for (const auto& tName : sliderNames)
			addSlider(tName, tName + "_val");
	}
	else {
		for (int i = 0; i < sliderNames.size();++i) {
			addSlider(sliderNames[i], values[i]);
		}
	}
}

void UIManager::addCheckBox(const std::string& tName, bool initValue) {
	auto tUi = std::make_shared<UICheckBox>(tName, initValue);
	allUIBase[tName] = tUi;
}

void UIManager::addCheckBox(cstring& tName, cstring valueName) {
	auto tUi = std::make_shared<UICheckBox>(tName,valueName);
	allUIBase[tName] = tUi;
}

void UIManager::addButton(const std::string& tName,float v) {
	auto tUi = std::make_shared<UIButton>(tName,v);
	allUIBase[tName] = tUi;
}
void UIManager::addButton(const std::string& tName, cstring valueName) {
	auto tUi = std::make_shared<UIButton>(tName, valueName);
	allUIBase[tName] = tUi;
}
void UIManager::addColorEditer(cstring& tName, const Vec4f& initV) {
	auto tUi = std::make_shared<UIColorEditer>(tName,initV);
	allUIBase[tName] = tUi;
}

void UIManager::addColorEditer(cstring& tName, cstring vName) {
	auto tUi = std::make_shared<UIColorEditer>(tName,vName);
	allUIBase[tName] = tUi;
}

void UIManager::addPanelUIs(cstring& pannelName, const std::vector<std::string>& uiNames) {
	for (const auto& tName : uiNames) 
		addPanelUI(pannelName, tName);
}
void UIManager::addPanelUI(cstring& pannelName, cstring uiName) {
	if (uiPannelSets.count(pannelName)==0) {
		uiPannelSets.insert(pannelName);
		uiPannels.push_back(pannelName);
	}
	pannelUIs[pannelName].push_back(uiName);

}

void UIManager::apply() {
	for (const auto& pannelName : uiPannels) {
		if(pannelUIs.count(pannelName) == 0)
			continue;
		ImGui::Begin(pannelName.c_str());
		for (const auto& uiName : pannelUIs[pannelName]) {
			allUIBase[uiName]->apply();
		}
		ImGui::End();
	}
}
void UIManager::paraseFromFile(const std::string& fpath) {
	auto jsonStr = Utils::readFileString(fpath);
	auto jsonObj = P3djson::parse(jsonStr);
	paraseFromJson(jsonObj);
}

void UIManager::addDefaultSliders(const P3djson& jsonObj) {
	for (int i = 0; i < jsonObj.size(); ++i) {
		std::string tName = jsonObj[i];
		addSlider(tName, tName, 0.0, 1.0);
	}
}
void UIManager::addWidgetFormJsonObj(const P3djson& jsonObj) {
	std::string dType = jsonObj["type"];
	std::string tName = jsonObj["name"];
	if (dType == "slider") {
		float v = JsonUtil::get(jsonObj, "value", 0.0f);
		float lb = JsonUtil::get(jsonObj, "boundL", 0.0f);
		float ub = JsonUtil::get(jsonObj, "boundU", 1.0f);
		addSlider(tName, v, lb, ub);
	}
	else if (dType == "colorEditer") {
		std::vector<float> v = JsonUtil::get(jsonObj, "value", std::vector<float>(4, 0.0f));
		Vec4f tv(v);
		addColorEditer(tName, tv);
	}
	else if (dType == "checkBox") {
		float v = JsonUtil::get(jsonObj, "value", 0.0f);
		bool bv = v > 0.5 ? true : false;
		addCheckBox(tName, bv);
	}
	else if (dType == "button") {
		float v = JsonUtil::get(jsonObj, "value", 0.0f);
		addButton(tName, v);
	}
	std::string valueName = JsonUtil::get(jsonObj, "valueName", "");
	if (!valueName.empty()) {
		allUIBase[tName]->setValueName(valueName);
	}
}
void UIManager::paraseFromJson(const P3djson& jsonObj) {
	for (const auto& obj : jsonObj["widgets"])
		addWidgetFormJsonObj(obj);
	if (jsonObj.count("default_sliders")) {
		addDefaultSliders(jsonObj["default_sliders"]);
	}
	const auto& tPannels = jsonObj["pannels"];
	int i = tPannels.size();
	for (auto tObj = tPannels.begin(); tObj != tPannels.end(); ++tObj) {
		std::string tName = tObj.key();
		std::vector<std::string> values = tObj.value();
		addPanelUIs(tName, values);
	}

}
void apply0() {

}
END_P3D_NS