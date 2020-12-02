#pragma once

#include <cstdint>

enum class e_error_code {
	INTERFACES = 1,
	PATTERNS,
	HOOKS,
	NETVARS,
	RENDER,
};

namespace utils {

	void error_and_exit(e_error_code error);
	void error_and_exit(e_error_code error, uint32_t info1);
	void error_and_exit(e_error_code error, uint32_t info1, uint32_t info2);
	void error_and_exit(e_error_code error, uint32_t info1, uint32_t info2, uint32_t info3);

	// you need more? ¯\_(ツ)_/¯

}
