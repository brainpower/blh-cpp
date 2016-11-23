/*
 * Copyright (c) 2013-2016 brainpower <brainpower at mailbox dot org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include "string_helpers.hpp"

#include <cstdarg>
#include <sstream>
#include <cstring>

namespace blh {

 // a simple format-string, for type-safety and more, use boost::format
std::string stringf(const char* format, ...){
	va_list arg_list;
	va_start(arg_list, format);

	// SUSv2 version doesn't work for buf NULL/size 0, so try printing
	// into a small buffer that avoids the double-rendering and alloca path too...
	char short_buf[256];
	const size_t needed = vsnprintf(short_buf, sizeof short_buf,
	                                format, arg_list) + 1;
	if (needed <= sizeof(short_buf)){
		va_end(arg_list);
		return short_buf;
	}

	// need more space...
	char* p = static_cast<char*>(alloca(needed));
	vsnprintf(p, needed, format, arg_list);
	va_end(arg_list);
	return p;
}

	// split a string into lines using getline
void string_getlines(std::deque<std::string> &v, const std::string &str){
	std::stringstream stream(str);
	while (1){
		std::string line;
		std::getline(stream,line);
		if(line != "")
			v.push_back(line);
		if (!stream.good())
			break;
	}
}

	// split a string into lines using getline
std::deque<std::string> string_getlines( const std::string &str){
	std::deque<std::string> v;
	std::stringstream stream(str);
	while (1){
		std::string line;
		std::getline(stream,line);

		if(line != "")
			v.push_back(line);
		if (!stream.good())
			break;
	}
	return v;
}

	// split string at every occurance of "c"
void string_split(std::deque<std::string> &v, const char c, const std::string &str){
	int pos = 0;

	for(uint i = 0; i<str.size(); ++i){
		if(str[i] == c){
			if(i-pos > 0 || i == 0)
				v.push_back(str.substr(pos, i-pos));
			pos = i+1;
		}
	}
	if(str.size()-pos > 0)
		v.push_back(str.substr(pos, str.size()-pos)); // add rest
}

// split string at every occurance of "c"
std::deque<std::string> string_split(const char c, const std::string &str){
	std::deque<std::string> v;
	int pos = 0;

	for(uint i = 0; i<str.size(); ++i){
		if(str[i] == c){
			if(i-pos > 0 || i == 0)
				v.push_back(str.substr(pos, i-pos));
			pos = i+1;
		}
	}
	if(str.size()-pos > 0)
		v.push_back(str.substr(pos, str.size()-pos)); // add rest
	return v;
}

void string_join(std::string &res, const char c, const std::deque<std::string> &v){
	res = v[0];
	for(uint i = 1; i<v.size(); ++i){
		res += c + v[i];
	}
}
void string_join(std::string &res, const std::deque<std::string> &v){
	res = "";
	for(auto &str : v){
		res += str;
	}
}
std::string string_join(const char c, const std::deque<std::string> &v){
	std::string res = v[0];
	for(uint i = 1; i<v.size(); ++i){
		res += c + v[i];
	}
	return res;
}
std::string string_join(const std::deque<std::string> &v){
	std::string res = "";
	for(auto &str : v){
		res += str;
	}
	return res;
}
std::string string_join(const std::deque<std::string> &v, uint s){
	std::string res = "";
	for(uint i = s; s<v.size(); ++i){
		res += v[i];
	}
	return res;
}
std::string string_join(const std::deque<std::string> &v, uint s, uint e){
	std::string res = "";
	for(uint i = s; s<e; ++i){
		res += v[i];
	}
	return res;
}

bool string_startswith(const std::string &prefix, const std::string &str){
	auto it1=prefix.begin();
	auto it2=str.begin();

	while( it1 != prefix.end() ){
		if( it2 == str.end() || *it1 != *it2 )
			return false;

		++it1;
		++it2;
	}
	return true;
}

int count_char(const char* str, const char c){
	int i = 0;
	const char *p = str;

	while(( p = strchr(p, c)) != NULL){
		++i; ++p;
	}
	return i;
}




char* alloc_strcpy(char **d, const char *s){
	auto slen = strlen(s);
	if(*d != nullptr) {
		auto dlen = strlen(*d);
		if(slen > dlen){
			delete[] (*d);	        // delete existing content
			(*d) = new char[slen+1]; // allocate new memory
		}
	} else {
		(*d) = new char[slen+1]; // allocate new memory
	}
	return strncpy(*d, s, slen+1);
}
}
