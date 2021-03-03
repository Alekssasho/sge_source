#include "CoreLog.h"
#include "sge_utils/utils/strings.h"
#include <varargs.h>

namespace sge {

void CoreLog::write(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	m_messages.emplace_back(Message(messageType_log, buffer));
}

void CoreLog::writeCheck(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	m_messages.emplace_back(Message(messageType_check, buffer));
}

void CoreLog::writeError(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	m_messages.emplace_back(Message(messageType_error, buffer));
}

void CoreLog::writeWarning(const char* format, ...) {
	va_list args;
	va_start(args, format);
	std::string buffer;
	string_format(buffer, format, args);
	va_end(args);

	m_messages.emplace_back(Message(messageType_warning, buffer));
}
} // namespace sge
