/*
SELECT all_employees.id, all_employees.name FROM ('argv[1]', 'Example1.Company') WHERE ((all_employees.name = "abc") AND (all_employees.active = "true"))

for (1..1) {
  for each all_employee in company.all_employees() {
    print all_employee.id()
    print all_employee.name()
    print all_employee.active()
    print all_employee.name()
  } //all_employee
} //company
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "all_employees.id",
  "all_employees.name",
};
using S0 = optional<int32>;  /*.id()*/
using S1 = optional<string>; /*.name()*/
using S2 = optional<bool>;   /*.active()*/
using S3 = optional<string>; /*.name()*/
using TupleType = tuple<S0, S1, S2, S3>;

void runSelect(const Company& company, vector<TupleType>& tuples) {
  if (company.ByteSize()) {
    for (int _=0; _<1; _++) {
      if (company.all_employees_size() > 0) {
        for (const Employee& all_employee : company.all_employees()) {
          S0 s0 = S0();
          if(all_employee.has_id()) {
            s0 = all_employee.id();
          }
          S1 s1 = S1();
          if(all_employee.has_name()) {
            s1 = all_employee.name();
          }
          S2 s2 = S2();
          if(all_employee.has_active()) {
            s2 = all_employee.active();
          }
          S3 s3 = S3();
          if(all_employee.has_name()) {
            s3 = all_employee.name();
          }
          tuples.emplace_back(s0, s1, s2, s3);
        }
      } else { // no all_employee
        tuples.emplace_back(S0(), S1(), S2(), S3());
      }
    }
  } else { // no company
    tuples.emplace_back(S0(), S1(), S2(), S3());
  }
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], stringify(get<0>(t)).size());
    sizes[1] = max(sizes[1], stringify(get<1>(t)).size());
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
    cout <<          setw(sizes[0]) << stringify(get<0>(t));
    cout << " | " << setw(sizes[1]) << stringify(get<1>(t));
    cout << endl;
  }
}

int main(int argc, char** argv) {
  Company company;
  parsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect(company, tuples);
  printTuples(tuples);
}
