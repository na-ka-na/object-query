/*
SELECT financial.quarterly_profits, financial.quarterly_revenues, all_employees.id, all_employees.name, all_employees.active, all_employees.active_direct_reports, founded, board_of_directors FROM ('argv[1]', 'Example1.Company')

for (1..1) {
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
} //company
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
  if (company.ByteSize()) {
    for (int _=0; _<1; _++) {
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
  } else { // no company
    tuples.emplace_back(S0(), S1(), S2(), S3(), S4(), S5(), S6(), S7());
  }
}

void printTuples(const vector<TupleType>& tuples) {
  vector<size_t> sizes;
  for (size_t i=0; i<header.size(); i++) {
    sizes.push_back(header[i].size());
  }
  for (const TupleType& t : tuples) {
    sizes[0] = max(sizes[0], Stringify(get<2>(t)).size());
    sizes[1] = max(sizes[1], Stringify(get<3>(t)).size());
    sizes[2] = max(sizes[2], Stringify(get<4>(t)).size());
    sizes[3] = max(sizes[3], Stringify(get<5>(t)).size());
    sizes[4] = max(sizes[4], Stringify(get<6>(t)).size());
    sizes[5] = max(sizes[5], Stringify(get<7>(t)).size());
    sizes[6] = max(sizes[6], Stringify(get<0>(t)).size());
    sizes[7] = max(sizes[7], Stringify(get<1>(t)).size());
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
    cout <<          setw(sizes[0]) << Stringify(get<2>(t));
    cout << " | " << setw(sizes[1]) << Stringify(get<3>(t));
    cout << " | " << setw(sizes[2]) << Stringify(get<4>(t));
    cout << " | " << setw(sizes[3]) << Stringify(get<5>(t));
    cout << " | " << setw(sizes[4]) << Stringify(get<6>(t));
    cout << " | " << setw(sizes[5]) << Stringify(get<7>(t));
    cout << " | " << setw(sizes[6]) << Stringify(get<0>(t));
    cout << " | " << setw(sizes[7]) << Stringify(get<1>(t));
    cout << endl;
  }
}

int main(int argc, char** argv) {
  Company company;
  ParsePbFromFile(argv[1], company);
  vector<TupleType> tuples;
  runSelect(company, tuples);
  printTuples(tuples);
}
