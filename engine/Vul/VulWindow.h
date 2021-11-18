//
// Created by bunring on 19-3-11.
//
#if VULKAN_ENABLE
#pragma once


#include "common/P3D_Utils.h"
#include"../RenderCommon.h"
#include "../Window.h"
#include <GLFW/glfw3.h>
#include "vulkanCommon.h"
BEGIN_P3D_NS

class VulWindow :public P3DWindow {
public:
	virtual ~VulWindow();
public:
	explicit VulWindow(uint32 width, uint32 height);
	//virtual void addComp(pComp comp, pShader shader) override;
	virtual void render();
		virtual void* getWinHandler() { return win.get(); }
private:
	void initVulkan();

	void createInstance();
	void release();
	std::vector<const char*> getWinValidExtensions();
	void setupDebugMessager(const VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void pickPhysicalDevice();
	void createLogiscDevice();
	void createSurface();
	void createSwapChain();
	void createImageViews();
private:
	std::shared_ptr<GLFWwindow> win;
	bool useGflw = true;
	VkInstance instance;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExten;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDebugReportCallbackEXT callback;
};
typedef std::shared_ptr<VulWindow> pVulWindow;

END_P3D_NS
#endif