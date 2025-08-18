#pragma once
#include <string>
#include <memory>

#ifdef ESP_PLATFORM
#include <esp_heap_caps.h>
#endif

template <typename T>
class PSRAMAllocator {
	public:
		using value_type = T;

		PSRAMAllocator() = default;

		template <class U>
		explicit PSRAMAllocator(const PSRAMAllocator<U>&) {}

		T* allocate(std::size_t n) {
#ifdef ESP_PLATFORM
			return static_cast<T*>(heap_caps_malloc(n * sizeof(T), MALLOC_CAP_SPIRAM));
#else
			return static_cast<T*>(::malloc(n * sizeof(T)));
#endif
		}

		void deallocate(T* p, std::size_t) {
#ifdef ESP_PLATFORM
			heap_caps_free(p);
#else
			::free(p);
#endif
		}
};

template <typename T, typename U>
bool operator==(const PSRAMAllocator<T>&, const PSRAMAllocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const PSRAMAllocator<T>&, const PSRAMAllocator<U>&) { return false; }

using PSRAMString = std::basic_string<char, std::char_traits<char>, PSRAMAllocator<char>>;