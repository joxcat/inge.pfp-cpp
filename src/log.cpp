#include <string>
#include <cstdarg>

#include "MicroBit.h"
#include "pfp/log.hpp"

Logger::Logger(std::function<void(char *)> on_log) {
	this->on_log = on_log;
	this->buffer = new std::array<char, PFP_LOG_BUFFER_SIZE>;
	this->span = nullptr;
}
Logger::Logger(Logger const& logger) {
	this->on_log = logger.on_log;
	this->buffer = logger.buffer;
	this->span = logger.span;
	this->is_clone = true;
}
Logger::~Logger() {
	if (!this->is_clone) {
		delete this->buffer;
	}
	if (this->span) delete this->span;
}
Logger Logger::clone() {
	return Logger(*this);
}

void Logger::log(char *msg, const char *level) {
	std::array<char, PFP_LOG_BUFFER_SIZE> msg_clone;
	sprintf(msg_clone.data(), "%s", msg);

	if (this->span && this->span != nullptr) {
		sprintf(this->buffer->data(), "[%s - %s]: %s\r\n", level, span->name, msg_clone.data());
	} else {
		sprintf(this->buffer->data(), "[%s]: %s\r\n", level, msg_clone.data());
	}

	// XXX: Does not work with clone
	this->on_log(this->buffer->data());
}

void Logger::debug(std::string const& msg) { this->debug((char *)msg.c_str()); }
void Logger::debug(const unsigned char *msg) { this->debug((char *)msg); }
void Logger::debug(const char *msg, ...) {
	#if LOG_LEVEL <= LOG_LEVEL_DEBUG
		va_list args;
		this->buffer->fill('\0');
		va_start(args, msg);
		vsprintf(this->buffer->data(), msg, args);
		va_end(args);
		this->log(this->buffer->data(), "DEBUG");
	#endif
}
void Logger::debug(char *msg) {
	#if LOG_LEVEL <= LOG_LEVEL_DEBUG
		this->buffer->fill('\0');
		this->log(msg, "DEBUG");
	#endif
}

void Logger::info(std::string const& msg) { this->info((char *)msg.c_str()); }
void Logger::info(const unsigned char *msg) { this->info((char *)msg); }
void Logger::info(const char *msg, ...) {
	#if LOG_LEVEL <= LOG_LEVEL_INFO
		va_list args;
		this->buffer->fill('\0');
		va_start(args, msg);
		vsprintf(this->buffer->data(), msg, args);
		va_end(args);
		this->log(this->buffer->data(), "INFO");
	#endif
}
void Logger::info(char *msg) {
	#if LOG_LEVEL <= LOG_LEVEL_INFO
		this->buffer->fill('\0');
		this->log(msg, "INFO");
	#endif
}

void Logger::warn(std::string const& msg) { this->warn((char *)msg.c_str()); }
void Logger::warn(const unsigned char *msg) { this->warn((char *)msg); }
void Logger::warn(const char *msg, ...) {
	#if LOG_LEVEL <= LOG_LEVEL_WARNING
		va_list args;
		this->buffer->fill('\0');
		va_start(args, msg);
		vsprintf(this->buffer->data(), msg, args);
		va_end(args);
		this->log(this->buffer->data(), "WARN");
	#endif
}
void Logger::warn(char *msg) {
	#if LOG_LEVEL <= LOG_LEVEL_WARNING
		this->buffer->fill('\0');
		this->log(msg, "WARN");
	#endif
}

void Logger::error(std::string const& msg) { this->error((char *)msg.c_str()); }
void Logger::error(const unsigned char *msg) { this->error((char *)msg); }
void Logger::error(const char *msg, ...) {
		this->buffer->fill('\0');
	#if LOG_LEVEL <= LOG_LEVEL_ERROR
		va_list args;
		this->buffer->fill('\0');
		va_start(args, msg);
		vsprintf(this->buffer->data(), msg, args);
		va_end(args);
		this->log(this->buffer->data(), "ERROR");
	#endif
}
void Logger::error(char *msg) {
	#if LOG_LEVEL <= LOG_LEVEL_ERROR
		this->buffer->fill('\0');
		this->log(msg, "ERROR");
	#endif
}

ManagedString bthex(uint8_t *buffer, uint8_t len) {
	char hex[len*5] = {'\0'};
	for (int i = 0; i < len; i++) {
		sprintf(hex, "%s 0x%02X", hex, buffer[i]);
	}
	ManagedString result(hex);
	return result;
}

ManagedString bttext(uint8_t *buffer, uint8_t len) {
	char text[len+1];
	for (int i = 0; i < len; i++) {
		text[i] = buffer[i];
	}
	text[len] = '\0';
	return ManagedString(text);
}

// Span
Logger Logger::enter_span(const char * name) {
	Logger span_logger = this->clone();
	Span * span = new Span(name);
	span_logger.span = span;
	return span_logger;
}

Span::Span(const char * name) {
	this->name = name;
}
Span::~Span() {}