/*
SELECT all_employees.name, all_employees.active_direct_reports FROM Example1.Company WHERE all_employees.active_direct_reports IS NOT NULL

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each active_direct_report in all_employee.all_employees.active_direct_reports {
      if (!all_employees.active_direct_reports IS NOT NULL) { continue; }
      tuples.add(all_employees.active_direct_reports)
      for each each_name in all_employee.all_employees.name {
        tuples.add(all_employees.name)
        tuples.record()
      } //each_name
    } //active_direct_report
  } //all_employee
} //document
tuples.print('all_employees.name', 'all_employees.active_direct_reports')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;

vector<string> header = {
  "all_employees.name",
  "all_employees.active_direct_reports",
};

using S0 = optional<JsonValue>; /* all_employees.active_direct_reports */
using S1 = optional<JsonValue>; /* all_employees.name */
using TupleType = tuple<S1, S0>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* active_direct_report : Iterators::mk_json_iterator("all_employees.active_direct_reports", all_employee, "active_direct_reports")) {
        S0 s0 = active_direct_report ? *active_direct_report : S0();
        if (!IsNotNull(s0)) { continue; }
        for (const auto* each_name : Iterators::mk_json_iterator("all_employees.name", all_employee, "name")) {
          S1 s1 = each_name ? *each_name : S1();
          tuples.emplace_back(s1, s0);
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
