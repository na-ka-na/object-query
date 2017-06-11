/*
SELECT financial.quarterly_profits, financial.quarterly_revenues, all_employees.id, all_employees.name, all_employees.active, all_employees.active_direct_reports, all_employees.enumx, all_employees.enumy, founded, board_of_directors FROM Example1.Company

with (company = parseFromFile()) {
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
          tuples.add(all_employees.enumx)
          for each active_direct_report in all_employee.active_direct_reports() {
            tuples.add(all_employees.active_direct_reports)
            for each each_enumy in all_employee.enumy() {
              tuples.add(all_employees.enumy)
              tuples.record()
            } //each_enumy
          } //active_direct_report
        } //all_employee
      } //quarterly_revenue
    } //quarterly_profit
  } //board_of_director
} //company
tuples.print('financial.quarterly_profits', 'financial.quarterly_revenues', 'all_employees.id', 'all_employees.name', 'all_employees.active', 'all_employees.active_direct_reports', 'all_employees.enumx', 'all_employees.enumy', 'founded', 'board_of_directors')
*/
#include "example1.pb.h"
#include "example1_utils.h"
#include "generated_common.h"

using namespace std;

vector<string> header = {
  "financial.quarterly_profits",
  "financial.quarterly_revenues",
  "all_employees.id",
  "all_employees.name",
  "all_employees.active",
  "all_employees.active_direct_reports",
  "all_employees.enumx",
  "all_employees.enumy",
  "founded",
  "board_of_directors",
};
using S0 = optional<bool>;   /* active() */
using S1 = optional<int32>;  /* active_direct_reports() */
using S2 = optional<string>; /* enumx() */
using S3 = optional<string>; /* enumy() */
using S4 = optional<int32>;  /* id() */
using S5 = optional<string>; /* name() */
using S6 = optional<int32>;  /* board_of_directors() */
using S7 = optional<float>;  /* financial().quarterly_profits() */
using S8 = optional<float>;  /* financial().quarterly_revenues() */
using S9 = optional<int32>;  /* founded() */
using TupleType = tuple<S7, S8, S4, S5, S0, S1, S2, S3, S9, S6>;

void runSelect(const vector<Example1::Company>& companys, vector<TupleType>& tuples) {
  for (const auto* company : Iterators::mk_iterator(&companys)) {
    S9 s9 = S9();
    if (company && company->has_founded()) {
      s9 = company->founded();
    }
    for (const auto* board_of_director : Iterators::mk_iterator(company ? &company->board_of_directors() : nullptr)) {
      S6 s6 = S6();
      if (board_of_director) {
        s6 = *board_of_director;
      }
      for (const auto* quarterly_profit : Iterators::mk_iterator(company ? &company->financial().quarterly_profits() : nullptr)) {
        S7 s7 = S7();
        if (quarterly_profit) {
          s7 = *quarterly_profit;
        }
        for (const auto* quarterly_revenue : Iterators::mk_iterator(company ? &company->financial().quarterly_revenues() : nullptr)) {
          S8 s8 = S8();
          if (quarterly_revenue) {
            s8 = *quarterly_revenue;
          }
          for (const auto* all_employee : Iterators::mk_iterator(company ? &company->all_employees() : nullptr)) {
            S4 s4 = S4();
            if (all_employee && all_employee->has_id()) {
              s4 = all_employee->id();
            }
            S5 s5 = S5();
            if (all_employee && all_employee->has_name()) {
              s5 = all_employee->name();
            }
            S0 s0 = S0();
            if (all_employee && all_employee->has_active()) {
              s0 = all_employee->active();
            }
            S2 s2 = S2();
            if (all_employee && all_employee->has_enumx()) {
              s2 = Example1::EnumX_Name(static_cast<Example1::EnumX>(all_employee->enumx()));
            }
            for (const auto* active_direct_report : Iterators::mk_iterator(all_employee ? &all_employee->active_direct_reports() : nullptr)) {
              S1 s1 = S1();
              if (active_direct_report) {
                s1 = *active_direct_report;
              }
              for (const auto* each_enumy : Iterators::mk_iterator(all_employee ? &all_employee->enumy() : nullptr)) {
                S3 s3 = S3();
                if (each_enumy) {
                  s3 = Example1::EnumY_Name(static_cast<Example1::EnumY>(*each_enumy));
                }
                tuples.emplace_back(s7, s8, s4, s5, s0, s1, s2, s3, s9, s6);
              }
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
    sizes[8] = max(sizes[8], Stringify(get<8>(t)).size());
    sizes[9] = max(sizes[9], Stringify(get<9>(t)).size());
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
    cout << " | " << setw(sizes[8]) << Stringify(get<8>(t));
    cout << " | " << setw(sizes[9]) << Stringify(get<9>(t));
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
