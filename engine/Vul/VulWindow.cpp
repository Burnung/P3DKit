//
// Created by bunring on 19-3-11.
//
#if VULKAN_ENABLE
#pragma once
#define GLFW_INCLUDE_VULKAN

#include "common/P3D_Utils.h"
#include"../RenderCommon.h"
#include "VulWindow.h"
#include "../common/Template_Utils.h"

BEGIN_P3D_NS

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation" };
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

VulWindow::~VulWindow() {

}

void VulWindow::release() {
	vkDestroyInstance(instance, nullptr);
	if (enableValidationLayer) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, nullptr);
	}
	glfwDestroyWindow(win.get());
}
std::vector<const char*> getRequiredExtensions() {
	std::vector<const char*> extensions;

	unsigned int glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	for (unsigned int i = 0; i < glfwExtensionCount; i++) {
		extensions.push_back(glfwExtensions[i]);
	}

	if (enableValidationLayer) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}
std::vector<const char*> VulWindow::getWinValidExtensions() {
	std::vector<const char*> ret;
	if (useGflw) {
		uint32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		ret = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}
	if (enableValidationLayer) {
		ret.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return ret;
}
void VulWindow::createInstance() {

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "just for fun";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 1, 0);
	appInfo.pEngineName = "P3D Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32 extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties>extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	auto enableExtensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32>(enableExtensions.size());
	createInfo.ppEnabledExtensionNames = enableExtensions.data();


	//auto extensions = getRequiredExtensions();
	//createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	//createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
	if (enableValidationLayer)
	{
		logOut.log("Vulkan avaliable extensions is: ");
		for (const auto& ext : extensions) {
			logOut.log(Utils::makeStr("\t %s,version %d", ext.extensionName, ext.specVersion));
		}

		P3D_ENSURE(VulUtils::checkValidationLayerSupport(validationLayers), "validation layers requested, but not available!");
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = VulUtils::debugCallback;
		debugCreateInfo.pUserData = nullptr;
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	P3D_ENSURE(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS,"failed to create instance!");
	setupDebugMessager(debugCreateInfo);
}
VulWindow::VulWindow(uint32 width, uint32 height):P3DWindow(width,height) {
	appType = APP_VULKAN;
	glfwInit();
	if (glfwVulkanSupported()) {
		logOut.log("glfw support vulkan!");
	}
	else {
		logOut.log("dot support vulkan,please use opengl renderer.");
		exit(1);
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);



	win = std::shared_ptr<GLFWwindow>(glfwCreateWindow(width, height, "Facial", nullptr, nullptr));

	if (win == nullptr) {
		logOut.log("Could not create glfw Window \n");
	}
	glfwMakeContextCurrent(win.get());
	initVulkan();
}

void VulWindow::initVulkan() {
	createInstance();
	createSurface();
	pickPhysicalDevice();
	createLogiscDevice();
	createSwapChain();
	createImageViews();

}

void VulWindow::createLogiscDevice() {
	auto index = VulUtils::findQueueFamilies(physicalDevice,surface);

	VkDeviceQueueCreateInfo queueCreateInfo = { };
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = index.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;

	auto queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount =deviceExtensions.size();
	createInfo.ppEnabledExtensionNames =  deviceExtensions.data();

	if (enableValidationLayer) {
		createInfo.enabledLayerCount = static_cast<uint32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	P3D_ENSURE(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) == VK_SUCCESS, "failed to create logic deivce");

	vkGetDeviceQueue(device, index.graphicsFamily.value(), 0, &graphicsQueue);

}
void VulWindow::pickPhysicalDevice(){
	uint32 deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	P3D_ENSURE(deviceCount > 0, "failed to find GPUs with Vulkan support");
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	int32 firstId = -1;
	doFor(k,deviceCount){
		if( VulUtils::isDeviceSuitable(devices[k],surface) && firstId < 0)
			firstId = k;
	}
	P3D_ENSURE(firstId >= 0, "failed to find suitable GPU");
	physicalDevice = devices[firstId];

}
void VulWindow::setupDebugMessager(const VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	if (!enableValidationLayer)
		return;

	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func == nullptr || func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		P3D_THROW_RUNTIME("failed to set up debug messenger!");
	}

}
void VulWindow::createSurface() {
	if (useGflw) {
		P3D_ENSURE(glfwCreateWindowSurface(instance, win.get(), nullptr, &surface) == VK_SUCCESS,
			"glfw failed to create window surface");
	}
	else {
		VulUtils::createSurface(instance, surface);
	}
}
void VulWindow::render() {
	glfwMakeContextCurrent(win.get());
	scene->render();
	glfwPollEvents();
	glfwSwapBuffers(win.get());
}
void VulWindow::createSwapChain() {
	auto swapChanSupport = VulUtils::querySwapChainSupport(physicalDevice, surface);

	auto surfaceFormat = VulUtils::chooseSwapSurfaceFormat(swapChanSupport.formats);
	auto presentMode = VulUtils::chooseSwapPresentMode(swapChanSupport.presentModes);
	auto extent = VulUtils::chooseSwapExtent(swapChanSupport.capabilities,width,height);

	uint32 imageCount = swapChanSupport.capabilities.minImageCount + 1;
	if (swapChanSupport.capabilities.maxImageCount > 0 && imageCount > swapChanSupport.capabilities.maxImageCount) {
		imageCount = swapChanSupport.capabilities.maxImageCount;
	}
	
	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	auto index = VulUtils::findQueueFamilies(physicalDevice,surface);
	uint32 queueFamilyIndicies[] = { index.graphicsFamily.value(),index.presentFamily.value()};
	if (index.graphicsFamily != index.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndicies;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	createInfo.preTransform = swapChanSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = static_cast<VkCompositeAlphaFlagBitsKHR>(swapChanSupport.capabilities.supportedCompositeAlpha);
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
	createInfo.oldSwapchain = VK_NULL_HANDLE;

//	PreCallValidateCreateSwapchainKHR()
	P3D_ENSURE(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) == VK_SUCCESS, "failed to create swap chain");

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExten = extent;

}

void VulWindow::createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());
	for (int i = 0; i < swapChainImages.size(); ++i) {
		VkImageViewCreateInfo creatInfo = {};
		creatInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		creatInfo.image = swapChainImages[i];
		creatInfo.format = swapChainImageFormat;
		creatInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		creatInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		creatInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		creatInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		creatInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		creatInfo.subresourceRange.baseMipLevel = 0;
		creatInfo.subresourceRange.levelCount = 1;
		creatInfo.subresourceRange.baseArrayLayer = 0;
		creatInfo.subresourceRange.layerCount = 1;
		creatInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		P3D_ENSURE(vkCreateImageView(device, &creatInfo, nullptr, &swapChainImageViews[i]) == VK_SUCCESS, "failed to create iamge view ");

	}
}
END_P3D_NS
#endif
