# CppLinq
A simple linq library for C++.
## Usage
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
* range
* repeat
### Query
* concat
* select
* skip
* take
* where
* zip
### Aggregate
* aggregate
* all
* any
* average
* count
* sum
### ToContainer
* to_list
* to_map
* to_set
* to_vector
