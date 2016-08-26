/*
SELECT financial.quarterly_profits, all_employees.name, all_employees.active FROM ('argv[1]', 'Example1.Company') ORDER BY all_employees.active, financial.quarterly_profits, all_employees.id

for (1..1) {
  company = parseFromFile()
  for each quarterly_profit in company.financial().quarterly_profits() {
    print financial.quarterly_profits
    for each all_employee in company.all_employees() {
      print all_employees.name
      print all_employees.active
    } //all_employee
  } //quarterly_profit
} //company
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "financial.quarterly_profits",
  "all_employees.name",
  "all_employees.active",
};
using S0 = optional<float>;  /*.financial().quarterly_profits()*/
using S1 = optional<string>; /*.name()*/
using S2 = optional<bool>;   /*.active()*/
using TupleType = tuple<S0, S1, S2>;

void runSelect(const Company& company, vector<TupleType>& tuples) {
  if (company.ByteSize()) {
    for (int _=0; _<1; _++) {
      if (company.financial().quarterly_profits_size() > 0) {
        for (const float& quarterly_profit : company.financial().quarterly_profits()) {
          S0 s0 = quarterly_profit;
          if (company.all_employees_size() > 0) {
            for (const Employee& all_employee : company.all_employees()) {
              S1 s1 = S1();
              if(all_employee.has_name()) {
                s1 = all_employee.name();
              }
              S2 s2 = S2();
              if(all_employee.has_active()) {
                s2 = all_employee.active();
              }
              tuples.emplace_back(s0, s1, s2);
            }
          } else { // no all_employee
            tuples.emplace_back(s0, S1(), S2());
          }
        }
      } else { // no quarterly_profit
        tuples.emplace_back(S0(), S1(), S2());
      }
    }
  } else { // no company
    tuples.emplace_back(S0(), S1(), S2());
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
    sizes[2] = max(sizes[2], Stringify(get<2>(t)).size());
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
    cout << " | " << setw(sizes[2]) << Stringify(get<2>(t));
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
