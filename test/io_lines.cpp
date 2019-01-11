#include <iostream>
#include <linq/string.hpp>

using namespace std;
using namespace linq;

int main()
{
    string str{ R"(Twinkle, twinkle, little star,
How I wonder what you are.
Up above the world so high,
Like a diamond in the sky.
)" };
    istringstream iss{ str };
    ostringstream oss;
    write_lines(oss, read_lines(iss));
    if (oss.str() == str)
    {
        cout << "Success." << endl;
    }
    return 0;
}
