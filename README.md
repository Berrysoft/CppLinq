# CppLinq
A simple linq library for C++.
## Usage
To find the square root of the non-negative even numbers less than 10:
``` c++
using namespace std;
using namespace linq;

for (auto i : range(0, 10)
                    >> where([](int a) { return a % 2 == 0; })
                    >> select([](int a) { return sqrt(a); }))
{
    cout << i << endl;
}
```
Here we use `operator>>` to make it extensible.
For extension sample, see [extension.cpp](test/extension.cpp).
## Supported methods
### Core
* append
* equals
* prepend
* range
* repeat
### Query
* cast
* concat
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
### Aggregate
* aggregate
* all
* any
* average
* back
* count
* default_if_empty
* distinct
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
* write_lines
