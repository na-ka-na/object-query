/*
SELECT all_employees.name, all_employees.active_direct_reports FROM Example1.Company WHERE all_employees.active_direct_reports IS NOT NULL

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
#include "proto_example1_utils.h"
#include "protobuf_generated_common.h"

using namespace std;

vector<string> header = {
  "all_employees.name",
  "all_employees.active_direct_reports",
};
using S0 = optional<int32>;  /* active_direct_reports() */
using S1 = optional<string>; /* name() */
using TupleType = tuple<S1, S0>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
      S1 s1 = S1();
      if (all_employee && all_employee->has_name()) {
        s1 = all_employee->name();
      }
      for (const auto* active_direct_report : Iterators::mk_pb_iterator(all_employee ? &all_employee->active_direct_reports() : nullptr)) {
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

int main(int argc, char** argv) {
  vector<Example1::Company> companys;
  FROM(argc, argv, companys);
  vector<TupleType> tuples;
  runSelect(companys, tuples);
  printTuples(tuples);
}
