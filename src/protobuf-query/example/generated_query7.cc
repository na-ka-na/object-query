/*
SELECT all_employees FROM Example1.Company

with (company = parseFromFile()) {
  for each all_employee in company.all_employees() {
    tuples.add(all_employees)
    tuples.record()
  } //all_employee
} //company
tuples.print('all_employees')
*/
#include "example1.pb.h"
#include "example1_utils.h"
#include "generated_common.h"

using namespace std;

vector<string> header = {
  "all_employees",
};
using S0 = optional<Example1::Employee>; /*.all_employees()*/
using TupleType = tuple<S0>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_iterator(&companys)) {
    for (const auto* all_employee : Iterators::mk_iterator(company ? &company->all_employees() : nullptr)) {
      S0 s0 = S0();
      if (all_employee) {
        s0 = *all_employee;
      }
      tuples.emplace_back(s0);
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
  vector<Example1::Company> companys;
  FROM(argc, argv, companys);
  vector<TupleType> tuples;
  runSelect(companys, tuples);
  printTuples(tuples);
}
