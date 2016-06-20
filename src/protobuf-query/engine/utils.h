
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
