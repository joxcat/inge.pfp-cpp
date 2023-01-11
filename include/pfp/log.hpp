#ifndef __PFP_LOG_HPP__
#define __PFP_LOG_HPP__

#include <string>
#include <functional>

#include "MicroBit.h"

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_ERROR 3

#define LOG_LEVEL LOG_LEVEL_DEBUG

#define PFP_LOG_BUFFER_SIZE 128
#define PFP_LOG_SPAN_NAME_SIZE 16

class Logger;

class Span {
	private:
		Logger *logger;

	public:
		const char *name;
		
		Span(const char * name);
		~Span();
};

class Logger {
	private:
		std::function<void(char *)> on_log;
		Span * span;
		void log(char *msg, const char *level);
		bool is_clone = false;

	public:
		std::array<char, PFP_LOG_BUFFER_SIZE> * buffer;
		
		Logger(std::function<void(char *)> on_log);
		Logger(Logger const& logger);
		~Logger();
		Logger clone();

		Logger enter_span(const char * name);
		
		void debug(std::string const& msg);
		void debug(const unsigned char *msg);
		void debug(const char *msg, ...);
		void debug(char *msg);
		
		void info(std::string const& msg);
		void info(const unsigned char *msg);
		void info(const char *msg, ...);
		void info(char *msg);

		void warn(std::string const& msg);
		void warn(const unsigned char *msg);
		void warn(const char *msg, ...);
		void warn(char *msg);

		void error(std::string const& msg);
		void error(const unsigned char *msg);
		void error(const char *msg, ...);
		void error(char *msg);
};

ManagedString bthex(uint8_t *buffer, uint8_t len);
ManagedString bttext(uint8_t *buffer, uint8_t len);

#endif // __PFP_LOG_HPP__