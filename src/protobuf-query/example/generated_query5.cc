/*
SELECT (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') AS employee FROM ('argv[1]', 'Example1.Company')

for (1..1) {
  company = parseFromFile()
  for each all_employee in company.all_employees() {
    tuples.add((((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))
    tuples.record()
  } //all_employee
} //company
tuples.print('(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')')')
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "employee",
};
using S0 = optional<bool>;   /*.active()*/
using S1 = optional<int32>;  /*.id()*/
using S2 = optional<string>; /*.name()*/
using S3 = decltype(Plus(Plus(Plus(Plus(Plus(ToStr(S1()), optional<string>(": ")), S2()), optional<string>(" (")), ToStr(S0())), optional<string>(")"))); /*(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')')*/
using TupleType = tuple<S3>;

void runSelect(const Company& company, vector<TupleType>& tuples) {
  if (company.ByteSize()) {
    for (int _=0; _<1; _++) {
      if (company.all_employees_size() > 0) {
        for (const Employee& all_employee : company.all_employees()) {
          S1 s1 = S1();
          if(all_employee.has_id()) {
            s1 = all_employee.id();
          }
          S2 s2 = S2();
          if(all_employee.has_name()) {
            s2 = all_employee.name();
          }
          S0 s0 = S0();
          if(all_employee.has_active()) {
            s0 = all_employee.active();
          }
          S3 s3 = Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"));
          tuples.emplace_back(s3);
        }
      } else { // no all_employee
        tuples.emplace_back(S3());
      }
    }
  } else { // no company
    tuples.emplace_back(S3());
  }
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], Stringify(get<0>(t)).size());
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
    cout << endl;
  }
}

int main(int argc, char** argv) {
  Company company;
  ParsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect(company, tuples);
  printTuples(tuples);
}
