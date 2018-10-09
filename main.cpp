#define _VKMOD_IMPLEMENTATION
#include "include/vkmod.hpp"
#include <iostream>
#define GLFW_USE_VULKAN
#include <GLFW/glfw3.h>
using namespace std;

int main() {
	cout << "Hello!" << endl;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* win = glfwCreateWindow(800, 600, "vkren", nullptr, nullptr);

	vkmod::push(vk::ApplicationInfo()
		.setApiVersion(VK_API_VERSION_1_0)
		.setApplicationVersion(VK_MAKE_VERSION(0, 0, 0))
		.setEngineVersion(VK_MAKE_VERSION(0, 0, 0))
		.setPApplicationName("sircad")
		.setPEngineName("sircadengine")
	);

	uint32_t count;
	const char** v = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> extensions;
	for(int i=0;i<count;i++) {
		extensions.push_back(v[i]);
		//vkmod::push ("extensions", v[i]);
	}

	std::vector<const char*> layers = {
		"VK_LAYER_LUNARG_core_validation"
	};


	vkmod::push ("layers", {"VK_LAYER_LUNARG_core_validation"});
	vkmod::push ("extensions", extensions);

	vkmod::push ("surface", [win](vk::Instance in) {
		VkInstance inst = in;
		VkSurfaceKHR surf;
		glfwCreateWindowSurface(inst, win, nullptr, &surf);
		return vk::SurfaceKHR(surf);
	});

	vkmod::init();

	auto gpus = vkmod::ls<vk::PhysicalDevice>();
	vkmod::push(gpus[0]);
	//auto dev = vkmod::get<vk::Device>();
	std::cout << "Count gpus: " << gpus.size() << std::endl;
	auto gpu = gpus[0];
	auto families = gpu.getQueueFamilyProperties();
	for (uint32_t idx=0; idx<families.size();idx++) {
		//VK_QUEUE_TRANSFER_BIT
		std::cout << "Queue count: " << families[idx].queueCount << std::endl;
		if(families[idx].queueFlags & vk::QueueFlagBits(VK_QUEUE_TRANSFER_BIT))
			std::cout << "Queue supports transfer\n";
	}
	uint32_t fam_idx = 0;

	float one = 1.0f;
	vk::DeviceQueueCreateInfo inf = vk::DeviceQueueCreateInfo()
		.setQueueCount(1)
		.setPQueuePriorities(&one)
		.setQueueFamilyIndex(fam_idx);

	vk::Device dev = gpu.createDevice(vk::DeviceCreateInfo()
		.setQueueCreateInfoCount(1)
		.setPQueueCreateInfos(&inf)
	);

	std::cout << "Device: " << dev << std::endl;

	vk::Queue que = dev.getQueue(0, 0);

	std::cout << "Queue " << que << std::endl;

	auto func = vkmod::get<std::function<vk::SurfaceKHR(vk::Instance)>>
			("surface");

	auto surf = func(vkmod::get<vkmod::Package<vk::Instance>>().vkobj);
	std::cout << "SURFACE: " << surf << std::endl;

	vk::SwapchainKHR chain = dev.createSwapchainKHR(vk::SwapchainCreateInfoKHR()
		.setImageArrayLayers(1)
		.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear)
		.setImageExtent(vk::Extent2D()
			.setWidth(800)
			.setHeight(600)
		)
		.setImageFormat(vk::Format::eR8G8B8A8Unorm)
		.setImageSharingMode(vk::SharingMode::eConcurrent)
		.setImageUsage(vk::ImageUsageFlagBits::eTransferSrc)
		.setMinImageCount(1)
		.setSurface(surf)
		.setQueueFamilyIndexCount(1)
		.setPQueueFamilyIndices(&fam_idx)
		.setPresentMode(vk::PresentModeKHR::eFifo)
		.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
	);

	std::cout << "CHAIN: " << chain << std::endl;
	auto imgs = dev.getSwapchainImagesKHR(chain);
	std::cout << "IMAGES SIZE: " << imgs.size() << std::endl;

	vk::CommandPool cmdpool = dev.createCommandPool(vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(fam_idx)
	);

	auto cmdbufs = dev.allocateCommandBuffers(vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(imgs.size())
		.setCommandPool(cmdpool)
		.setLevel(vk::CommandBufferLevel::ePrimary)
	);

	std::cout << "CommandBuffer: " << cmdbufs.size() << std::endl;

	auto ran = vk::ImageSubresourceRange()
			.setBaseArrayLayer(0)
			.setLayerCount(1)
			.setBaseMipLevel(0)
			.setLevelCount(1)
			.setAspectMask(vk::ImageAspectFlagBits::eColor);

	uint32_t i=0;
	for (auto cmdbuf : cmdbufs) {
		vk::ImageMemoryBarrier bar;
		bar.setSrcAccessMask(vk::AccessFlagBits::eMemoryRead)
		.setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
		.setOldLayout(vk::ImageLayout::eUndefined)
		.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
		.setSrcQueueFamilyIndex(fam_idx)
		.setDstQueueFamilyIndex(fam_idx)
		.setImage(imgs[i])
		.setSubresourceRange(ran);
		cmdbuf.begin(vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eSimultaneousUse)
		);
		cmdbuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &bar);
		cmdbuf.clearColorImage(imgs[i], vk::ImageLayout::eTransferDstOptimal, vk::ClearColorValue().setFloat32(std::array<float, 4>{0.2, 0.4, 1}), {ran});


		vk::ImageMemoryBarrier bar2;
		bar2.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
		.setDstAccessMask(vk::AccessFlagBits::eTransferRead)
		.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
		.setNewLayout(vk::ImageLayout::ePresentSrcKHR)
		.setSrcQueueFamilyIndex(fam_idx)
		.setDstQueueFamilyIndex(fam_idx)
		.setImage(imgs[i])
		.setSubresourceRange(ran);


		cmdbuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &bar2);


		cmdbuf.end();
		i++;
	}

	vk::Semaphore sem0 = dev.createSemaphore(vk::SemaphoreCreateInfo());
	vk::Semaphore sem1 = dev.createSemaphore(vk::SemaphoreCreateInfo());
	vk::Fence mfen = dev.createFence(vk::FenceCreateInfo());

	while(!glfwWindowShouldClose(win)) {
		glfwPollEvents();
		vkmod::loop();

		uint32_t img_idx = dev.acquireNextImageKHR(chain, UINT64_MAX, sem0, mfen).value;

		que.submit({vk::SubmitInfo().setCommandBufferCount(1).setPCommandBuffers(&cmdbufs[img_idx])}, mfen);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		que.presentKHR(vk::PresentInfoKHR()
			.setSwapchainCount(1)
			.setPSwapchains(&chain)
			.setPImageIndices(&img_idx)
		);

		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	return 0;
}
