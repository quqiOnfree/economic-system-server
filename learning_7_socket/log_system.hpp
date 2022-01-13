#pragma once
#ifndef LOG_SYSTEM
#define LOG_SYSTEM

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

inline static const char* q_get_time()
{
	std::time_t tt = std::time(0);
	std::tm* time_ = std::localtime((const std::time_t*)&tt);
	std::string msgs("");
	msgs += '[' +
		(time_->tm_mday >= 10 ? std::to_string(time_->tm_mday) : std::string("0") + std::to_string(time_->tm_mday)) + " "
		+ (time_->tm_hour >= 10 ? std::to_string(time_->tm_hour) : std::string("0") + std::to_string(time_->tm_hour))
		+ ':' +
		(time_->tm_min >= 10 ? std::to_string(time_->tm_min) : std::string("0") + std::to_string(time_->tm_min)) + ':'
		+ (time_->tm_sec >= 10 ? std::to_string(time_->tm_sec) : std::string("0") + std::to_string(time_->tm_sec))
		+ "]";
	char* n_str = new char[msgs.size() + 1]{ 0 };
	strcpy(n_str, msgs.c_str());
	return n_str;
}

class Log
{
private:
	std::string name;
	FILE* file;
	bool have_write_time;
	bool have_open;
	template<typename... Args> bool write_2(const char* msg_, const Args... args);
	template<> bool write_2(const char* msg_)
	{
		if (!file)
		{
			return false;
		}
		if (!have_write_time)
		{
			const char* t = q_get_time();
			fputs(t, file);
			printf("%s", t);
			delete t;
			have_write_time = true;
		}

		puts(msg_);
		fputs(msg_, file);
		fputs("\n", file);
		have_write_time = false;
		return true;
	}
public:
	Log();
	Log(const char* file_name);
	~Log();
	template<typename... Args> bool write(const char* msg_, const Args... args);
	template<> bool write(const char* msg_)
	{
		if (!have_open)
		{
			file = fopen(name.c_str(), "a");
			have_open = true;
		}
		if (!file)
		{
			return false;
		}
		if (!have_write_time)
		{
			const char* t = q_get_time();
			fputs(t, file);
			printf("%s", t);
			delete t;
			have_write_time = true;
		}
		puts(msg_);
		fputs(msg_, file);
		fputs("\n", file);

		have_write_time = false;
		return true;
	}
	Log& operator=(const Log& l);
};

template<typename... Args>
bool Log::write_2(const char* msg_, const Args... args)
{
	if (!file)
	{
		return false;
	}

	printf("%s", msg_);
	fputs(msg_, file);

	return write_2(args...);
}

template<typename... Args>
bool Log::write(const char* msg_, const Args... args)
{
	if (!have_open)
	{
		file = fopen(name.c_str(), "a");
	}
	if (!file)
	{
		return false;
	}
	const char* t = q_get_time();
	fputs(t, file);
	printf("%s", t);
	delete t;

	printf("%s", msg_);
	fputs(msg_, file);

	have_write_time = true;
	return Log::write_2(args...);
}


#endif // !LOG_SYSTEM
