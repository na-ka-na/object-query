/*
SELECT all_employees.id, (financial.quarterly_revenues/all_employees.active_direct_reports) FROM ('argv[1]', 'Example1.Company') WHERE ((all_employees.active_direct_reports > 0) AND ((financial.quarterly_revenues*2) > 1))

for (1..1) {
  company = parseFromFile()
  for each quarterly_revenue in company.financial().quarterly_revenues() {
    if (!((financial.quarterly_revenues*2) > 1)) { continue; }
    for each all_employee in company.all_employees() {
      tuples.add(all_employees.id)
      for each active_direct_report in all_employee.active_direct_reports() {
        if (!(all_employees.active_direct_reports > 0)) { continue; }
        tuples.add((financial.quarterly_revenues/all_employees.active_direct_reports))
        tuples.record()
      } //active_direct_report
    } //all_employee
  } //quarterly_revenue
} //company
tuples.print('all_employees.id', '(financial.quarterly_revenues/all_employees.active_direct_reports)')
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "all_employees.id",
  "(financial.quarterly_revenues/all_employees.active_direct_reports)",
};
using S0 = optional<int32>;  /*.active_direct_reports()*/
using S1 = optional<int32>;  /*.id()*/
using S2 = optional<float>;  /*.financial().quarterly_revenues()*/
using S3 = decltype(Divide(S2(), S0())); /*(financial.quarterly_revenues/all_employees.active_direct_reports)*/
using TupleType = tuple<S1, S3>;

void runSelect(const vector<Company>& companies, vector<TupleType>& tuples) {
for (int _=0; _<1; _++) { // dummy loop
  if (companies.size() > 0) {
    for (const Company& company: companies) {
      if (company.financial().quarterly_revenues_size() > 0) {
        for (const float& quarterly_revenue : company.financial().quarterly_revenues()) {
          S2 s2 = quarterly_revenue;
          if (!Gt(Mult(s2, optional<int64>(2)), optional<int64>(1))) { continue; }
          if (company.all_employees_size() > 0) {
            for (const Employee& all_employee : company.all_employees()) {
              S1 s1 = S1();
              if(all_employee.has_id()) {
                s1 = all_employee.id();
              }
              if (all_employee.active_direct_reports_size() > 0) {
                for (const int32& active_direct_report : all_employee.active_direct_reports()) {
                  S0 s0 = active_direct_report;
                  if (!Gt(s0, optional<int64>(0))) { continue; }
                  S3 s3 = Divide(s2, s0);
                  tuples.emplace_back(s1, s3);
                }
              } else { // no active_direct_report
                S0 s0 = S0();
                S3 s3 = S3();
                if (!Gt(s0, optional<int64>(0))) { continue; }
                tuples.emplace_back(s1, s3);
              }
            }
          } else { // no all_employee
            S1 s1 = S1();
            S0 s0 = S0();
            S3 s3 = S3();
            if (!Gt(s0, optional<int64>(0))) { continue; }
            tuples.emplace_back(s1, s3);
          }
        }
      } else { // no quarterly_revenue
        S2 s2 = S2();
        S1 s1 = S1();
        S0 s0 = S0();
        S3 s3 = S3();
        if (!Gt(s0, optional<int64>(0))) { continue; }
        if (!Gt(Mult(s2, optional<int64>(2)), optional<int64>(1))) { continue; }
        tuples.emplace_back(s1, s3);
      }
    }
  } else { // no company
    S2 s2 = S2();
    S1 s1 = S1();
    S0 s0 = S0();
    S3 s3 = S3();
    if (!Gt(s0, optional<int64>(0))) { continue; }
    if (!Gt(Mult(s2, optional<int64>(2)), optional<int64>(1))) { continue; }
    tuples.emplace_back(s1, s3);
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
