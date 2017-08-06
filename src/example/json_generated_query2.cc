/*
SELECT all_employees.id, all_employees.name FROM Example1.Company WHERE ((all_employees.id IS NOT NULL AND (((all_employees.name = 'def') AND (all_employees.active = TRUE)) OR (all_employees.name = 'abc'))) AND (financial.quarterly_profits > 0))

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each each_active in all_employee.all_employees.active {
      for each each_id in all_employee.all_employees.id {
        if (!all_employees.id IS NOT NULL) { continue; }
        tuples.add(all_employees.id)
        for each each_name in all_employee.all_employees.name {
          if (!(((all_employees.name = 'def') AND (all_employees.active = TRUE)) OR (all_employees.name = 'abc'))) { continue; }
          tuples.add(all_employees.name)
          for each each_financial in document.financial {
            for each quarterly_profit in each_financial.financial.quarterly_profits {
              if (!(financial.quarterly_profits > 0)) { continue; }
              tuples.record()
            } //quarterly_profit
          } //each_financial
        } //each_name
      } //each_id
    } //each_active
  } //all_employee
} //document
tuples.print('all_employees.id', 'all_employees.name')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;
using namespace oq;

vector<string> header = {
  "all_employees.id",
  "all_employees.name",
};

string $c3 = "abc";
string $c1 = "def";
auto $c4 = 0;
auto $c2 = true;

using S0 = optional<JsonValue>; /* all_employees.active */
using S1 = optional<JsonValue>; /* all_employees.id */
using S2 = optional<JsonValue>; /* all_employees.name */
using S3 = optional<JsonValue>; /* financial.quarterly_profits */
using TupleType = tuple<S1, S2>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* each_active : Iterators::mk_json_iterator("all_employees.active", all_employee, "active")) {
        S0 s0 = each_active ? *each_active : S0();
        for (const auto* each_id : Iterators::mk_json_iterator("all_employees.id", all_employee, "id")) {
          S1 s1 = each_id ? *each_id : S1();
          if (!IsNotNull(s1)) { continue; }
          for (const auto* each_name : Iterators::mk_json_iterator("all_employees.name", all_employee, "name")) {
            S2 s2 = each_name ? *each_name : S2();
            if (!((Eq(s2, &$c1) && Eq(s0, &$c2)) || Eq(s2, &$c3))) { continue; }
            for (const auto* each_financial : Iterators::mk_json_iterator("financial", document, "financial")) {
              for (const auto* quarterly_profit : Iterators::mk_json_iterator("financial.quarterly_profits", each_financial, "quarterly_profits")) {
                S3 s3 = quarterly_profit ? *quarterly_profit : S3();
                if (!Gt(s3, &$c4)) { continue; }
                tuples.emplace_back(s1, s2);
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
