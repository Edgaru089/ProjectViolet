#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <functional>
#include <vector>
#include <mutex>
#include "StringParser.hpp"
using namespace std;

#define AUTOLOCK(a) lock_guard<mutex> __mutex_lock(a)

class Log {
public:

	const string logLevelName[5] = { "DEBUG", " INFO", " WARN", "ERROR", "FATAL ERROR" };

	enum LogLevel {
		Debug,
		Info,
		Warning,
		Error,
		FatalError
	};

	Log() :ignoreLevel(-1) {}
	Log(ostream& output) :out({ &output }), ignoreLevel(-1) {}
	Log(ostream* output) :out({ output }), ignoreLevel(-1) {}

	void log(string content, LogLevel level = Info) {
		if (level <= ignoreLevel) return;
		time_t curtime = time(NULL);
		char buffer[64];
		strftime(buffer, 63, "[%T", localtime(&curtime));
		string final = string(buffer) + " " + logLevelName[level] + "]: " + content;
		lock.lock();
		buffers.push_back(final);
		for (ostream* i : out) {
			(*i) << final << '\n';
			i->flush();
		}
		for (const auto& i : outf)
			i(final);
		lock.unlock();
	}

	template<typename... Args>
	void logf(LogLevel level, string format, Args... args) {
		char buf[2560];
		sprintf(buf, format.c_str(), args...);
		log(string(buf), level);
	}

	void operator() (string content, LogLevel level = Info) {
		log(content, level);
	}

	void addOutputStream(ostream& output) { out.push_back(&output); }
	void addOutputStream(ostream* output) { out.push_back(output); }
	void addOutputHandler(function<void(const string&)> output) { outf.push_back(output); }

	// Lower and equal; use -1 to ignore nothing
	void ignore(int level) { ignoreLevel = level; }
	int getIgnoreLevel() { return ignoreLevel; }

	const vector<string>& getBuffers() { AUTOLOCK(lock); return buffers; }
	void clearBuffer() { AUTOLOCK(lock); buffers.clear(); }

private:
	vector<ostream*> out;
	vector<function<void(const string&)>> outf;
	mutex lock;
	int ignoreLevel;

	vector<string> buffers;
};

Log dlog;

class LogMessage {
public:

	LogMessage() :level(Log::Info) {}
	LogMessage(Log::LogLevel level) :level(level) {}

	LogMessage& operator <<(bool                data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(char                data) { buffer += (data); return *this; }
	LogMessage& operator <<(unsigned char       data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(short               data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(unsigned short      data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(int                 data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(unsigned int        data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(long long           data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(unsigned long long  data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(float               data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(double              data) { buffer += StringParser::toString(data); return *this; }
	LogMessage& operator <<(const char*         data) { buffer += string(data); return *this; }
	LogMessage& operator <<(const std::string&  data) { buffer += data; return *this; }

	LogMessage& operator <<(Log::LogLevel      level) { this->level = level;  return *this; }
	LogMessage& operator <<(Log&                 log) { flush(log);  return *this; }

public:

	void setLevel(Log::LogLevel level) { this->level = level; }
	void flush(Log& log) { logout(log); clear(); }
	void logout(Log& log) { log(buffer, level); }
	void clear() { buffer = ""; }

private:
	string buffer;
	Log::LogLevel level;
};

#define mlog LogMessage()
#define mlogd LogMessage(Log::Debug)
