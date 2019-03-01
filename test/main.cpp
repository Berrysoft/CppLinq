#include "aggregate_test.hpp"
#include "core_test.hpp"
#include "query_test.hpp"
#include "string_test.hpp"
#include "winrt_test.hpp"

using namespace sf;
using namespace bstest;

int main()
{
    test_manager manager{};

    manager.add<enumerable_test>();
    manager.add<construct_test>();
    manager.add<calculate_test>();
    manager.add<extension_test>();

    manager.add<where_select_test>();
    manager.add<skip_take_test>();
    manager.add<linear_test>();

    manager.add<aggregate_test>();
    manager.add<find_test>();
    manager.add<set_test>();
    manager.add<group_test>();
    manager.add<to_container_test>();

    manager.add<string_test>();

#ifdef LINQ_USE_WINRT
    manager.add<winrt_test>();
#endif

    auto vec{ manager.run() };
    for (auto& e : vec)
    {
        println(e.what());
        println();
    }
    if (vec.empty())
    {
        println("Test success.");
    }
}
