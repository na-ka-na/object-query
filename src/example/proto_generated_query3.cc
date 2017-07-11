/*
SELECT all_employees.id, (financial.quarterly_revenues/all_employees.active_direct_reports) FROM Example1.Company WHERE ((all_employees.active_direct_reports > 0) AND ((financial.quarterly_revenues*2) > 1))

with (company = parseFromFile()) {
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
#include "proto_example_utils.h"
#include "protobuf_generated_common.h"

using namespace std;

vector<string> header = {
  "all_employees.id",
  "(financial.quarterly_revenues/all_employees.active_direct_reports)",
};

auto $c1 = 0;
auto $c3 = 1;
auto $c2 = 2;

using S0 = optional<int32>;  /* active_direct_reports() */
using S1 = optional<int32>;  /* id() */
using S2 = optional<float>;  /* financial().quarterly_revenues() */
using S3 = decltype(Divide(S2(), S0())); /* (financial.quarterly_revenues/all_employees.active_direct_reports) */
using TupleType = tuple<S1, S3>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    for (const auto* quarterly_revenue : Iterators::mk_pb_iterator(company ? &company->financial().quarterly_revenues() : nullptr)) {
      S2 s2 = S2();
      if (quarterly_revenue) {
        s2 = *quarterly_revenue;
      }
      if (!Gt(Mult(s2, &$c2), &$c3)) { continue; }
      for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
        S1 s1 = S1();
        if (all_employee && all_employee->has_id()) {
          s1 = all_employee->id();
        }
        for (const auto* active_direct_report : Iterators::mk_pb_iterator(all_employee ? &all_employee->active_direct_reports() : nullptr)) {
          S0 s0 = S0();
          if (active_direct_report) {
            s0 = *active_direct_report;
          }
          if (!Gt(s0, &$c1)) { continue; }
          S3 s3 = Divide(s2, s0);
          tuples.emplace_back(s1, s3);
        }
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
