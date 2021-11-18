
#if VULKAN_ENABLE

#include "vulkanCommon.h"
#include <vector>
#include <algorithm>
#include "../common/Template_Utils.h"

#ifdef IS_WINDOWS
#include <vulkan/vulkan_win32.h>
#include <libloaderapi.h>
#endif

BEGIN_P3D_NS


bool VulUtils::checkValidationLayerSupport(std::vector<const char*> validationNames) {
	uint32 layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> avaliableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());

	for (auto layerName : validationNames) {
		auto layerFound = false;
		for (const auto& layerProperties : avaliableLayers) {
			if (std::string(layerName) == layerProperties.layerName) {
				layerFound = true;
				break;
			}
		}
		if (!layerCount)
			return false;
	}
	return true;
}
VKAPI_ATTR VkBool32 VKAPI_CALL VulUtils::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		logOut.log(Utils::makeStr("vulkan error validataion layer : %s", pCallbackData->pMessage));
	return VK_FALSE;
}

QueueFamilyIndices VulUtils::findQueueFamilies(VkPhysicalDevice& device,const VkSurfaceKHR& surface) {
	QueueFamilyIndices index;
	uint32 queCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queCount, nullptr);
	std::vector<VkQueueFamilyProperties> properties(queCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queCount, properties.data());
	
	int i = 0;
	for (const auto property : properties) {
		if (property.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			index.graphicsFamily = i;
		VkBool32 presentSupport = false;
		auto ret = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (presentSupport)
			index.presentFamily = i;

		if(index.isComplete())
			break;
		++i;
	}
	return index;
}

bool VulUtils::isDeviceSuitable(VkPhysicalDevice&device,VkSurfaceKHR& surface) {
	auto index = findQueueFamilies(device,surface);

	return index.isComplete();
}

void VulUtils::createSurface(VkInstance& instance, VkSurfaceKHR& surface) {
#ifdef IS_WINDOWS
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = nullptr;
	createInfo.hinstance = GetModuleHandle(nullptr);
	P3D_ENSURE(vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) == VK_SUCCESS,
		"failed to create surface");
#endif

#ifdef IS_LINUX

#endif
}

SwapChainSupportDetails VulUtils::querySwapChainSupport(const VkPhysicalDevice& device,const VkSurfaceKHR& surface) {
	SwapChainSupportDetails details;
	auto ret = 	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	
	uint32 formatCount;
	ret = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount > 0) {
		details.formats.resize(formatCount);
		ret = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}
	uint32 presentModeCount;
	ret = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount > 0) {
		details.presentModes.resize(presentModeCount);
		ret = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}
	return details;

}
VkSurfaceFormatKHR VulUtils::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaliableFormats) {
	for (const auto& it : avaliableFormats) {
		if (it.format == VK_FORMAT_B8G8R8A8_UNORM && it.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return it;
	}
	return avaliableFormats[0];
}
VkPresentModeKHR VulUtils::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	for (const auto& it : presentModes) {
		if (it == VK_PRESENT_MODE_MAILBOX_KHR)
			return it;
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D VulUtils::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,uint32 winWidth,uint32 winHeight) {
	if (capabilities.currentExtent.width != UINT32_MAX)
		return capabilities.currentExtent;
	else {
		VkExtent2D actualExtent = { winWidth,winHeight };
		actualExtent.width = std::max(capabilities.minImageExtent.width,std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height,std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;

	}
}
END_P3D_NS
#endif