/*
SELECT (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') AS employee, ((((STR(all_employees.id)+' ')+employee)+' ')+STR(founded)) AS employee2 FROM Example1.Company WHERE (employee2 LIKE '.*true.*')

with (document = parseFromFile()) {
  for each all_employee in document.all_employees {
    for each each_active in all_employee.all_employees.active {
      for each each_id in all_employee.all_employees.id {
        for each each_name in all_employee.all_employees.name {
          tuples.add((((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))
          for each each_founded in document.founded {
            if (!(((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)) LIKE '.*true.*')) { continue; }
            tuples.add(((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)))
            tuples.record()
          } //each_founded
        } //each_name
      } //each_id
    } //each_active
  } //all_employee
} //document
tuples.print('(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')')', '((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded))')
*/
#include "json_example_utils.h"
#include "json_generated_common.h"

using namespace std;

vector<string> header = {
  "employee",
  "employee2",
};

string $c4 = " ";
string $c2 = " (";
string $c3 = ")";
regex $c5 = regex(".*true.*", regex::optimize);
string $c1 = ": ";

template<typename Arg0, typename Ret=decltype(STR(Arg0()))>
optional<Ret> $STR(const optional<Arg0>& arg0) {
  if (arg0) {
    return optional<Ret>(STR(*arg0));
  } else {
    return optional<Ret>();
  }
}

using S0 = optional<JsonValue>; /* all_employees.active */
using S1 = optional<JsonValue>; /* all_employees.id */
using S2 = optional<JsonValue>; /* all_employees.name */
using S3 = optional<JsonValue>; /* founded */
using S4 = decltype(Plus(Plus(Plus(Plus(Plus($STR(S1()), &$c1), S2()), &$c2), $STR(S0())), &$c3)); /* (((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')') */
using S5 = decltype(Plus(Plus(Plus(Plus($STR(S1()), &$c4), Plus(Plus(Plus(Plus(Plus($STR(S1()), &$c1), S2()), &$c2), $STR(S0())), &$c3)), &$c4), $STR(S3()))); /* ((((STR(all_employees.id)+' ')+(((((STR(all_employees.id)+': ')+all_employees.name)+' (')+STR(all_employees.active))+')'))+' ')+STR(founded)) */
using TupleType = tuple<S4, S5>;

void runSelect(const vector<rapidjson::Document>& documents, vector<TupleType>& tuples) {
  for (const auto* document : Iterators::mk_json_iterator(documents)) {
    for (const auto* all_employee : Iterators::mk_json_iterator("all_employees", document, "all_employees")) {
      for (const auto* each_active : Iterators::mk_json_iterator("all_employees.active", all_employee, "active")) {
        S0 s0 = each_active ? *each_active : S0();
        for (const auto* each_id : Iterators::mk_json_iterator("all_employees.id", all_employee, "id")) {
          S1 s1 = each_id ? *each_id : S1();
          for (const auto* each_name : Iterators::mk_json_iterator("all_employees.name", all_employee, "name")) {
            S2 s2 = each_name ? *each_name : S2();
            S4 s4 = Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3);
            for (const auto* each_founded : Iterators::mk_json_iterator("founded", document, "founded")) {
              S3 s3 = each_founded ? *each_founded : S3();
              if (!Like(Plus(Plus(Plus(Plus($STR(s1), &$c4), Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3)), &$c4), $STR(s3)), $c5)) { continue; }
              S5 s5 = Plus(Plus(Plus(Plus($STR(s1), &$c4), Plus(Plus(Plus(Plus(Plus($STR(s1), &$c1), s2), &$c2), $STR(s0)), &$c3)), &$c4), $STR(s3));
              tuples.emplace_back(s4, s5);
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
