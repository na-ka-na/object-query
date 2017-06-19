/*
SELECT (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') AS employee, ((((STR(all_employees.id)+' ')+employee)+' ')+STR(founded)) AS employee2 FROM Example1.Company WHERE (employee2 LIKE '.*true.*')

with (company = parseFromFile()) {
  for each all_employee in company.all_employees() {
    if (!(((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)) LIKE '.*true.*')) { continue; }
    tuples.add((((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))
    tuples.add(((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)))
    tuples.record()
  } //all_employee
} //company
tuples.print('(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')')', '((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded))')
*/
#include "example1.pb.h"
#include "proto_example1_utils.h"
#include "protobuf_generated_common.h"

using namespace std;

vector<string> header = {
  "employee",
  "employee2",
};

template<typename Arg0, typename Ret=decltype(STR(Arg0()))>
optional<Ret> $STR(const optional<Arg0>& arg0) {
  if (arg0) {
    return optional<Ret>(STR(*arg0));
  } else {
    return optional<Ret>();
  }
}
using S0 = optional<bool>;   /* active() */
using S1 = optional<int32>;  /* id() */
using S2 = optional<string>; /* name() */
using S3 = optional<int32>;  /* founded() */
using S4 = decltype(Plus(Plus(Plus(Plus(Plus($STR(S1()), optional<string>(": ")), S2()), optional<string>(" (")), $STR(S0())), optional<string>(")"))); /* (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') */
using S5 = decltype(Plus(Plus(Plus(Plus($STR(S1()), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus($STR(S1()), optional<string>(": ")), S2()), optional<string>(" (")), $STR(S0())), optional<string>(")"))), optional<string>(" ")), $STR(S3()))); /* ((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)) */
using TupleType = tuple<S4, S5>;

std::regex r6(".*true.*", std::regex::optimize);

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    S3 s3 = S3();
    if (company && company->has_founded()) {
      s3 = company->founded();
    }
    for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
      S1 s1 = S1();
      if (all_employee && all_employee->has_id()) {
        s1 = all_employee->id();
      }
      S2 s2 = S2();
      if (all_employee && all_employee->has_name()) {
        s2 = all_employee->name();
      }
      S0 s0 = S0();
      if (all_employee && all_employee->has_active()) {
        s0 = all_employee->active();
      }
      if (!Like(Plus(Plus(Plus(Plus($STR(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus($STR(s1), optional<string>(": ")), s2), optional<string>(" (")), $STR(s0)), optional<string>(")"))), optional<string>(" ")), $STR(s3)), r6)) { continue; }
      S4 s4 = Plus(Plus(Plus(Plus(Plus($STR(s1), optional<string>(": ")), s2), optional<string>(" (")), $STR(s0)), optional<string>(")"));
      S5 s5 = Plus(Plus(Plus(Plus($STR(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus($STR(s1), optional<string>(": ")), s2), optional<string>(" (")), $STR(s0)), optional<string>(")"))), optional<string>(" ")), $STR(s3));
      tuples.emplace_back(s4, s5);
    }
  }
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], Stringify(get<0>(t)).size());
    sizes[1] = max(sizes[1], Stringify(get<1>(t)).size());
  }
  cout << left;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
  for(const TupleType& t : tuples) {
    cout <<          setw(sizes[0]) << Stringify(get<0>(t));
    cout << " | " << setw(sizes[1]) << Stringify(get<1>(t));
    cout << endl;
  }
}

int main(int argc, char** argv) {
  vector<Example1::Company> companys;
  FROM(argc, argv, companys);
  vector<TupleType> tuples;
  runSelect(companys, tuples);
  printTuples(tuples);
}
