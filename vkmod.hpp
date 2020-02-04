#include <string>

#define VULKAN_INCLUDE <vulkan/vulkan.hpp>

#include VULKAN_INCLUDE

namespace vkmod
{
	namespace layer
	{
		namespace google
		{
			std::string unique_objects = "VK_LAYER_GOOGLE_unique_objects";
		}
		namespace lunarg
		{
			std::string api_dump = "VK_LAYER_LUNARG_device_limits";
			std::string core_validation = "VK_LAYER_LUNARG_image";
			std::string image = "VK_LAYER_LUNARG_image";
			std::string object_tracker = "VK_LAYER_LUNARG_object_tracker";
			std::string parameter_validation = "VK_LAYER_LUNARG_parameter_validation";
			std::string swapchain = "VK_LAYER_LUNARG_swapchain";
			std::string threading = "VK_LAYER_GOOGLE_threading";
			std::string standard_validation = "VK_LAYER_LUNARG_standard_validation";
			std::string device_limits = "VK_LAYER_LUNARG_device_limits";
		}

	}

	struct Ctx;
}

#define VKMOD_IMPL
#ifdef VKMOD_IMPL
namespace vkmod
{
	struct Ctx
	{
		vk::Instance inst;

		Ctx(vk::InstanceCreateInfo instance_info)
		{
			inst = vk::createInstance(instance_info);

		}
	}
}
#endif
