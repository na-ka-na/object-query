# Protobuf Query

## Introduction

This project allows one to run SQL queries over protocol buffers. Right now protobuf 2.x is supported.

## Features
- All proto scalar and compound types are supported except maps.

- Querying a field at any nested level is supported.

- Querying across any number of repeated fields is supported.

- Selecting multiple repeated fields will result in a cartesian product across them.

- For non repeated fields, one may query `has_xxx`. For repeated fields, one may query `xxx_size`.

- Enums are printed as name and may be filtered by name in where clause.

- No assumption on the presence of values is made, missing values come as `NULL`.

#### SQL
- Following SQL statements are supported right now: `SELECT`, `FROM`, `WHERE`, `ORDER BY`.

- Following SQL keywords are supported right now: `AS`, `LIKE`, `IS`, `NOT`, `NULL`, `AND`, `OR`, `ASC`, `DESC`,
  `TRUE`, `FALSE`, `,`, `+`, `-`, `*`, `/`, `=`, `!=`, `<`, `<=`, `>`, `>=`, `(`, `)`

- `SELECT *` or `SELECT a.b.*` are allowed. This will select all primitive fields in that message.

- `WHERE` clause allows full expression support.

- Arithmetic operators behave per C++14 standard.

- String concatenation is supported with `+`.

- `LIKE` is different: it takes [default C++](http://en.cppreference.com/w/cpp/regex/ecmascript) regular expressions instead of supporting SQL standard.

- Following custom SQL functions are supported right now: `STR`, `INT` for primitives. You may define
  your own overrides for these functions, for e.g `STR` may be overriden for message types:

        string STR(const X& x);
        message X {
        }
        message Z {
          X x;
          int y;
        }
        SELECT STR(x), 'y='+STR(y) FROM Z;

- Additionally you may define any custom functions (must return primitives or string):

        string foo(const X& x);
        string bar(int y);
        double qux(const X& x, int y);

        SELECT foo(x), bar(y), qux(x,y) FROM Z;

- Alias support with `AS`:
    
        SELECT y % 10 AS y10 FROM Z WHERE y10=3 ORDER BY y10

## Examples

Example: person.proto:

    package com.ka;
    
    message Persons {
        repeated Person persons = 1;
    }
    message Person {
        optional string name = 1;
        optional string ssn = 2;
        repeated string emails = 3;
        repeated Address addresses = 4;
    }
    message Address {
        optional uint32 zip = 1;
    }

With the following data:

    {
        persons: [
            {name: "p1", emails: [], addresses: []},
            {name: "p2", emails: ["e2"], addresses: []},
            {name: "p3", ssn: "", emails: [], addresses: [{zip: 123}]},
            {name: "p4", ssn: "ssn4", emails: ["e4"], addresses: [{zip: 123}]},
            {name: "p5", ssn: "ssn5", emails: ["e5", "e55"], addresses: [{zip: 123}, {zip: 456}]},
        ],
    }

We can run SQL queries:

    SELECT persons.name FROM com.ka.Persons
    persons.name
    ------------
    p1
    p2
    p3
    p4
    p5

    SELECT persons.* FROM com.ka.Persons
    persons.name | persons.ssn
    ------------ | --------------
    p1           | NULL
    p2           | NULL
    p3           | 
    p4           | ssn4
    p5           | ssn5

    SELECT persons.name, persons.emails FROM com.ka.Persons
    persons.name | persons.emails
    ------------ | --------------
    p1           | NULL
    p2           | e2
    p3           | NULL
    p4           | e4
    p5           | e5
    p5           | e55

    SELECT persons.name, persons.emails, persons.addresses.zip FROM com.ka.Persons
    persons.name | persons.emails | persons.addresses.zip
    ------------ | -------------- | ---------------------
    p1           | NULL           | NULL
    p2           | e2             | NULL
    p3           | NULL           | 123
    p4           | e4             | 123
    p5           | e5             | 123
    p5           | e5             | 456
    p5           | e55            | 123
    p5           | e55            | 456

    SELECT persons.name, persons.emails FROM com.ka.Persons WHERE persons.emails LIKE '^.*[1-4]$'
    persons.name | persons.emails
    ------------ | --------------
    p2           | e2
    p4           | e4

    SELECT persons.name, persons.emails FROM com.ka.Persons WHERE persons.emails LIKE '^.*[1-4]$' OR persons.name = 'p1'
    persons.name | persons.emails
    ------------ | --------------
    p1           | NULL
    p2           | e2
    p4           | e4

    SELECT persons.name FROM com.ka.Persons WHERE persons.emails IS NULL
    persons.name
    ------------
    p1
    p3

    SELECT persons.name, persons.has_ssn, persons.emails_size FROM com.ka.Persons ORDER BY persons.emails_size DESC
    persons.name | persons.has_ssn | persons.emails_size
    ------------ | --------------- | -------------------
    p5           | true            | 2
    p2           | false           | 1
    p4           | true            | 1
    p1           | false           | 0
    p3           | true            | 0


## Usage

#### Pre requisites
- C++ compiler with C++14 support. Runs on Mac OS X El Capitan.
- Install protobuf 2.x
- Install gflags


#### Building
- Clone the git repo
    
        cd object-query; mkdir build; cd build; cmake ../src; cd protobuf-query/main; make


#### Running
- Find usage

        $ ./RunQuery
        RunQuery: <required-flags> [optional-flags] <sql-query> [sql-args...]
        ...

- Minimal usage
        
        $ ./RunQuery \
            --cppProtoHeader=/path/to/persons.pb.h \
            --cppProtoLib=/path/to/persons.so \
            "SELECT persons.name FROM com.ka.Persons" \
            /path/to/proto.bin

- A special C++ macro `FROM` reads the proto from the binary file provided after the SQL query.
  It may be overriden by employing `--cppExtraIncludes` (see below for all available flags).
  Any extra includes are included before the `FROM` macro which is wrapped around `ifndef`.
  `FROM` macro should take `argc`, `argv`, `vector<Proto>&` as parameters and read in the protos
  in the provided vector reference.

#### Under the hood
RunQuery is performing the following steps
* Step (1) Loads the proto lib to understand the structure of proto (using proto reflection).
* Step (2) Parses the SQL query and generated C++14 source code for it.
* Step (3) Compiles the generated code with the `--cppProtoHeader` and links it with `--cppProtoLib`.
* Step (4) Invokes the generated binary to actually run the query.
* Step (5) The generated binary reads in the protos using the `FROM` macro.
* Step (6) The actual query is executed against the protos.

#### Required flags
* `--cppProtoHeader`: path/to/compiled/proto/header.pb.h. See src/protobuf-query/example/CMakeLists.txt
  for an example of how proto is compiled, you may add your proto there to build it.
* `--cppProtoLib`: path/to/built/proto/library.so which contains the proto defintion, either .so or .dylib.

#### Optional flags
* `--codeGenDir`: directory where generated code files are emitted (default '.').
* `--codeCompileDir`: directory where compiled binary is emitted (default '.').
* `--codeGenPrefix`: prefix for generated code files and binary (default 'generated_query').
* `--cppExtraIncludes`: comma separated extra headers to include in generated code,
                        e.g. with `FROM` macro overriden, or custom function definitions (default '').
* `--cppExtraLinkLibs`: comma separated extra lib names to link with generated code,
                        used for the `-l` argument (default '').
* `--cppExtraIncludeDirs`: comma separated directories to satisfy `--cppExtraIncludes`,
                           used for the `-I` argument (default '').
* `--cppExtraLinkLibDirs`: comma separated directories to satisfy `--cppExtraLinkLibs`,
                           used for the `-L` argument (default '').
