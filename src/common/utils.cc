/*
Copyright (c) 2016 https://github.com/na-ka-na

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain the License at http://www.apache.org/licenses/LICENSE-2.0
*/

#include "utils.h"
#include "global_include.h"

using namespace oq;

const function<string(const string&)> Utils::string2str = [](const string& str) {return str;};

vector<string> Utils::splitDotIdentifier(const string& identifier) {
  static regex notdot("([^.]+)");
  auto partsBegin = sregex_iterator(identifier.begin(), identifier.end(), notdot);
  vector<string> selectFieldParts;
  for (sregex_iterator it = partsBegin; it != sregex_iterator(); ++it) {
    selectFieldParts.push_back(it->str());
  }
  return selectFieldParts;
}

string Utils::makePlural(const string& name) {
  ASSERT(!name.empty());
  return name + "s";
}

string Utils::makeSingular(const string& name) {
  ASSERT(!name.empty());
  if ((name.size() > 1) && (name.substr(name.size()-1, 1) == "s")) {
    return name.substr(0, name.size()-1);
  } else {
    return "each_" + name;
  }
}

