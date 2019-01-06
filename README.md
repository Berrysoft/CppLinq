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
* count
* distinct
* except
* get_at
* intersect
* limit
* reverse
* sort
* sum
* union_set
### ToContainer
* to_list
* to_map
* to_multimap
* to_set
* to_vector
### String
* instr
* join
* split
