
#include "tinnet/includes/memory/ScopedStorage.h"

#include "catch2.hpp"

#include <cstddef>

TEST_CASE("tinnet::memory::ScopedStorage")
{
	SECTION("Alignment")
	{
		tinnet::memory::ScopedStorage sStorage{sizeof(float)};

		CHECK((reinterpret_cast<std::size_t>(sStorage.aligned()) % 32 == 0) == true);
	}
}