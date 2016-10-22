#include "generated_common.h"
#include "example1.pb.h"

template<>
inline string Stringify(const Example1::Employee& t) {
  return t.name() + "(id=" + to_string(t.id()) + ")";
}
