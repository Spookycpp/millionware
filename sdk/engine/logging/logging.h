#pragma once

#include <fmt/format.h>

// why tf is it even defined ???
#ifdef SEVERITY_ERROR
#undef SEVERITY_ERROR
#endif

enum e_log_severity
{
	SEVERITY_TRACE,
	SEVERITY_DEBUG,
	SEVERITY_INFO,
	SEVERITY_WARNING,
	SEVERITY_ERROR,
};

namespace logging
{
	void init(int severity);

	bool should_log(int severity);

	void print(int severity, const std::string &message);

	template <typename ...A>
	void print(int severity, const std::string &format_str, A&& ...args)
	{
		if (!should_log(severity))
			return;

		print(severity, std::format(format_str, args...));
	}

	template <typename ...A>
	void trace(const std::string &format_str, A&& ...args)
	{
		print(SEVERITY_TRACE, format_str, args...);
	}

	template <typename ...A>
	void debug(const std::string &format_str, A&& ...args)
	{
		print(SEVERITY_DEBUG, format_str, args...);
	}

	template <typename ...A>
	void info(const std::string &format_str, A&& ...args)
	{
		print(SEVERITY_INFO, format_str, args...);
	}

	template <typename ...A>
	void warning(const std::string &format_str, A&& ...args)
	{
		print(SEVERITY_WARNING, format_str, args...);
	}

	template <typename ...A>
	void error(const std::string &format_str, A&& ...args)
	{
		print(SEVERITY_ERROR, format_str, args...);
	}
}
