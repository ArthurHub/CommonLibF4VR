#pragma once

namespace RE::msvc
{
	class type_info;

	template <class>
	class function;

	/**
	 * The std::function of the MSVC that built Fallout 4 OG and Fallout 4 VR (0x20 bytes: 0x18 of inline storage, then the
	 * callable's proxy). The game hands these out and takes them by reference, as BSTThreadScrapFunction, so this mirrors
	 * its layout and ownership: it can't wrap a C++ callable, only one the game built (e.g. BSCore::CreateThreadScrapFunction),
	 * and it destroys that callable through the game's own proxy, which frees it from the heap the game allocated it on.
	 */
	// class std::_Func_class
	template <class R, class... Args>
	class function<R(Args...)>
	{
	public:
		using result_type = R;

		function() noexcept = default;
		function(const function&) = delete;
		function& operator=(const function&) = delete;

		function(function&& a_rhs) noexcept
		{
			steal(a_rhs);
		}

		function& operator=(function&& a_rhs) noexcept
		{
			if (this != std::addressof(a_rhs)) {
				reset();
				steal(a_rhs);
			}
			return *this;
		}

		~function() noexcept
		{
			reset();
		}

		[[nodiscard]] explicit operator bool() const noexcept { return good(); }

		result_type operator()(Args&&... a_args) const
		{
			assert(good());
			return _fn->do_call(std::forward<Args>(a_args)...);
		}

		/**
		 * Destroy the held callable, as std::_Func_class::_Tidy does: a callable in the inline storage is only destroyed,
		 * one on the heap is also freed.
		 */
		void reset() noexcept
		{
			if (_fn) {
				_fn->delete_this(!is_local());
				_fn = nullptr;
			}
		}

	private:
		// class std::_Func_base
		class __declspec(novtable) proxy_t
		{
		public:
			// add
			virtual proxy_t*         copy(void*) = 0;          // 00
			virtual proxy_t*         move(void*) = 0;          // 01'r
			virtual result_type      do_call(Args&&...) = 0;   // 02
			virtual const type_info& target_type() const = 0;  // 03
			virtual void             delete_this(bool) = 0;    // 04
			virtual ~proxy_t() = default;                      // 05
			virtual const void* get() const = 0;               // 06
		};

		[[nodiscard]] bool good() const noexcept { return _fn != nullptr; }

		[[nodiscard]] bool is_local() const noexcept { return static_cast<const void*>(_fn) == static_cast<const void*>(_storage); }

		/**
		 * Take the callable of a_rhs, leaving it empty. A callable held on the heap changes owner; one in the inline storage
		 * is moved into ours by its proxy, as std::_Func_class::_Reset_move does.
		 */
		void steal(function& a_rhs) noexcept
		{
			if (!a_rhs._fn) {
				return;
			}
			if (a_rhs.is_local()) {
				_fn = a_rhs._fn->move(_storage);
				a_rhs.reset();
			} else {
				_fn = std::exchange(a_rhs._fn, nullptr);
			}
		}

		// std::aligned_storage[_t] is deprecated in C++23; use the recommended alignas byte buffer instead.
		alignas(long double) std::byte _storage[3 * sizeof(void*)]{};  // 00
		proxy_t* _fn{ nullptr };                                       // 18
	};
	static_assert(sizeof(function<void()>) == 0x20);
}
