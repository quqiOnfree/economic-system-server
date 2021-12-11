#pragma once
#ifndef HEAD_MSG_HPP
#define HEAD_MSG_HPP

#include <iostream>
#include <string>

struct make_long
{
	size_t len;
};

class make_head
{
private:
	std::string str;
	char* head;
	unsigned long long len;
public:
	make_head();
	make_head(const char* str_);
	//make_head(const make_head& mk);
	~make_head();
	//make_head& operator=(const char* str_);
	make_head& operator=(const make_head& mk);
	//make_head& operator+(const make_head& mk) const;
	//friend std::ostream& operator<<(std::ostream& os, const make_head& mk);
	unsigned long long get_len() const;
	char* get_char() const;
	char* get_head() const;
	char* get_head_str() const;
};

class unpack_head
{
public:
	unpack_head();
	unpack_head(const char* str_, size_t size);
	~unpack_head();
	void add_str(const char* str_, size_t size);
	bool can_get() const;
	char* get_char();
	unpack_head& operator=(const unpack_head& uh);

private:
	char head[8]{ 0 };
	char* str;
	make_long ml;
	size_t len;
};

#endif
