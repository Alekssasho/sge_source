#include "sge_utils/sge_utils.h"
#include "sge_utils/utils/strings.h"

#include <cstring>
#include <stdio.h>

#include "Logger.h"

namespace sge {

//-------------------------------------------------------------------
// Loger
//-------------------------------------------------------------------
Logger::Logger() {
}

void Logger::close() {
}

Logger* Logger::getDefaultLog() {
	static Logger defaultLogger;
	return &defaultLogger;
}

void Logger::setLogOutputFile([[maybe_unused]] const char* filename) {
}

void Logger::write(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	printf(buffer.data());
}

void Logger::writeError(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	printf(buffer.data());
}

void Logger::writeWarning(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	printf(buffer.data());
}

} // namespace sge
