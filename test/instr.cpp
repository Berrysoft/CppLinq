#include <iostream>
#include <linq/aggregate.hpp>
#include <linq/string.hpp>
#include <string>

using namespace std;
using namespace linq;

bool instr_test()
{
    string str{ "Hello world !" };
    return str >> instr('o');
}

bool instr_str_test()
{
    string str{ "Hello world !" };
    return str >> instr<char>("world");
}

int main()
{
    if (instr_test() && instr_str_test())
    {
        cout << "Success." << endl;
    }
    return 0;
}
