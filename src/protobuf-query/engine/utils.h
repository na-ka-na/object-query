
#pragma once

#include <string>
#include <vector>
#include <functional>

using namespace std;

template<typename T, typename It>
static string join(const string& delim, It begin, It end,
                   const function<string(const T&)>& to_str) {
  string joined;
  if (begin != end) {
     joined += to_str(*begin);
     while ((++begin) != end) {
       joined += delim;
       joined += to_str(*begin);
     }
  }
  return joined;
}

template<typename T>
static string joinVec(const string& delim, const vector<T>& ts,
                      const function<string(const T&)>& to_str) {
  return join<T>(delim, ts.cbegin(), ts.cend(), to_str);
}

const function<string(const string&)> string2str = [](const string& str) {return str;};

template<typename T>
static function<string(const T&)> strfn() {return [](const T& t) {return t.str();};}

static vector<string> splitString(string str, char delim) {
  vector<string> parts;
  if (str.length() == 0) {
    return parts;
  }
  string::size_type i = 0;
  string::size_type j = str.find(delim);
  while (j != string::npos) {
    parts.push_back(str.substr(i, j-i));
    i = ++j;
    j = str.find(delim, j);
  }
  if (j == string::npos) {
    parts.push_back(str.substr(i, str.length()));
  }
  return parts;
}
