#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM_MANAGER
#define COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM_MANAGER

#include <vulkan_graphics_environments>
#include "vulkan_gpu_fence.hpp"
#include "vulkan_gpu_semaphore.hpp"
#include <unique_deleter_ptr>
#include <cstddef>
#include <optional>

namespace compwolf::vulkan
{
	/** Aggregate type containing data used by [[vulkan_gpu_program_manager]] for synchronization between its work. */
	struct gpu_program_sync
	{
		/** Semaphore waiting for all work to be done. */
		vulkan_gpu_semaphore semaphore;
		/** Fence waiting for all work to be done. */
		vulkan_gpu_fence fence;
	};

	/** Aggregate type used by gpu_manager.new_job to specify the job to create. */
	struct gpu_program_manager_settings
	{
		/** The type of work the programs should be able to perform. */
		gpu_work_type_set type;
		/** Used on each thread's gpu to determine what thread to use.
		 * This and family_scorer's scores are summed to determine the threads' scores; the thread with the highest score is generally picked.
		 * gpu_program_manager::new_manager_for may still pick a lower-scoring thread, for example when the highest scoring thread is already busy with work.
		 * If this returns std::nullopt, no thread on the given gpu will be picked; this should only be done if the gpu actually cannot perform the work.
		 * If empty, gpu_program_manager::new_manager_for will score the gpus 0.
		 */
		std::function<std::optional<float>(const vulkan_gpu_connection&)> gpu_scorer;
		/** Used on each thread's family to determine what thread to use.
		 * This and gpu_scorer's scores are summed to determine the threads' scores; the thread with the highest score is generally picked.
		 * gpu_program_manager::new_manager_for may still pick a lower-scoring thread, for example when the highest scoring thread is already busy with work.
		 * If this returns std::nullopt, no thread in the given family will be picked; this should only be done if the family actually cannot perform the work.
		 * If empty, gpu_program_manager::new_manager_for will score the families 0.
		 */
		std::function<std::optional<float>(const vulkan_gpu_thread_family&)> family_scorer;
	};

	/** A manager for some [[vulkan_gpu_program]]s.
	 * Programs should be split into different managers when they can run asynchronous.
	 * In vulkan terms, this represents a VkCommandPool.
	 */
	class vulkan_gpu_program_manager
	{
	private:
		vulkan_gpu_connection* _gpu;
		std::size_t _family_index;
		std::size_t _thread_index;

		unique_deleter_ptr<vulkan_handle::command_pool_t> _pool;
		std::vector<gpu_program_sync> _syncs;

		destruct_event<> _destructing;

	public: // accessors
		/** Returns the gpu that the manager is on. */
		auto gpu() noexcept -> vulkan_gpu_connection& { return *_gpu; }
		/** Returns the gpu that the manager is on.
		 * @customoverload
		 */
		auto gpu() const noexcept -> const vulkan_gpu_connection& { return *_gpu; }

		/** Returns the family of threads that the manager is on. */
		auto thread_family() const noexcept -> const vulkan_gpu_thread_family&
			{ return gpu().thread_families()[_family_index]; }

		/** Returns the family of threads that the manager is on. */
		auto thread() const noexcept -> const vulkan_gpu_thread&
			{ return thread_family().threads[_thread_index]; }

		/** Returns the index of the gpu-thread-family in the gpu's families()-vector. */
		auto thread_family_index() const noexcept -> std::size_t { return _family_index; }
		/** Returns the index of the gpu-thread in the gpu-thread-family's threads-vector. */
		auto thread_index() const noexcept -> std::size_t { return _thread_index; }

		/** Returns a pointer to the manager's latest synchronization-data.
		 * Waiting on the data's fence waits for all of the manager's programs to finish.
		 * Returns nullptr if there are no synchronization data.
		 */
		auto latest_synchronization() const noexcept -> const gpu_program_sync*
			{ return _syncs.empty() ? nullptr : &_syncs.back(); }

		/** Returns whether any of the programs are still running. */
		auto working() const noexcept -> bool
		{
			return latest_synchronization() ? !latest_synchronization()->fence.completed() : false;
		}

		/** Returns an event that is invoked right before the manager is destructed. */
		auto destructing() const noexcept -> const event<>&
		{ return _destructing; }

	public: // modifiers
		/** Waits until all of the programs are done, and then returns.
		 * @customoverload
		 */
		void wait() const noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
		}
		/** Waits until all of the programs are done, and then returns. */
		void wait() noexcept
		{
			if (latest_synchronization()) latest_synchronization()->fence.wait();
			_syncs.clear();
		}

		/** Replaces the manager's latest synchronization-data;
		 * the new data should denote when all of the manager's programs are done.
		 * @return the given synchronization-data.
		 */
		auto push_synchronization(gpu_program_sync&& s) noexcept -> gpu_program_sync&
		{
			return _syncs.emplace_back(std::move(s));
		}

	public: // vulkan-related
		/** Returns the manager's vulkan_command_pool, representing a VkCommandPool. */
		auto vulkan_pool() const noexcept -> vulkan_handle::command_pool { return _pool.get(); }

		/** Returns the latest synchronization-object's vulkan_gpu_fence, representing a VkFence.
		 * Returns nullptr if there are no synchronization objects.
		 */
		auto last_vulkan_fence() const noexcept -> vulkan_handle::fence
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().fence.vulkan_fence();
		}

		/** Returns the latest synchronization-object's vulkan_gpu_semaphore, representing a VkSemaphore.
		 * Returns nullptr if there are no synchronization objects.
		 */
		auto last_vulkan_semaphore() const noexcept -> vulkan_handle::semaphore
		{
			if (_syncs.empty()) return nullptr;
			return _syncs.back().semaphore.vulkan_semaphore();
		}

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_program_manager]].
		 * Using this program is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_program_manager() = default;
		vulkan_gpu_program_manager(vulkan_gpu_program_manager&&) = default;
		auto operator=(vulkan_gpu_program_manager&&) -> vulkan_gpu_program_manager& = default;

		/** Creates a manager on the given thread;
		 * The given thread is the thread on the given gpu, whose family is at the given family-index of the gpu's families()-vector, and who is at the given thread-index of the family's threads-vector.
		 * @throws std::out_of_range if the given gpu does not have a family at the given family-index, or the family does not have a thread at the given thread-index.
		 */
		vulkan_gpu_program_manager(vulkan_gpu_connection&, std::size_t family_index, std::size_t thread_index);

		/** Finds the best thread on the given gpu to run some programs, based on the given settings, and creates a manager on it.
		 * @throws std::runtime_error if the given gpus have no threads at all to perform the given type of programs.
		 */
		static auto new_manager_for(vulkan_gpu_connection&, gpu_program_manager_settings)
			-> vulkan_gpu_program_manager;
		/** Finds the best thread on the given gpus to run some programs, based on the given settings, and creates a manager on it.
		 * @throws std::runtime_error if the given gpus have no threads at all to perform the given type of programs.
		 */
		static auto new_manager_for(vulkan_graphics_environment&, gpu_program_manager_settings)
			-> vulkan_gpu_program_manager;
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM_MANAGER
