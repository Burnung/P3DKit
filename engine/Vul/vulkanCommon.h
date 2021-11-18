#if DVULKAN_ENABLE
#pragma once
#include <vulkan/vulkan.hpp>
#include "../RenderCommon.h"
#include"../common/Template_Utils.h"
BEGIN_P3D_NS

#ifdef NDEBUG
const bool enableValidationLayer = false;
#else
const bool enableValidationLayer = true;
#endif
struct QueueFamilyIndices{
	std::optional<uint32> graphicsFamily;
	std::optional<uint32> presentFamily;
	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulUtils {
public:
	static bool checkValidationLayerSupport(std::vector<const char*> vidationNames);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice& device,const VkSurfaceKHR& surface);
	static bool isDeviceSuitable(VkPhysicalDevice&device,VkSurfaceKHR& surface);
	static void createSurface(VkInstance& instance,VkSurfaceKHR& surface);
	static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device,const VkSurfaceKHR& surface);
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& avaliableFormats);
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modes);
	static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,uint32 width,uint32 height);

public:
	int a = 0;
};
END_P3D_NS
#endif
