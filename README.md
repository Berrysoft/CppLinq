# CppLinq
A simple LINQ library for C++.
## Usage
To find the square root of the non-negative even numbers less than 10:
``` c++
using namespace std;
using namespace linq;

range(0, 10) >>
	where([](int a) { return a % 2 == 0; }) >>
	select([](int a) { return sqrt(a); }) >>
	for_each([](double a) { cout << a << endl; });
```
Here we use `operator>>` to make it extensible.
For extension sample, see [extension_test.cpp](test/extension_test.cpp).
## Supported methods
### Core
* append
* concat
* insert
* prepend
* range
* repeat
### Query
* select
* select_many
* skip
* skip_while
* take
* take_while
* where
* zip
### Aggregate
* aggregate
* all
* any
* average
* back
* contains
* count
* default_if_empty
* distinct
* empty
* except
* for_each
* front
* get_at
* group
* group_join
* index_of
* intersect
* join
* limit
* max
* min
* reverse
* single
* sort
* sum
* union_set
### ToContainer
* to_deque
* to_list
* to_map
* to_multimap
* to_multiset
* to_set
* to_unordered_map
* to_unordered_multimap
* to_unordered_multiset
* to_unordered_set
* to_vector
### String
* ends_with
* instr
* joinstr
* read_lines
* remove
* replace
* split
* starts_with
* trim
* trim_right
* trim_left
* write_lines
