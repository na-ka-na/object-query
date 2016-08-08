/*
SELECT all_employees.id, all_employees.name FROM ('argv[1]', 'Example1.Company') WHERE ((all_employees.id IS NOT NULL AND (((all_employees.name = "def") AND (all_employees.active = TRUE)) OR (all_employees.name = "abc"))) AND (financial.quarterly_profits > 0))

for (1..1) {
  for each quarterly_profit in company.financial().quarterly_profits() {
    if (!(financial.quarterly_profits > 0)) { continue; }
    for each all_employee in company.all_employees() {
      if (!all_employees.id IS NOT NULL) { continue; }
      if (!(((all_employees.name = "def") AND (all_employees.active = TRUE)) OR (all_employees.name = "abc"))) { continue; }
      print all_employee.id()
      print all_employee.name()
    } //all_employee
  } //quarterly_profit
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
using S0 = optional<float>;  /*.financial().quarterly_profits()*/
using S1 = optional<int32>;  /*.id()*/
using S2 = optional<string>; /*.name()*/
using S3 = optional<bool>;   /*.active()*/
using TupleType = tuple<S1, S2>;

void runSelect(const Company& company, vector<TupleType>& tuples) {
  if (company.ByteSize()) {
    for (int _=0; _<1; _++) {
      if (company.financial().quarterly_profits_size() > 0) {
        for (const float& quarterly_profit : company.financial().quarterly_profits()) {
          S0 s0 = quarterly_profit;
          if (!gt(s0, optional<int64_t>(0))) { continue; }
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
              S3 s3 = S3();
              if(all_employee.has_active()) {
                s3 = all_employee.active();
              }
              if (!isNotNull(s1)) { continue; }
              if (!((eq(s2, optional<string>("def")) && eq(s3, optional<bool>(true))) || eq(s2, optional<string>("abc")))) { continue; }
              tuples.emplace_back(s1, s2);
            }
          } else { // no all_employee
            tuples.emplace_back(S1(), S2());
          }
        }
      } else { // no quarterly_profit
        tuples.emplace_back(S1(), S2());
      }
    }
  } else { // no company
    tuples.emplace_back(S1(), S2());
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
