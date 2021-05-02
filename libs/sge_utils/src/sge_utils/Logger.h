#pragma once

namespace sge {

class Logger {
  private:
  public:
	Logger();
	~Logger() { close(); }

	void setLogOutputFile(const char* filename);
	void write(const char* format, ...);
	void writeError(const char* format, ...);
	void writeWarning(const char* format, ...);
	void close();

  public:
	static Logger* getDefaultLog();
};

} // namespace sge
