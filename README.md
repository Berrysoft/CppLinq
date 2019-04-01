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
* equals
* equals_weak
* insert
* prepend
* range
* repeat
### Query
* cast
* select
* select_index
* select_many
* select_many_index
* skip
* skip_while
* skip_while_index
* take
* take_while
* take_while_index
* where
* where_index
* zip
* zip_index
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
* for_each_index
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
* peek
* peek_index
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
* to_set
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
