#ifndef COMPWOLF_GPU_MAP
#define COMPWOLF_GPU_MAP

#include <utility>
#include <map>
#include <events>

namespace compwolf
{
	/** Contains a version of a gpu-dependent object for each gpu.
	 * @typeparam ElementType The type of gpu-dependent object this contains.
	 * @typeparam CopyToNewGPU When getting a version not yet created, CopyToNewGPU::func(mapPtr, new_gpu) is called to create the new version, where:
	 * * mapPtr is a pointer to this [[gpu_map]].
	 * * new_gpu is the gpu the new version should be on.
	 * @see gpu_connection
	 */
	template <typename ElementType, typename CopyToNewGPU>
	class gpu_map
	{
	public:
		/** The type of [[graphics_environment]] that the elements works with. */
		using environment_type = typename ElementType::environment_type;

		/** The type of gpu-dependent object this contains. */
		using element_type = ElementType;
		/** When getting a version not yet created, CopyToNewGPU::func(mapPtr, new_gpu) is called to create the new version, where:
		 * * mapPtr is a pointer to this [[gpu_map]].
		 * * new_gpu is the gpu the new version should be on.
		 */
		using copy_to_new_gpu = CopyToNewGPU;
	
	private:
		std::map<typename environment_type::gpu_type*, ElementType> _elements;
		event_key<> _gpu_destructing_key;

	public:
		/** Returns the version for the given gpu. */
		auto get(typename environment_type::gpu_type& gpu) noexcept -> element_type&
		{
			// get existing
			auto key = &gpu;
			auto i = _elements.find(key);
			if (i != _elements.end()) return i->second;

			// construct if not existing
			return _elements.emplace(
				key, copy_to_new_gpu::func(this, gpu)
			).first->second;
		}

		/** Returns whether this map is valid to use.
		 * A map is invalid if it was default-constructed or if its environment has been destroyed.
		 */
		auto is_valid() { return !_elements.empty(); }

	public: // constructors
		/** Constructs an invalid [[gpu_map]].
		 * Using this map is undefined behaviour.
		 * @overload
		 */
		gpu_map() noexcept = default;
		gpu_map(const gpu_map&) = default;
		auto operator=(const gpu_map&) -> gpu_map& = default;
		gpu_map(gpu_map&&) = default;
		auto operator=(gpu_map&&) -> gpu_map& = default;

		template <typename... Args>
		gpu_map(Args&&... args)
		{
			ElementType original_element(std::forward<Args>(args)...);
			auto& gpu = original_element.gpu();
			_elements.emplace(&gpu, std::move(original_element));
			_gpu_destructing_key = gpu.destructing().subscribe([this]()
				{
					_elements.clear();
				}
			);
		}
		/**
		 * @param environment This gets replaced by a gpu before the arguments are forwarded to the element's constructor.
		 * @overload
		 */
		template <typename... Args>
		gpu_map(environment_type& environment, Args&&... args)
			: gpu_map(environment.gpus()[0], args...)
		{ }
	};
}

#endif // ! COMPWOLF_GPU_MAP