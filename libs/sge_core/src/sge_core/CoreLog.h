#pragma once

#include "sgecore_api.h"
#include <string>
#include <vector>

#include "sge_utils/utils/basetypes.h"

namespace sge {

struct SGE_CORE_API CoreLog : public Noncopyable {
	enum MessageType : int {
		/// Just a message that something has been done.
		messageType_log,
		/// A message that something important or anticipated has been done. Usually a normal message that we want to outline ot the user.
		messageType_check,
		/// A warrning that something might be worng or the user needs to pay attention.
		messageType_warning,
		/// An message that singnal to the user that something failed or couldn't be done.
		messageType_error,
	};

	struct Message {
		Message(MessageType type, std::string msg)
		    : type(type)
		    , message(std::move(msg)) {}

		MessageType type;
		std::string message;
	};

	void write(const char* format, ...);
	void writeCheck(const char* format, ...);
	void writeError(const char* format, ...);
	void writeWarning(const char* format, ...);

	const std::vector<Message>& getMessages() const { return m_messages; }

  private:
	std::string m_logFilename;
	std::vector<Message> m_messages;
};


} // namespace sge
