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

#ifndef BLH_STRING_HELPERS_HPP
#define BLH_STRING_HELPERS_HPP

#include <string>
#include <deque>

namespace blh {

	std::string stringf(const char* format, ...);

	void string_getlines(std::deque<std::string> &v, const std::string &str);
	std::deque<std::string> string_getlines(const std::string &str);

	void string_split(std::deque<std::string> &v, const char c, const std::string &str);
	std::deque<std::string> string_split(const char c, const std::string &str);

	void string_join(std::string &res, const char c, const std::deque<std::string> &v);
	void string_join(std::string &res, const std::deque<std::string> &v);
	std::string string_join(const char c, const std::deque<std::string> &v);
	std::string string_join(const std::deque<std::string> &v);
	std::string string_join(const std::deque<std::string> &v, uint s);
	std::string string_join(const std::deque<std::string> &v, uint s, uint e);

	bool string_startswith(const std::string &prefix, const std::string &str);
}

#endif // BLH_STRING_HELPERS_HPP
