/*
SELECT financial.quarterly_profits, financial.quarterly_revenues, all_employees.id, all_employees.name, all_employees.active, all_employees.active_direct_reports, all_employees.enumx, all_employees.enumy, founded, board_of_directors FROM Example1.Company

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each each_active in all_employee.all_employees.active {
      tuples.add(all_employees.active)
      for each active_direct_report in all_employee.all_employees.active_direct_reports {
        tuples.add(all_employees.active_direct_reports)
        for each each_enumx in all_employee.all_employees.enumx {
          tuples.add(all_employees.enumx)
          for each each_enumy in all_employee.all_employees.enumy {
            tuples.add(all_employees.enumy)
            for each each_id in all_employee.all_employees.id {
              tuples.add(all_employees.id)
              for each each_name in all_employee.all_employees.name {
                tuples.add(all_employees.name)
                for each board_of_director in document.board_of_directors {
                  tuples.add(board_of_directors)
                  for each each_financial in document.financial {
                    for each quarterly_profit in each_financial.financial.quarterly_profits {
                      tuples.add(financial.quarterly_profits)
                      for each quarterly_revenue in each_financial.financial.quarterly_revenues {
                        tuples.add(financial.quarterly_revenues)
                        for each each_founded in document.founded {
                          tuples.add(founded)
                          tuples.record()
                        } //each_founded
                      } //quarterly_revenue
                    } //quarterly_profit
                  } //each_financial
                } //board_of_director
              } //each_name
            } //each_id
          } //each_enumy
        } //each_enumx
      } //active_direct_report
    } //each_active
  } //all_employee
} //document
tuples.print('financial.quarterly_profits', 'financial.quarterly_revenues', 'all_employees.id', 'all_employees.name', 'all_employees.active', 'all_employees.active_direct_reports', 'all_employees.enumx', 'all_employees.enumy', 'founded', 'board_of_directors')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;
using namespace oq;

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

using S0 = optional<JsonValue>; /* all_employees.active */
using S1 = optional<JsonValue>; /* all_employees.active_direct_reports */
using S2 = optional<JsonValue>; /* all_employees.enumx */
using S3 = optional<JsonValue>; /* all_employees.enumy */
using S4 = optional<JsonValue>; /* all_employees.id */
using S5 = optional<JsonValue>; /* all_employees.name */
using S6 = optional<JsonValue>; /* board_of_directors */
using S7 = optional<JsonValue>; /* financial.quarterly_profits */
using S8 = optional<JsonValue>; /* financial.quarterly_revenues */
using S9 = optional<JsonValue>; /* founded */
using TupleType = tuple<S7, S8, S4, S5, S0, S1, S2, S3, S9, S6>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* each_active : Iterators::mk_json_iterator("all_employees.active", all_employee, "active")) {
        S0 s0 = each_active ? *each_active : S0();
        for (const auto* active_direct_report : Iterators::mk_json_iterator("all_employees.active_direct_reports", all_employee, "active_direct_reports")) {
          S1 s1 = active_direct_report ? *active_direct_report : S1();
          for (const auto* each_enumx : Iterators::mk_json_iterator("all_employees.enumx", all_employee, "enumx")) {
            S2 s2 = each_enumx ? *each_enumx : S2();
            for (const auto* each_enumy : Iterators::mk_json_iterator("all_employees.enumy", all_employee, "enumy")) {
              S3 s3 = each_enumy ? *each_enumy : S3();
              for (const auto* each_id : Iterators::mk_json_iterator("all_employees.id", all_employee, "id")) {
                S4 s4 = each_id ? *each_id : S4();
                for (const auto* each_name : Iterators::mk_json_iterator("all_employees.name", all_employee, "name")) {
                  S5 s5 = each_name ? *each_name : S5();
                  for (const auto* board_of_director : Iterators::mk_json_iterator("board_of_directors", document, "board_of_directors")) {
                    S6 s6 = board_of_director ? *board_of_director : S6();
                    for (const auto* each_financial : Iterators::mk_json_iterator("financial", document, "financial")) {
                      for (const auto* quarterly_profit : Iterators::mk_json_iterator("financial.quarterly_profits", each_financial, "quarterly_profits")) {
                        S7 s7 = quarterly_profit ? *quarterly_profit : S7();
                        for (const auto* quarterly_revenue : Iterators::mk_json_iterator("financial.quarterly_revenues", each_financial, "quarterly_revenues")) {
                          S8 s8 = quarterly_revenue ? *quarterly_revenue : S8();
                          for (const auto* each_founded : Iterators::mk_json_iterator("founded", document, "founded")) {
                            S9 s9 = each_founded ? *each_founded : S9();
                            tuples.emplace_back(s7, s8, s4, s5, s0, s1, s2, s3, s9, s6);
                          }
                        }
                      }
                    }
                  }
                }
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
    sizes[0] = max(sizes[0], PrintSize(get<0>(t)));
    sizes[1] = max(sizes[1], PrintSize(get<1>(t)));
    sizes[2] = max(sizes[2], PrintSize(get<2>(t)));
    sizes[3] = max(sizes[3], PrintSize(get<3>(t)));
    sizes[4] = max(sizes[4], PrintSize(get<4>(t)));
    sizes[5] = max(sizes[5], PrintSize(get<5>(t)));
    sizes[6] = max(sizes[6], PrintSize(get<6>(t)));
    sizes[7] = max(sizes[7], PrintSize(get<7>(t)));
    sizes[8] = max(sizes[8], PrintSize(get<8>(t)));
    sizes[9] = max(sizes[9], PrintSize(get<9>(t)));
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
    Print(cout << " | " << setw(sizes[3]), get<3>(t));
    Print(cout << " | " << setw(sizes[4]), get<4>(t));
    Print(cout << " | " << setw(sizes[5]), get<5>(t));
    Print(cout << " | " << setw(sizes[6]), get<6>(t));
    Print(cout << " | " << setw(sizes[7]), get<7>(t));
    Print(cout << " | " << setw(sizes[8]), get<8>(t));
    Print(cout << " | " << setw(sizes[9]), get<9>(t));
    cout << endl;
  }
}

int main(int argc, char** argv) {
  vector<rapidjson::Document> documents;
  FROM(argc, argv, documents);
  vector<TupleType> tuples;
  runSelect(documents, tuples);
  printTuples(tuples);
}
