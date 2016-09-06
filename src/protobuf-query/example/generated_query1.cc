/*
SELECT financial.quarterly_profits, financial.quarterly_revenues, all_employees.id, all_employees.name, all_employees.active, all_employees.active_direct_reports, founded, board_of_directors FROM ('argv[1]', 'Example1.Company')

for (1..1) {
  company = parseFromFile()
  tuples.add(founded)
  for each board_of_director in company.board_of_directors() {
    tuples.add(board_of_directors)
    for each quarterly_profit in company.financial().quarterly_profits() {
      tuples.add(financial.quarterly_profits)
      for each quarterly_revenue in company.financial().quarterly_revenues() {
        tuples.add(financial.quarterly_revenues)
        for each all_employee in company.all_employees() {
          tuples.add(all_employees.id)
          tuples.add(all_employees.name)
          tuples.add(all_employees.active)
          for each active_direct_report in all_employee.active_direct_reports() {
            tuples.add(all_employees.active_direct_reports)
            tuples.record()
          } //active_direct_report
        } //all_employee
      } //quarterly_revenue
    } //quarterly_profit
  } //board_of_director
} //company
tuples.print('financial.quarterly_profits', 'financial.quarterly_revenues', 'all_employees.id', 'all_employees.name', 'all_employees.active', 'all_employees.active_direct_reports', 'founded', 'board_of_directors')
*/
#include "example1.pb.h"
#include "generated_common.h"

using namespace std;
using namespace Example1;

vector<string> header = {
  "financial.quarterly_profits",
  "financial.quarterly_revenues",
  "all_employees.id",
  "all_employees.name",
  "all_employees.active",
  "all_employees.active_direct_reports",
  "founded",
  "board_of_directors",
};
using S0 = optional<bool>;   /*.active()*/
using S1 = optional<int32>;  /*.active_direct_reports()*/
using S2 = optional<int32>;  /*.id()*/
using S3 = optional<string>; /*.name()*/
using S4 = optional<int32>;  /*.board_of_directors()*/
using S5 = optional<float>;  /*.financial().quarterly_profits()*/
using S6 = optional<float>;  /*.financial().quarterly_revenues()*/
using S7 = optional<int32>;  /*.founded()*/
using TupleType = tuple<S5, S6, S2, S3, S0, S1, S7, S4>;

void runSelect(const vector<Company>& companies, vector<TupleType>& tuples) {
  for (const Company* company : Iterators::mk_iterator(&companies)) {
    S7 s7 = S7();
    if (company && company->has_founded()) {
      s7 = company->founded();
    }
    for (const int32* board_of_director : Iterators::mk_iterator(company ? &company->board_of_directors() : nullptr)) {
      S4 s4 = S4();
      if (board_of_director) {
        s4 = *board_of_director;
      }
      for (const float* quarterly_profit : Iterators::mk_iterator(company ? &company->financial().quarterly_profits() : nullptr)) {
        S5 s5 = S5();
        if (quarterly_profit) {
          s5 = *quarterly_profit;
        }
        for (const float* quarterly_revenue : Iterators::mk_iterator(company ? &company->financial().quarterly_revenues() : nullptr)) {
          S6 s6 = S6();
          if (quarterly_revenue) {
            s6 = *quarterly_revenue;
          }
          for (const Employee* all_employee : Iterators::mk_iterator(company ? &company->all_employees() : nullptr)) {
            S2 s2 = S2();
            if (all_employee && all_employee->has_id()) {
              s2 = all_employee->id();
            }
            S3 s3 = S3();
            if (all_employee && all_employee->has_name()) {
              s3 = all_employee->name();
            }
            S0 s0 = S0();
            if (all_employee && all_employee->has_active()) {
              s0 = all_employee->active();
            }
            for (const int32* active_direct_report : Iterators::mk_iterator(all_employee ? &all_employee->active_direct_reports() : nullptr)) {
              S1 s1 = S1();
              if (active_direct_report) {
                s1 = *active_direct_report;
              }
              tuples.emplace_back(s5, s6, s2, s3, s0, s1, s7, s4);
            }
          }
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
    sizes[2] = max(sizes[2], Stringify(get<2>(t)).size());
    sizes[3] = max(sizes[3], Stringify(get<3>(t)).size());
    sizes[4] = max(sizes[4], Stringify(get<4>(t)).size());
    sizes[5] = max(sizes[5], Stringify(get<5>(t)).size());
    sizes[6] = max(sizes[6], Stringify(get<6>(t)).size());
    sizes[7] = max(sizes[7], Stringify(get<7>(t)).size());
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
    cout << " | " << setw(sizes[3]) << Stringify(get<3>(t));
    cout << " | " << setw(sizes[4]) << Stringify(get<4>(t));
    cout << " | " << setw(sizes[5]) << Stringify(get<5>(t));
    cout << " | " << setw(sizes[6]) << Stringify(get<6>(t));
    cout << " | " << setw(sizes[7]) << Stringify(get<7>(t));
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
