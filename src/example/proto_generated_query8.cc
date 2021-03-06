/*
SELECT *, all_employees.* AS emp FROM Example1.Company

with (company = parseFromFile()) {
  tuples.add(founded)
  for each all_employee in company.all_employees() {
    tuples.add(all_employees.id)
    tuples.add(all_employees.name)
    tuples.add(all_employees.active)
    tuples.add(all_employees.enumx)
    tuples.record()
  } //all_employee
} //company
tuples.print('founded', 'all_employees.id', 'all_employees.name', 'all_employees.active', 'all_employees.enumx')
*/
#include "example1.pb.h"
#include "proto_example_utils.h"
#include "protobuf_generated_common.h"

using namespace std;
using namespace oq;

vector<string> header = {
  "founded",
  "emp.id",
  "emp.name",
  "emp.active",
  "emp.enumx",
};

using S0 = optional<bool>;   /* active() */
using S1 = optional<MyString>; /* enumx() */
using S2 = optional<int32>;  /* id() */
using S3 = optional<MyString>; /* name() */
using S4 = optional<int32>;  /* founded() */
using TupleType = tuple<S4, S2, S3, S0, S1>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    S4 s4 = S4();
    if (company && company->has_founded()) {
      s4 = company->founded();
    }
    for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
      S2 s2 = S2();
      if (all_employee && all_employee->has_id()) {
        s2 = all_employee->id();
      }
      S3 s3 = S3();
      if (all_employee && all_employee->has_name()) {
        s3 = MyString(&(all_employee->name()));
      }
      S0 s0 = S0();
      if (all_employee && all_employee->has_active()) {
        s0 = all_employee->active();
      }
      S1 s1 = S1();
      if (all_employee && all_employee->has_enumx()) {
        s1 = MyString(&(Example1::EnumX_Name(static_cast<Example1::EnumX>(all_employee->enumx()))));
      }
      tuples.emplace_back(s4, s2, s3, s0, s1);
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
    sizes[3] = max(sizes[3], PrintSize(get<3>(t)));
    sizes[4] = max(sizes[4], PrintSize(get<4>(t)));
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
    Print(cout << " | " << setw(sizes[3]), get<3>(t));
    Print(cout << " | " << setw(sizes[4]), get<4>(t));
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
