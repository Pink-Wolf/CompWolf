#include <private/vulkan_programs/vulkan_gpu_program_manager.hpp>

#include "compwolf_vulkan.hpp"
#include <stdexcept>

namespace compwolf::vulkan
{
	static auto find_thread(const vulkan_gpu_thread_family& family) noexcept -> std::size_t
	{
		std::size_t best_thread_index = 0;
		if (family.threads[best_thread_index].program_manager_count == 0) return best_thread_index;

		for (size_t thread_index = 1; thread_index < family.threads.size(); ++thread_index)
		{
			auto program_manager_count = family.threads[best_thread_index].program_manager_count;
			if (program_manager_count < family.threads[best_thread_index].program_manager_count)
			{
				best_thread_index = thread_index;
				if (program_manager_count == 0) return thread_index;
			}
		}
		return best_thread_index;
	}
	static auto find_family(gpu_program_manager_settings& settings, const vulkan_gpu_connection& gpu,
		float& best_score, float& best_custom_score) noexcept
		-> std::optional<std::size_t>
	{
		const vulkan_gpu_thread_family* best_family = nullptr;
		std::size_t best_family_index = 0;
		best_score = std::numeric_limits<float>::lowest();

		for (std::size_t family_index = 0; family_index < gpu.thread_families().size(); ++family_index)
		{
			auto& family = gpu.thread_families()[family_index];

			auto additional_work_types = family.work_types ^ settings.type;
			if ((additional_work_types & settings.type).any()) continue;

			std::optional<float> custom_score_container = settings.family_scorer ? settings.family_scorer(family) : std::optional<float>(0.f);
			if (!custom_score_container.has_value()) continue;
			auto custom_score = custom_score_container.value();

			float score = 0.f;
			{
				score -= additional_work_types.count() / static_cast<float>(gpu_work_type::size);

				score -= static_cast<float>(family.program_manager_count / family.threads.size()); // Truncation of "job-count / thread-count" is intentional
			}

			constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_work_type::size);
			bool is_better = score > best_score + very_small_score_difference;
			if (best_score - score < very_small_score_difference) is_better = is_better || custom_score > best_custom_score; // use custom_score if scores are same

			if (is_better)
			{
				best_family = &family;
				best_family_index = family_index;
				best_score = score;
				best_custom_score = custom_score;
			}
		}

		if (!best_family) return std::nullopt;
		return best_family_index;
	}
	static auto find_family(gpu_program_manager_settings& settings, const std::vector<vulkan_gpu_connection>& gpus) noexcept
		-> std::optional<std::pair<size_t, std::size_t>>
	{
		std::size_t best_gpu_index = 0;
		std::size_t best_family_index = 0;
		float best_score = std::numeric_limits<float>::lowest();
		float best_custom_score = std::numeric_limits<float>::lowest();

		for (auto& gpu : gpus)
		{
			auto additional_work_types_for_gpu = gpu.work_types() ^ settings.type;
			if ((additional_work_types_for_gpu & settings.type).any()) continue;

			std::optional<float> custom_gpu_score_container = settings.gpu_scorer ? settings.gpu_scorer(gpu) : std::optional<float>(0.f);
			if (!custom_gpu_score_container.has_value()) continue;
			auto custom_gpu_score = custom_gpu_score_container.value();

			float score, custom_score;
			auto index_container = find_family(settings, gpu, score, custom_score);
			custom_score += custom_gpu_score;

			if (!index_container.has_value()) continue;
			auto family_index = index_container.value();

			constexpr float very_small_score_difference = .1f / static_cast<float>(gpu_work_type::size);
			bool is_better = score > best_score + very_small_score_difference;
			if (best_score - score < very_small_score_difference) is_better = is_better || custom_score > best_custom_score; // use custom_score if scores are same

			if (is_better)
			{
				best_family_index = family_index;
				best_score = score;
				best_custom_score = custom_score;
			}
		}

		return std::make_pair(best_gpu_index, best_family_index);
	}

	/******************************** constructors ********************************/

	vulkan_gpu_program_manager::vulkan_gpu_program_manager(vulkan_gpu_connection& gpu_in
		, std::size_t family_index_in, std::size_t thread_index_in)
		: _gpu(&gpu_in)
		, _family_index(family_index_in)
		, _thread_index(thread_index_in)
		, _syncs_count(0)
	{
		auto logicDevice = to_vulkan(gpu().vulkan_device());

		VkCommandPool commandPool;
		{
			VkCommandPoolCreateInfo createInfo{
				.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
				.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
				.queueFamilyIndex = static_cast<uint32_t>(thread_family_index()),
			};

			auto result = vkCreateCommandPool(logicDevice, &createInfo, nullptr, &commandPool);

			switch (result)
			{
			case VK_SUCCESS: break;
			default:
				const char* message;
				GET_VULKAN_ERROR_STRING(result, message,
					"Could not set up a \"command pool\" for managing gpu-programs: ")
					throw std::runtime_error(message);
			}

			auto& family = gpu_in.thread_families()[_family_index];
			auto& thread = family.threads[_thread_index];
			_pool = unique_deleter_ptr<vulkan_handle::command_pool_t>(from_vulkan(commandPool),
				[logicDevice, &family, &thread](vulkan_handle::command_pool c)
				{
					vkDeviceWaitIdle(logicDevice);
					vkDestroyCommandPool(logicDevice, to_vulkan(c), nullptr);

					--family.program_manager_count;
					--thread.program_manager_count;
				}
			);
			++family.program_manager_count;
			++thread.program_manager_count;
		}
	}

	auto vulkan_gpu_program_manager::new_manager_for(vulkan_gpu_connection& gpu
		, gpu_program_manager_settings settings) -> vulkan_gpu_program_manager
	{
		float a, b;
		auto i = find_family(settings, gpu, a, b);
		if (!i) throw std::runtime_error("The machine's GPUs could not perform a job because of the type of work it requires.");

		auto family_index = i.value();
		auto thread_index = find_thread(gpu.thread_families()[family_index]);
		return vulkan_gpu_program_manager(gpu, family_index, thread_index);
	}
	auto vulkan_gpu_program_manager::new_manager_for(vulkan_graphics_environment& environment
		, gpu_program_manager_settings settings) -> vulkan_gpu_program_manager
	{
		auto& gpus = environment.gpus();
		auto i = find_family(settings, gpus);
		if (!i) throw std::runtime_error("The machine's GPUs could not perform a job because of the type of work it requires.");

		auto& [gpu_index, family_index] = i.value();
		auto& gpu = gpus[gpu_index];
		auto thread_index = find_thread(gpu.thread_families()[family_index]);
		return vulkan_gpu_program_manager(gpu, family_index, thread_index);
	}

	/******************************** modifiers ********************************/
	auto vulkan_gpu_program_manager::new_synchronization(bool signaled) noexcept -> gpu_program_sync&
	{
		++_syncs_count;
		if (_syncs_count > _syncs.size())
			_syncs.emplace_back(vulkan_gpu_semaphore(gpu()), vulkan_gpu_fence(gpu(), signaled));
		else
			if (!signaled) latest_synchronization()->fence.reset(); // note semaphore does not need to be reset to be re-used
		return *latest_synchronization();
	}
}
