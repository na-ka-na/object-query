/*
SELECT financial.quarterly_profits, all_employees.name, all_employees.active FROM Example1.Company ORDER BY all_employees.active, financial.quarterly_profits DESC, all_employees.id

with (company = parseFromFile()) {
  for each quarterly_profit in company.financial().quarterly_profits() {
    tuples.add(financial.quarterly_profits)
    for each all_employee in company.all_employees() {
      tuples.add(all_employees.name)
      tuples.add(all_employees.active)
      tuples.add(all_employees.id)
      tuples.record()
    } //all_employee
  } //quarterly_profit
} //company
tuples.sortBy('all_employees.active', 'financial.quarterly_profits', 'all_employees.id')
tuples.print('financial.quarterly_profits', 'all_employees.name', 'all_employees.active')
*/
#include "example1.pb.h"
#include "proto_example_utils.h"
#include "protobuf_generated_common.h"

using namespace std;
using namespace oq;

vector<string> header = {
  "financial.quarterly_profits",
  "all_employees.name",
  "all_employees.active",
};

using S0 = optional<bool>;   /* active() */
using S1 = optional<int32>;  /* id() */
using S2 = optional<MyString>; /* name() */
using S3 = optional<float>;  /* financial().quarterly_profits() */
using TupleType = tuple<S3, S2, S0, S1>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_vec_iterator(&companys)) {
    for (const auto* quarterly_profit : Iterators::mk_pb_iterator(company ? &company->financial().quarterly_profits() : nullptr)) {
      S3 s3 = S3();
      if (quarterly_profit) {
        s3 = *quarterly_profit;
      }
      for (const auto* all_employee : Iterators::mk_pb_iterator(company ? &company->all_employees() : nullptr)) {
        S1 s1 = S1();
        if (all_employee && all_employee->has_id()) {
          s1 = all_employee->id();
        }
        S2 s2 = S2();
        if (all_employee && all_employee->has_name()) {
          s2 = MyString(&(all_employee->name()));
        }
        S0 s0 = S0();
        if (all_employee && all_employee->has_active()) {
          s0 = all_employee->active();
        }
        tuples.emplace_back(s3, s2, s0, s1);
      }
    }
  }
}

bool compareTuples(const TupleType& t1, const TupleType& t2) {
  int c;
  c = Compare(get<2>(t1), get<2>(t2));
  if (c < 0) {return true;} else if (c > 0) {return false;}
  c = -Compare(get<0>(t1), get<0>(t2));
  if (c < 0) {return true;} else if (c > 0) {return false;}
  c = Compare(get<3>(t1), get<3>(t2));
  if (c < 0) {return true;} else if (c > 0) {return false;}
  return false;
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], PrintSize(get<0>(t)));
    sizes[1] = max(sizes[1], PrintSize(get<1>(t)));
    sizes[2] = max(sizes[2], PrintSize(get<2>(t)));
  }
  cout << std::left;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << setw(sizes[i]) << header[i];
  }
  cout << endl;
  for (size_t i=0; i<header.size(); i++) {
    cout << ((i==0) ? "" : " | ") << string(sizes[i], '-');
  }
  cout << endl;
  for(const TupleType& t : tuples) {
    Print(cout <<          setw(sizes[0]), get<0>(t));
    Print(cout << " | " << setw(sizes[1]), get<1>(t));
    Print(cout << " | " << setw(sizes[2]), get<2>(t));
    cout << endl;
  }
}

int main(int argc, char** argv) {
  vector<Example1::Company> companys;
  FROM(argc, argv, companys);
  vector<TupleType> tuples;
  runSelect(companys, tuples);
  std::sort(tuples.begin(), tuples.end(), compareTuples);
  printTuples(tuples);
}
