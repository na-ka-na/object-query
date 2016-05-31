/*
print company.founded()
for each board_of_director in company.board_of_directors() {
  print board_of_director
  for each quarterly_profit in company.financial().quarterly_profits() {
    print quarterly_profit
    for each quarterly_revenue in company.financial().quarterly_revenues() {
      print quarterly_revenue
      for each all_employee in company.all_employees() {
        print all_employee.id()
        print all_employee.name()
        print all_employee.active()
        for each active_direct_report in all_employee.active_direct_reports() {
          print active_direct_report
        } //active_direct_report
      } //all_employee
    } //quarterly_revenue
  } //quarterly_profit
} //board_of_director
*/
#include <tuple>
#include <vector>
#include "example1.pb.h"
#include "generated_common.h"
using namespace std;
using namespace Example1;
using S0 = optional<int32>;  /*.founded()*/
using S1 = optional<int32>;  /*.board_of_directors()*/
using S2 = optional<float>;  /*.financial().quarterly_profits()*/
using S3 = optional<float>;  /*.financial().quarterly_revenues()*/
using S4 = optional<int32>;  /*.id()*/
using S5 = optional<string>; /*.name()*/
using S6 = optional<bool>;   /*.active()*/
using S7 = optional<int32>;  /*.active_direct_reports()*/
using TupleType = tuple<S0, S1, S2, S3, S4, S5, S6, S7>;

void runSelect(const Company& company, vector<TupleType>& tuples) {
  S0 s0 = S0();
  if(company.has_founded()) {
    s0 = company.founded();
  }
  if (company.board_of_directors_size() > 0) {
    for (const int32& board_of_director : company.board_of_directors()) {
      S1 s1 = board_of_director;
      if (company.financial().quarterly_profits_size() > 0) {
        for (const float& quarterly_profit : company.financial().quarterly_profits()) {
          S2 s2 = quarterly_profit;
          if (company.financial().quarterly_revenues_size() > 0) {
            for (const float& quarterly_revenue : company.financial().quarterly_revenues()) {
              S3 s3 = quarterly_revenue;
              if (company.all_employees_size() > 0) {
                for (const Employee& all_employee : company.all_employees()) {
                  S4 s4 = S4();
                  if(all_employee.has_id()) {
                    s4 = all_employee.id();
                  }
                  S5 s5 = S5();
                  if(all_employee.has_name()) {
                    s5 = all_employee.name();
                  }
                  S6 s6 = S6();
                  if(all_employee.has_active()) {
                    s6 = all_employee.active();
                  }
                  if (all_employee.active_direct_reports_size() > 0) {
                    for (const int32& active_direct_report : all_employee.active_direct_reports()) {
                      S7 s7 = active_direct_report;
                      tuples.emplace_back(s0, s1, s2, s3, s4, s5, s6, s7);
                    }
                  } else { // no active_direct_report
                    tuples.emplace_back(s0, s1, s2, s3, s4, s5, s6, S7());
                  }
                }
              } else { // no all_employee
                tuples.emplace_back(s0, s1, s2, s3, S4(), S5(), S6(), S7());
              }
            }
          } else { // no quarterly_revenue
            tuples.emplace_back(s0, s1, s2, S3(), S4(), S5(), S6(), S7());
          }
        }
      } else { // no quarterly_profit
        tuples.emplace_back(s0, s1, S2(), S3(), S4(), S5(), S6(), S7());
      }
    }
  } else { // no board_of_director
    tuples.emplace_back(s0, S1(), S2(), S3(), S4(), S5(), S6(), S7());
  }
}