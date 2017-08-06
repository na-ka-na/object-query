/*
SELECT financial.quarterly_profits, all_employees.name, all_employees.active FROM Example1.Company ORDER BY all_employees.active, financial.quarterly_profits DESC, all_employees.id

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each each_active in all_employee.all_employees.active {
      tuples.add(all_employees.active)
      for each each_id in all_employee.all_employees.id {
        tuples.add(all_employees.id)
        for each each_name in all_employee.all_employees.name {
          tuples.add(all_employees.name)
          for each each_financial in document.financial {
            for each quarterly_profit in each_financial.financial.quarterly_profits {
              tuples.add(financial.quarterly_profits)
              tuples.record()
            } //quarterly_profit
          } //each_financial
        } //each_name
      } //each_id
    } //each_active
  } //all_employee
} //document
tuples.sortBy('all_employees.active', 'financial.quarterly_profits', 'all_employees.id')
tuples.print('financial.quarterly_profits', 'all_employees.name', 'all_employees.active')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;
using namespace oq;

vector<string> header = {
  "financial.quarterly_profits",
  "all_employees.name",
  "all_employees.active",
};

using S0 = optional<JsonValue>; /* all_employees.active */
using S1 = optional<JsonValue>; /* all_employees.id */
using S2 = optional<JsonValue>; /* all_employees.name */
using S3 = optional<JsonValue>; /* financial.quarterly_profits */
using TupleType = tuple<S3, S2, S0, S1>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* each_active : Iterators::mk_json_iterator("all_employees.active", all_employee, "active")) {
        S0 s0 = each_active ? *each_active : S0();
        for (const auto* each_id : Iterators::mk_json_iterator("all_employees.id", all_employee, "id")) {
          S1 s1 = each_id ? *each_id : S1();
          for (const auto* each_name : Iterators::mk_json_iterator("all_employees.name", all_employee, "name")) {
            S2 s2 = each_name ? *each_name : S2();
            for (const auto* each_financial : Iterators::mk_json_iterator("financial", document, "financial")) {
              for (const auto* quarterly_profit : Iterators::mk_json_iterator("financial.quarterly_profits", each_financial, "quarterly_profits")) {
                S3 s3 = quarterly_profit ? *quarterly_profit : S3();
                tuples.emplace_back(s3, s2, s0, s1);
              }
            }
          }
        }
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
  vector<rapidjson::Document> documents;
  FROM(argc, argv, documents);
  vector<TupleType> tuples;
  runSelect(documents, tuples);
  std::sort(tuples.begin(), tuples.end(), compareTuples);
  printTuples(tuples);
}
