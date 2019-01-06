#include <iostream>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

int main()
{
    string str{ "Hello world !" };
    string_view views[]{ "Hello", "world", "!" };
    auto e{ str >> split(' ') };
    if (e >> equals(views))
    {
        cout << "Success." << endl;
    }
    return 0;
}
