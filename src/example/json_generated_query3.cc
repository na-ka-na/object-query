/*
SELECT all_employees.id, (financial.quarterly_revenues/all_employees.active_direct_reports) FROM Example1.Company WHERE ((all_employees.active_direct_reports > 0) AND ((financial.quarterly_revenues*2) > 1))

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each active_direct_report in all_employee.all_employees.active_direct_reports {
      if (!(all_employees.active_direct_reports > 0)) { continue; }
      for each each_id in all_employee.all_employees.id {
        tuples.add(all_employees.id)
        for each each_financial in document.financial {
          for each quarterly_revenue in each_financial.financial.quarterly_revenues {
            if (!((financial.quarterly_revenues*2) > 1)) { continue; }
            tuples.add((financial.quarterly_revenues/all_employees.active_direct_reports))
            tuples.record()
          } //quarterly_revenue
        } //each_financial
      } //each_id
    } //active_direct_report
  } //all_employee
} //document
tuples.print('all_employees.id', '(financial.quarterly_revenues/all_employees.active_direct_reports)')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;

vector<string> header = {
  "all_employees.id",
  "(financial.quarterly_revenues/all_employees.active_direct_reports)",
};

auto $c1 = 0;
auto $c3 = 1;
auto $c2 = 2;

using S0 = optional<JsonValue>; /* all_employees.active_direct_reports */
using S1 = optional<JsonValue>; /* all_employees.id */
using S2 = optional<JsonValue>; /* financial.quarterly_revenues */
using S3 = decltype(Divide(S2(), S0())); /* (financial.quarterly_revenues/all_employees.active_direct_reports) */
using TupleType = tuple<S1, S3>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* active_direct_report : Iterators::mk_json_iterator("all_employees.active_direct_reports", all_employee, "active_direct_reports")) {
        S0 s0 = active_direct_report ? *active_direct_report : S0();
        if (!Gt(s0, &$c1)) { continue; }
        for (const auto* each_id : Iterators::mk_json_iterator("all_employees.id", all_employee, "id")) {
          S1 s1 = each_id ? *each_id : S1();
          for (const auto* each_financial : Iterators::mk_json_iterator("financial", document, "financial")) {
            for (const auto* quarterly_revenue : Iterators::mk_json_iterator("financial.quarterly_revenues", each_financial, "quarterly_revenues")) {
              S2 s2 = quarterly_revenue ? *quarterly_revenue : S2();
              if (!Gt(Mult(s2, &$c2), &$c3)) { continue; }
              S3 s3 = Divide(s2, s0);
              tuples.emplace_back(s1, s3);
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
  vector<rapidjson::Document> documents;
  FROM(argc, argv, documents);
  vector<TupleType> tuples;
  runSelect(documents, tuples);
  printTuples(tuples);
}
