/*
SELECT all_employees.name, all_employees.active_direct_reports FROM ('argv[1]', 'Example1.Company') WHERE all_employees.active_direct_reports IS NOT NULL

with (company = parseFromFile()) {
  for each all_employee in company.all_employees() {
    tuples.add(all_employees.name)
    for each active_direct_report in all_employee.active_direct_reports() {
      if (!all_employees.active_direct_reports IS NOT NULL) { continue; }
      tuples.add(all_employees.active_direct_reports)
      tuples.record()
    } //active_direct_report
  } //all_employee
} //company
tuples.print('all_employees.name', 'all_employees.active_direct_reports')
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "all_employees.name",
  "all_employees.active_direct_reports",
};
using S0 = optional<int32>;  /*.active_direct_reports()*/
using S1 = optional<string>; /*.name()*/
using TupleType = tuple<S1, S0>;

void runSelect(const vector<Company>& companys, vector<TupleType>& tuples) {
  for (const Company* company : Iterators::mk_iterator(&companys)) {
    for (const Employee* all_employee : Iterators::mk_iterator(company ? &company->all_employees() : nullptr)) {
      S1 s1 = S1();
      if (all_employee && all_employee->has_name()) {
        s1 = all_employee->name();
      }
      for (const int32* active_direct_report : Iterators::mk_iterator(all_employee ? &all_employee->active_direct_reports() : nullptr)) {
        S0 s0 = S0();
        if (active_direct_report) {
          s0 = *active_direct_report;
        }
        if (!IsNotNull(s0)) { continue; }
        tuples.emplace_back(s1, s0);
      }
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

int main(int, char** argv) {
  Company company;
  ParsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect({company}, tuples);
  printTuples(tuples);
}
