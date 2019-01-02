# CppLinq
A simple linq library for C++.
## Usage
``` c++
using namespace std;
using namespace linq;

for (auto i : range(0, 10)
                    .where([](int a) { return a % 2 == 0; })
                    .select([](int a) { return sqrt(a); }))
{
    cout << i << endl;
}
```
