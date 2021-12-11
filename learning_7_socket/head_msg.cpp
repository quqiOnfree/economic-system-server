#define _CRT_SECURE_NO_WARNINGS

#include "head_msg.hpp"
#include <cstring>

void ch_head(char* head, const char* heads)//消息头赋值
{
	strcpy(head, heads);
}

make_head::make_head()//初始化为空
{
	str = "";
	len = str.size();
	head = new char[8]{ 0 };
}

make_head::make_head(const char* str_)//初始化
{
	head = new char[8]{ 0 };
	str = str_;
	len = str.size();
	make_long ml {len};
	char heads[8]{ 0 };
	memcpy(&heads, &ml, sizeof(ml));
	ch_head(head, heads);
}

//make_head::make_head(const make_head& mk)//复制构造函数（没测试过，慎用）
//{
//	str = mk.str;
//	len = str.size();
//	make_long ml{ len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(head, heads);
//}

make_head::~make_head()//构析
{
	delete[] head;
}

//make_head& make_head::operator=(const char* str_)//赋值新class（没测试过，慎用）
//{
//	str = str_;
//	len = str.size();
//	make_long ml{ len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(head, heads);
//	return *this;
//}

make_head& make_head::operator=(const make_head& mk)//赋值新class
{
	if (this == &mk)
	{
		return *this;
	}
	str = mk.str;
	strcpy(head, mk.head);
	len = mk.len;
	return *this;
}

//make_head& make_head::operator+(const make_head& mk) const//加法（没啥用）
//{
//	make_head newm;
//	newm.str = str + mk.str;
//	newm.len = str.size();
//	make_long ml{ newm.len };
//	char heads[8]{ 0 };
//	memcpy(&heads, &ml, sizeof(ml));
//	ch_head(newm.head, heads);
//	return newm;
//}

//std::ostream& operator<<(std::ostream& os, const make_head& mk)//打印（没啥用）
//{
//	os << mk.head << mk.str;
//	return os;
//}

unsigned long long make_head::get_len() const//获取带消息头后的长度，如果要消息头，可以在后面 减去8
{
	return (len + 8LL);
}

char* make_head::get_char() const//获取不带消息头的数据
{
	char* str_ = new char[str.size() + 1]{ 0 };
	strcpy(str_, str.c_str());
	return str_;
}

char* make_head::get_head() const//获取消息头
{
	return head;
}

char* make_head::get_head_str() const//获取带有消息头的数据
{
	char head_[9]{ 0 };
	char st;
	strcpy(head_, head);

	char* strs = new char[make_head::get_len()+1]{ 0 };
	strcpy(strs, head_);
	size_t length = len;
	for (size_t i = 0; i < length; i++)
	{
		st = str[i];
		strs[i+8] = st;
	}
	return strs;
}

unpack_head::unpack_head()//初始化为空
{
	str = new char[1]{ 0 };
	ml.len = 0;
	len = 0;
}

unpack_head::unpack_head(const char* str_,size_t size)//设置初始化数据
{
	if (!size || !strlen(str_))//如果数据为空
	{
		*this = unpack_head();
		return;
	}
	len = size - 8;
	size_t length = 8;
	char strs[1024]{ 0 };

	for (size_t i = 0; i < length; i++)
	{
		head[i] = str_[i];
	}
	memcpy(&ml, &head, sizeof(head));

	for (size_t i = 8; i < size; i++)
	{
		strs[i - 8] = str_[i];
	}

	str = new char[size - 8]{ 0 };
	length = size - 8;
	for (size_t i = 0; i < length; i++)
	{
		str[i] = strs[i];
	}
}

unpack_head::~unpack_head()//构析函数
{
	delete[] str;
}

void unpack_head::add_str(const char* str_,size_t size)//添加新数据
{
	char* str_n = new char[size + len]{ 0 };

	for (size_t i = 0; i < len; i++)
	{
		str_n[i] = str[i];
	}
	for (size_t i = 0; i < size; i++)
	{
		str_n[len + i] = str_[i];
	}

	len = size + len;

	delete[] str;
	str = str_n;
}

bool unpack_head::can_get() const//判断是否能获取数据
{
	if (len>=ml.len)
	{
		return true;
	}
	return false;
}

char* unpack_head::get_char()//获取消息头之后的数据
{
	if (unpack_head::can_get())
	{
		char* str_ = new char[ml.len + 1]{ 0 };
		for (size_t i = 0; i < ml.len; i++)
		{
			str_[i] = str[i];
		}

		if (len - ml.len)
		{
			char* str_n = new char[len - ml.len]{ 0 };
			for (size_t i = 0; i < len - ml.len; i++)
			{
				str_n[i] = str[i + ml.len];
			}
			delete[] str;
			str = str_n;
		}

		len = len - ml.len;

		if (len > 8)//如果获取完数据len不为8，获取新的消息头
		{
			*this = unpack_head(str, len);
		}

		return str_;
	}
	char* str_ = new char[1]{ 0 };
	return str_;
}

unpack_head& unpack_head::operator=(const unpack_head& uh) //赋值class
{
	if (&uh==this)
	{
		return *this;
	}

	strcpy(head, uh.head);
	delete[] str;
	str = new char[uh.len]{ 0 };
	for (size_t i = 0; i < uh.len; i++)
	{
		str[i] = uh.str[i];
	}
	ml.len = uh.ml.len;
	len = uh.len;
	return *this;
}
