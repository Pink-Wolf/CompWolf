#ifndef COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM
#define COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM

#include <vulkan_graphics_environments>
#include <gpu_programs>
#include "vulkan_gpu_program_manager.hpp"
#include "vulkan_gpu_fence.hpp"
#include <unique_deleter_ptr>

namespace compwolf::vulkan
{
	/** Aggregate type containing data passed by [[vulkan_gpu_program]] to its code. */
	struct vulkan_code_parameters
	{
		/** The program's [[vulkan_handle::command]], representing a VkCommandBuffer. */
		vulkan_handle::command command;
	};

	/** A "program", which allows the cpu to know when the gpu has finished some work. */
	class vulkan_gpu_program : public gpu_specific_program<vulkan_graphics_environment, vulkan_gpu_fence>
	{
	private:
		vulkan_gpu_program_manager* _manager;
		unique_deleter_ptr<vulkan_handle::command_t> _vulkan_command;
		event_key<> _manager_destructing_key;

	public: // accessors
		/** Returns the manager that the program is on. */
		auto manager() noexcept -> vulkan_gpu_program_manager& { return *_manager; }
		/** Returns the manager that the program is on. */
		auto manager() const noexcept -> const vulkan_gpu_program_manager& { return *_manager; }

		/** Returns whether this is valid, that is one not constructed by the default constructor. */
		operator bool() const noexcept
		{
			return !!_vulkan_command;
		}

	public: // modifiers
		/** Runs the program.
		 * @return a fence denoting when the program is finished running.
		 * @throws std::runtime_error if there was an error submitting the program to the gpu due to causes outside of the program.
		 */
		auto execute() -> const vulkan_gpu_fence& final;

	public: // vulkan-related
		/** Returns the [[vulkan_handle::command]], representing a Vkprogram. */
		auto vulkan_program() const noexcept -> vulkan_handle::command { return _vulkan_command.get(); }

	public: // constructors
		/** Constructs an invalid [[vulkan_gpu_program]].
		 * Using this program is undefined behaviour.
		 * @overload
		 */
		vulkan_gpu_program() = default;
		vulkan_gpu_program(vulkan_gpu_program&&) = default;
		auto operator=(vulkan_gpu_program&&) -> vulkan_gpu_program& = default;

		/** Creates a program for the given gpu.
		 * @throws std::runtime_error if there was an error during creation of the program due to causes outside of the program.
		 */
		vulkan_gpu_program(vulkan_gpu_program_manager&
			, std::function<void(const vulkan_code_parameters&)> code);
	};
}

#endif // ! COMPWOLF_GRAPHICS_VULKAN_GPU_PROGRAM
