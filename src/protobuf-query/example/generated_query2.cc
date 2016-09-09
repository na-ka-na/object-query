/*
SELECT all_employees.id, all_employees.name FROM ('argv[1]', 'Example1.Company') WHERE ((all_employees.id IS NOT NULL AND (((all_employees.name = 'def') AND (all_employees.active = TRUE)) OR (all_employees.name = 'abc'))) AND (financial.quarterly_profits > 0))

with (company = parseFromFile()) {
  for each quarterly_profit in company.financial().quarterly_profits() {
    if (!(financial.quarterly_profits > 0)) { continue; }
    for each all_employee in company.all_employees() {
      if (!all_employees.id IS NOT NULL) { continue; }
      if (!(((all_employees.name = 'def') AND (all_employees.active = TRUE)) OR (all_employees.name = 'abc'))) { continue; }
      tuples.add(all_employees.id)
      tuples.add(all_employees.name)
      tuples.record()
    } //all_employee
  } //quarterly_profit
} //company
tuples.print('all_employees.id', 'all_employees.name')
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "all_employees.id",
  "all_employees.name",
};
using S0 = optional<bool>;   /*.active()*/
using S1 = optional<int32>;  /*.id()*/
using S2 = optional<string>; /*.name()*/
using S3 = optional<float>;  /*.financial().quarterly_profits()*/
using TupleType = tuple<S1, S2>;

void runSelect(const vector<Company>& companys, vector<TupleType>& tuples) {
  for (const Company* company : Iterators::mk_iterator(&companys)) {
    for (const float* quarterly_profit : Iterators::mk_iterator(company ? &company->financial().quarterly_profits() : nullptr)) {
      S3 s3 = S3();
      if (quarterly_profit) {
        s3 = *quarterly_profit;
      }
      if (!Gt(s3, optional<int64>(0))) { continue; }
      for (const Employee* all_employee : Iterators::mk_iterator(company ? &company->all_employees() : nullptr)) {
        S1 s1 = S1();
        if (all_employee && all_employee->has_id()) {
          s1 = all_employee->id();
        }
        S2 s2 = S2();
        if (all_employee && all_employee->has_name()) {
          s2 = all_employee->name();
        }
        S0 s0 = S0();
        if (all_employee && all_employee->has_active()) {
          s0 = all_employee->active();
        }
        if (!IsNotNull(s1)) { continue; }
        if (!((Eq(s2, optional<string>("def")) && Eq(s0, optional<bool>(true))) || Eq(s2, optional<string>("abc")))) { continue; }
        tuples.emplace_back(s1, s2);
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
  Company company;
  ParsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect({company}, tuples);
  printTuples(tuples);
}
