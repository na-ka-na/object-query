/*
SELECT (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') AS employee, ((((STR(all_employees.id)+' ')+employee)+' ')+STR(founded)) AS employee2 FROM ('argv[1]', 'Example1.Company') WHERE (employee2 LIKE '.*true.*')

for (1..1) {
  company = parseFromFile()
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
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "employee",
  "employee2",
};
using S0 = optional<bool>;   /*.active()*/
using S1 = optional<int32>;  /*.id()*/
using S2 = optional<string>; /*.name()*/
using S3 = optional<int32>;  /*.founded()*/
using S4 = decltype(Plus(Plus(Plus(Plus(Plus(ToStr(S1()), optional<string>(": ")), S2()), optional<string>(" (")), ToStr(S0())), optional<string>(")"))); /*(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')')*/
using S5 = decltype(Plus(Plus(Plus(Plus(ToStr(S1()), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus(ToStr(S1()), optional<string>(": ")), S2()), optional<string>(" (")), ToStr(S0())), optional<string>(")"))), optional<string>(" ")), ToStr(S3()))); /*((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded))*/
using TupleType = tuple<S4, S5>;

std::regex r6(".*true.*", std::regex::optimize);

void runSelect(const vector<Company>& companies, vector<TupleType>& tuples) {
for (int _=0; _<1; _++) { // dummy loop
  if (companies.size() > 0) {
    for (const Company& company: companies) {
      S3 s3 = S3();
      if(company.has_founded()) {
        s3 = company.founded();
      }
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
          if (!Like(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"))), optional<string>(" ")), ToStr(s3)), r6)) { continue; }
          S4 s4 = Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"));
          S5 s5 = Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"))), optional<string>(" ")), ToStr(s3));
          tuples.emplace_back(s4, s5);
        }
      } else { // no all_employee
        S1 s1 = S1();
        S2 s2 = S2();
        S0 s0 = S0();
        S4 s4 = S4();
        S5 s5 = S5();
        if (!Like(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"))), optional<string>(" ")), ToStr(s3)), r6)) { continue; }
        tuples.emplace_back(s4, s5);
      }
    }
  } else { // no company
    S3 s3 = S3();
    S1 s1 = S1();
    S2 s2 = S2();
    S0 s0 = S0();
    S4 s4 = S4();
    S5 s5 = S5();
    if (!Like(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(" ")), Plus(Plus(Plus(Plus(Plus(ToStr(s1), optional<string>(": ")), s2), optional<string>(" (")), ToStr(s0)), optional<string>(")"))), optional<string>(" ")), ToStr(s3)), r6)) { continue; }
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
  Company company;
  ParsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect({company}, tuples);
  printTuples(tuples);
}
