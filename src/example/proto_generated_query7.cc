/*
SELECT EmployeeStr(all_employees.name,all_employees.id), STR(all_employees.id), STR(all_employees) FROM Example1.Company

with (company = parseFromFile()) {
  for each all_employee in company.all_employees() {
    tuples.add(EmployeeStr(all_employees.name,all_employees.id))
    tuples.add(STR(all_employees.id))
    tuples.add(STR(all_employees))
    tuples.record()
  } //all_employee
} //company
tuples.print('EmployeeStr(all_employees.name,all_employees.id)', 'STR(all_employees.id)', 'STR(all_employees)')
*/
#include "example1.pb.h"
#include "proto_example_utils.h"
#include "protobuf_generated_common.h"

using namespace std;

vector<string> header = {
  "EmployeeStr(all_employees.name,all_employees.id)",
  "STR(all_employees.id)",
  "STR(all_employees)",
};

template<typename Arg0, typename Arg1, typename Ret=decltype(EmployeeStr(Arg0(), Arg1()))>
optional<Ret> $EmployeeStr(const optional<Arg0>& arg0, const optional<Arg1>& arg1) {
  if (arg0 && arg1) {
    return optional<Ret>(EmployeeStr(*arg0, *arg1));
  } else {
    return optional<Ret>();
  }
}
template<typename Arg0, typename Ret=decltype(STR(Arg0()))>
optional<Ret> $STR(const optional<Arg0>& arg0) {
  if (arg0) {
    return optional<Ret>(STR(*arg0));
  } else {
    return optional<Ret>();
  }
}

using S0 = optional<Example1::Employee>; /* all_employees() */
using S1 = optional<int32>;  /* id() */
using S2 = optional<MyString>; /* name() */
using S3 = decltype($EmployeeStr(S2(), S1())); /* EmployeeStr(all_employees.name,all_employees.id) */
using S4 = decltype($STR(S1())); /* STR(all_employees.id) */
using S5 = decltype($STR(S0())); /* STR(all_employees) */
using TupleType = tuple<S3, S4, S5>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
      S0 s0 = S0();
      if (all_employee) {
        s0 = *all_employee;
      }
      S1 s1 = S1();
      if (all_employee && all_employee->has_id()) {
        s1 = all_employee->id();
      }
      S2 s2 = S2();
      if (all_employee && all_employee->has_name()) {
        s2 = MyString(&(all_employee->name()));
      }
      S3 s3 = $EmployeeStr(s2, s1);
      S4 s4 = $STR(s1);
      S5 s5 = $STR(s0);
      tuples.emplace_back(s3, s4, s5);
    }
  }
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], PrintSize(get<0>(t)));
    sizes[1] = max(sizes[1], PrintSize(get<1>(t)));
    sizes[2] = max(sizes[2], PrintSize(get<2>(t)));
  }
  cout << std::left;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
  for(const TupleType& t : tuples) {
    Print(cout <<          setw(sizes[0]), get<0>(t));
    Print(cout << " | " << setw(sizes[1]), get<1>(t));
    Print(cout << " | " << setw(sizes[2]), get<2>(t));
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
