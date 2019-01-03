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
* all
* any
* concat
* count
* range
* select
* skip
* take
* to_list
* to_set
* to_vector
* where
