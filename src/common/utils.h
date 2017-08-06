/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <regex>
#include "global_include.h"

using namespace std;

namespace oq {

class Utils {
public:
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

  static const function<string(const string&)> string2str;

  template<typename T>
  static function<string(const T&)> strfn() {return [](const T& t) {return t.str();};}

  static vector<string> splitDotIdentifier(const string& identifier);

  static string makePlural(const string& name);

  static string makeSingular(const string& name);
};

struct CodeGenSpec {
  vector<string> headerIncludes;
};

} // namespace oq
