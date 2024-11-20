#include "private/vulkan_graphics_environments/vulkan_gpu_connection.hpp"
#include "compwolf_vulkan.hpp"

#include <stdexcept>
#include <algorithm>

namespace compwolf::vulkan
{
	vulkan_gpu_connection::vulkan_gpu_connection(
		vulkan_handle::instance vulkan_instance,
		vulkan_handle::physical_device vulkan_physical_device)
		: _vulkan_instance(vulkan_instance), _vulkan_physical_device(vulkan_physical_device)
	{
		auto instance = to_vulkan(vulkan_instance);
		auto physicalDevice = to_vulkan(vulkan_physical_device);

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);

		auto extensionProperties = get_size_and_vector<uint32_t, VkExtensionProperties>(
			[physicalDevice](uint32_t* size, VkExtensionProperties* data)
			{
				auto result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, size, data);
				switch (result)
				{
				case VK_SUCCESS:
				case VK_INCOMPLETE:
					break;
				default: throw std::runtime_error("Could not get extensions from gpu");
				}
			}
		);
		std::vector<const char*> enabled_extensions;
		VkPhysicalDeviceFeatures enabled_features{};

		bool has_swapchain_extension = std::any_of(
			extensionProperties.cbegin(),
			extensionProperties.cend(),
			[](VkExtensionProperties a) { return 0 == strcmp(a.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME); }
		);

		bool is_present_device = has_swapchain_extension && features.samplerAnisotropy;
		if (is_present_device)
		{
			enabled_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			enabled_features.samplerAnisotropy = VK_TRUE;
		}

		const float queue_priority_item = .0f;
		std::vector<float> queue_priority(8, queue_priority_item);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		{
			auto queueFamilies = get_size_and_vector<uint32_t, VkQueueFamilyProperties>(
				[physicalDevice](uint32_t* size, VkQueueFamilyProperties* data) { vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, size, data); }
			);

			queueCreateInfos.reserve(queueFamilies.size());
			_thread_families.reserve(queueFamilies.size());

			for (std::size_t queue_index = 0; queue_index < queueFamilies.size(); queue_index++)
			{
				auto& queueFamily = queueFamilies[queue_index];

				vulkan_gpu_thread_family connection;

				bool draw_queue = queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
				if (draw_queue) connection.work_types[gpu_work_type::draw] = true;

				bool present_queue = is_present_device
					&& glfwGetPhysicalDevicePresentationSupport(instance, physicalDevice, static_cast<uint32_t>(queue_index));
				if (present_queue) connection.work_types[gpu_work_type::present] = true;

				auto queue_count = queueFamily.queueCount;
				connection.threads.resize(queue_count);
				if (queue_priority.size() < queue_count) queue_priority.resize(queue_count, queue_priority_item);

				VkDeviceQueueCreateInfo queueCreateInfo{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = static_cast<uint32_t>(queue_index),
					.queueCount = static_cast<uint32_t>(queue_count),
					.pQueuePriorities = queue_priority.data(),
				};

				_work_types |= connection.work_types;
				queueCreateInfos.push_back(std::move(queueCreateInfo));
				_thread_families.push_back(std::move(connection));
			}
		}

		VkDeviceCreateInfo createInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
			.pQueueCreateInfos = queueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size()),
			.ppEnabledExtensionNames = enabled_extensions.data(),
			.pEnabledFeatures = &enabled_features
		};

		VkDevice logicDevice;
		auto result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicDevice);

		switch (result)
		{
		case VK_SUCCESS: break;
		case VK_ERROR_EXTENSION_NOT_PRESENT: throw std::runtime_error("Could not set up a connection to a gpu; the machine does not have the right extensions.");
		case VK_ERROR_FEATURE_NOT_PRESENT: throw std::runtime_error("Could not set up a connection to a gpu; the machine does not have the right features.");
		default: throw std::runtime_error("Could not set up a connection to a gpu.");
		}

		_vulkan_device = from_vulkan(logicDevice);

		for (uint32_t family_index = 0; family_index < _thread_families.size(); ++family_index)
		{
			auto& family = _thread_families[family_index];
			for (uint32_t thread_index = 0; thread_index < family.threads.size(); ++thread_index)
			{
				auto& thread = family.threads[thread_index];

				VkQueue queue;
				vkGetDeviceQueue(logicDevice, family_index, thread_index, &queue);
				thread.queue = from_vulkan(queue);
			}
		}
	}

	/******************************** CompWolf::freeable ********************************/

	void vulkan_gpu_connection::free() noexcept
	{
		if (empty()) return;

		vkDestroyDevice(to_vulkan(_vulkan_device), nullptr);
		_vulkan_device = nullptr;
	}
}
