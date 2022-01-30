#pragma once

#ifndef INICPP_HPP
#define INICPP_HPP

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>

namespace ini
{
	class ifile
	{
	private:
		std::string filename;
	public:
		ifile();
		ifile(const char* file_name);
		ifile(const ifile& ifi);
		~ifile();
		bool write(const char* appname, const char* keyname, const char* str);
		bool write(const char* appname, const char* keyname, const int value);
		bool write(const std::string appname, const std::string keyname, const std::string str);
		bool write(const std::string appname, const std::string keyname, const int value);

		const char* getCStr(const char* appname, const char* keyname);
		const char* getCStr(const std::string appname, const std::string keyname);
		long double getInt(const char* appname, const char* keyname);
		long double getInt(const std::string appname, const std::string keyname);
		long double getDouble(const std::string appname, const std::string keyname);
		void getSection(std::vector<std::string>& sec);
		std::string getString(const std::string appname, const std::string keyname);

		bool deleteSection(const char* name);
		bool deleteSection(const std::string name);
		bool deleteKey(const char* section, const char* keyname);
		bool deleteKey(const std::string section, const std::string keyname);

		ifile& operator=(const ifile& i);
	};
}

#endif // !INICPP_HPP
