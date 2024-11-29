#ifndef COMPWOLF_EVENT
#define COMPWOLF_EVENT

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>

namespace compwolf
{
	template <typename ParameterType = void>
	class event;


	template <typename ParameterType = void>
	class event_key
	{
	public:
		/** The parameter type of the event's subscribers. */
		using parameter_type = ParameterType;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void(parameter_type)>;
		/** The type of object used to internally represent a subscriber. */
		using event_type = event<ParameterType>;
		/** The type of object used to internally represent a subscriber. */
		using internal_key_type = std::vector<value_type>::size_type;
	private:
		const event_type* _event = nullptr;
		internal_key_type _internal_key;

	public: // accessors
		constexpr auto internal_key() const noexcept -> internal_key_type { return _internal_key; }
		constexpr auto target_event() const noexcept -> const event_type& { return *_event; }

	public: // constructors
		constexpr event_key() noexcept = default;
		constexpr event_key(event_key&& other) noexcept
		{
			_event = other._event;
			_internal_key = other._internal_key;

			other._event = nullptr;
		}
		constexpr ~event_key() noexcept;

		auto operator==(event_key&& other) noexcept -> event_key&
		{
			this->~event_key();
			return *new(this)event_key(std::move(other));
		}

		/** Should only be constructed by [[event]]. */
		constexpr event_key(const event_type& e, internal_key_type k) noexcept
			: _event(&e), _internal_key(k)
		{}
	};
	/** @hidden */
	template <>
	class event_key<void>
	{
	public:
		/** The parameter type of the event's subscribers. */
		using parameter_type = void;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void()>;
		/** The type of object used to internally represent a subscriber. */
		using event_type = event<void>;
		/** The type of object used to internally represent a subscriber. */
		using internal_key_type = std::vector<value_type>::size_type;
	private:
		const event_type* _event = nullptr;
		internal_key_type _internal_key;

	public: // accessors
		constexpr auto internal_key() const noexcept -> internal_key_type { return _internal_key; }
		constexpr auto target_event() const noexcept -> const event_type& { return *_event; }

	public: // constructors
		constexpr event_key() noexcept = default;
		constexpr event_key(event_key&& other) noexcept
		{
			_event = other._event;
			_internal_key = other._internal_key;

			other._event = nullptr;
		}
		constexpr ~event_key() noexcept;

		auto operator==(event_key&& other) noexcept -> event_key&
		{
			this->~event_key();
			return *new(this)event_key(std::move(other));
		}

		/** Should only be constructed by [[event]]. */
		constexpr event_key(const event_type& e, internal_key_type k) noexcept
			: _event(&e), _internal_key(k)
		{}
	};

	/** An implementation of the observer pattern, with functors as the observers and this as the subject.
	 * Specifically, allows functions and functors to "subscribe" to this; this can then be invoked to invoke all subscribers.
	 * @typeparam ParameterType The type of object to pass to subscribers; subscribers must have this as their only parameter.
	 * If void, passes nothing to subscribers; subscribers must then have no parameters.
	 * Consider making this a reference type to minimize data copying.
	 */
	template <typename ParameterType>
	class event
	{
	public:
		/** The parameter type of the event's subscribers. */
		using parameter_type = ParameterType;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void(parameter_type)>;
		/** The type of object used to represent a subscriber. */
		using key_type = event_key<ParameterType>;

	private:
		mutable std::vector<value_type> _observers;

	public: // modifiers
		/** Subscribes the given functor to the event.
		 * @return a key to identify the functor, if you later wishes to unsubscribe it from the event.
		 */
		key_type subscribe(value_type observer) const noexcept
		{
			auto key = _observers.size();
			_observers.emplace_back(std::move(observer));
			return key_type(*this, key);
		}
		/** Unsubscribes the functor represented by the given key from the event.
		 * The key was generated when the functor subscribed to the event.
		 */
		void unsubscribe(key_type&& observer_key) const noexcept
		{
			_observers[observer_key.internal_key()] = value_type();
		}

	public: // operators
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void invoke(parameter_type parameter)
		{
			for (auto& observer : std::vector<value_type>(_observers)) // copy vector so it cannot be modified during loop
			{
				if (observer == nullptr) continue;
				observer(static_cast<std::add_rvalue_reference_t<parameter_type>>(parameter)); // turn non-reference to xvalue
			}
		}
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void operator()(parameter_type parameter)
		{
			invoke(static_cast<std::add_rvalue_reference_t<parameter_type>>(parameter)); // turn non-reference to xvalue
		}
	};

	/** An implementation of the observer pattern, with functors as the observers and this as the subject.
	 * Specifically, allows functions and functors to "subscribe" to this; this can then be invoked to invoke all subscribers.
	 * @hidden
	 */
	template <>
	class event<void>
	{
	public:
		/** Void, to signal that this event passes no data long to its subscribers when invoked. */
		using parameter_type = void;
		/** The type of functor that can subscribe to the event. */
		using value_type = std::function<void(parameter_type)>;
		/** The type of object used to represent a subscriber. */
		using key_type = event_key<void>;

	private:
		mutable std::vector<value_type> _observers;

	public: // modifiers
		/** Subscribes the given functor to the event.
		 * @return a key to identify the functor, if you later wishes to unsubscribe it from the event.
		 */
		key_type subscribe(value_type observer) const noexcept
		{
			auto key = _observers.size();
			_observers.emplace_back(std::move(observer));
			return key_type(*this, key);
		}
		/** Unsubscribes the functor represented by the given key from the event.
		 * The key was generated when the functor subscribed to the event.
		 */
		void unsubscribe(key_type&& observer_key) const noexcept
		{
			_observers[observer_key.internal_key()] = value_type();
		}

	public: // operators
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void invoke()
		{
			for (auto& observer : std::vector<value_type>(_observers)) // copy vector so it cannot be modified during loop
			{
				if (observer == nullptr) continue;
				observer();
			}
		}
		/** Invokes all subscribed functions/functors, passing along the given value to them. */
		void operator()()
		{
			invoke();
		}
	};

	/** @hidden */
	template <typename ParameterType>
	constexpr event_key<ParameterType>::~event_key() noexcept
	{
		if (_event) _event->unsubscribe(std::move(*this));
	}
	/** @hidden */
	constexpr event_key<void>::~event_key() noexcept
	{
		if (_event) _event->unsubscribe(std::move(*this));
	}
}

#endif // ! COMPWOLF_EVENT
