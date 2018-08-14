#pragma once

#include <cstdio>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

class StringParser {
public:

	template<typename... Args>
	static const string toStringF(string format, Args... args) {
		char buffer[8192];
		sprintf(buffer, format.c_str(), args...);
		return string(buffer);
	}

	//把所有pair.first替换成pair.second
	static const string replaceSubString(string source, initializer_list<pair<string, string>> replace) {
		string dest;

		for (pair<string, string> i : replace) {
			dest = ""s;
			for (int j = 0; j < source.size();) {
				if (source.substr(j, i.first.size()) == i.first) {
					j += i.first.size();
					dest += i.second;
				}
				else {
					dest += source[j];
					j++;
				}
			}
			source = dest;
		}
		return source;
	}

	//将各种整数/浮点数转换为字符串
	//或者考虑std::to_string()??? 
	static const string toString(bool                   data) { return to_string((int)data); }
	static const string toString(short                  data) { return to_string((int)data); }
	static const string toString(unsigned short         data) { return to_string((int)data); }
	static const string toString(int                    data) { return to_string(data); }
	static const string toString(unsigned int           data) { return to_string(data); }
	static const string toString(long long              data) { return to_string(data); }
	static const string toString(unsigned long long     data) { return to_string(data); }
	static const string toString(float                  data) { return to_string(data); }
	static const string toString(double                 data) { return to_string(data); }

	/* 过时
	static const string toString(bool                   data) { char buff[48]; sprintf(buff, "%d", data);   return string(buff); }
	static const string toString(short                  data) { char buff[48]; sprintf(buff, "%d", data);   return string(buff); }
	static const string toString(unsigned short         data) { char buff[48]; sprintf(buff, "%d", data);   return string(buff); }
	static const string toString(int                    data) { char buff[48]; sprintf(buff, "%d", data);   return string(buff); }
	static const string toString(unsigned int           data) { char buff[48]; sprintf(buff, "%u", data);   return string(buff); }
	static const string toString(long long              data) { char buff[48]; sprintf(buff, "%lld", data); return string(buff); }
	static const string toString(unsigned long long     data) { char buff[48]; sprintf(buff, "%llu", data); return string(buff); }
	static const string toString(float                  data) { char buff[48]; sprintf(buff, "%f", data);   return string(buff); }
	static const string toString(double                 data) { char buff[48]; sprintf(buff, "%lf", data);  return string(buff); }
	*/

	//将字符串转换为各种整数/浮点数
	static const bool      toBool(string&     data) { int x;       sscanf(data.c_str(), "%d", &x);   return x; }
	static const short     toShort(string&    data) { int x;       sscanf(data.c_str(), "%d", &x);   return x; }
	static const int       toInt(string&      data) { int x;       sscanf(data.c_str(), "%d", &x);   return x; }
	static const long long toLongLong(string& data) { long long x; sscanf(data.c_str(), "%lld", &x); return x; }
	static const float     toFloat(string&    data) { float x;     sscanf(data.c_str(), "%f", &x);   return x; }
	static const double    toDouble(string&   data) { double x;    sscanf(data.c_str(), "%lf", &x);  return x; }

};
