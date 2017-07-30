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
#include "proto_example_utils.h"
#include "protobuf_generated_common.h"

using namespace std;

vector<string> header = {
  "employee",
  "employee2",
};

string $c4 = " ";
string $c2 = " (";
string $c3 = ")";
regex $c5 = regex(".*true.*", regex::optimize);
string $c1 = ": ";

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
using S2 = optional<MyString>; /* name() */
using S3 = optional<int32>;  /* founded() */
using S4 = decltype(Plus(Plus(Plus(Plus(Plus($STR(S1()), &$c1), S2()), &$c2), $STR(S0())), &$c3)); /* (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') */
using S5 = decltype(Plus(Plus(Plus(Plus($STR(S1()), &$c4), Plus(Plus(Plus(Plus(Plus($STR(S1()), &$c1), S2()), &$c2), $STR(S0())), &$c3)), &$c4), $STR(S3()))); /* ((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)) */
using TupleType = tuple<S4, S5>;

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
        s2 = MyString(&(all_employee->name()));
      }
      S0 s0 = S0();
      if (all_employee && all_employee->has_active()) {
        s0 = all_employee->active();
      }
      if (!Like(Plus(Plus(Plus(Plus($STR(s1), &$c4), Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3)), &$c4), $STR(s3)), $c5)) { continue; }
      S4 s4 = Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3);
      S5 s5 = Plus(Plus(Plus(Plus($STR(s1), &$c4), Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3)), &$c4), $STR(s3));
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
    sizes[0] = max(sizes[0], PrintSize(get<0>(t)));
    sizes[1] = max(sizes[1], PrintSize(get<1>(t)));
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
