#pragma once
#include "test_utility.hpp"

class aggregate_test : public bstest::test_base
{
public:
    aggregate_test()
    {
        add_test(count_test);
        add_test(any_all_test);
        add_test(aggregate_method_test);
        add_test(reverse_test);
        add_test(sort_test);
        add_test(sort_custom_test);
        add_test(sorter_test);
        add_test(min_max_test);
        add_test(for_each_test);
        add_test(for_each_index_test);
        add_test(peek_test);
        add_test(peek_index_test);
    }

private:
    void count_test();
    void any_all_test();
    void aggregate_method_test();
    void reverse_test();
    void sort_test();
    void sort_custom_test();
    void sorter_test();
    void min_max_test();
    void for_each_test();
    void for_each_index_test();
    void peek_test();
    void peek_index_test();

public:
    struct pack
    {
        std::string name;
        int score;
    };
};

inline std::ostream& operator<<(std::ostream& stream, const aggregate_test::pack& p)
{
    return stream << '(' << p.name << ", " << p.score << ')';
}

inline bool operator==(const aggregate_test::pack& p1, const aggregate_test::pack& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const aggregate_test::pack& p1, const aggregate_test::pack& p2) { return !(p1 == p2); }

class find_test : public bstest::test_base
{
public:
    find_test()
    {
        add_test(get_at_test);
        add_test(index_of_test);
        add_test(front_back_test);
        add_test(default_test);
        add_test(single_test);
    }

private:
    void get_at_test();
    void index_of_test();
    void front_back_test();
    void default_test();
    void single_test();
};

class set_test : public bstest::test_base
{
public:
    set_test()
    {
        add_test(distinct_test);
        add_test(union_set_test);
        add_test(intersect_test);
        add_test(except_test);
    }

private:
    void distinct_test();
    void union_set_test();
    void intersect_test();
    void except_test();
};

class group_test : public bstest::test_base
{
public:
    group_test()
    {
        add_test(group_method_test);
        add_test(group_join_test);
        add_test(join_test);
    }

private:
    void group_method_test();
    void group_join_test();
    void join_test();

public:
    struct pack
    {
        int index;
        int score;
    };
    struct pack2
    {
        int index;
        std::string name;
    };
    struct pack3
    {
        std::string name;
        int score;
    };
};

inline std::ostream& operator<<(std::ostream& stream, const group_test::pack& p)
{
    return stream << '(' << p.index << ", " << p.score << ')';
}

inline bool operator==(const group_test::pack& p1, const group_test::pack& p2)
{
    return p1.index == p2.index && p1.score == p2.score;
}
inline bool operator!=(const group_test::pack& p1, const group_test::pack& p2) { return !(p1 == p2); }

inline std::ostream& operator<<(std::ostream& stream, const group_test::pack3& p)
{
    return stream << '(' << p.name << ", " << p.score << ')';
}

inline bool operator==(const group_test::pack3& p1, const group_test::pack3& p2)
{
    return p1.name == p2.name && p1.score == p2.score;
}
inline bool operator!=(const group_test::pack3& p1, const group_test::pack3& p2) { return !(p1 == p2); }

class to_container_test : public bstest::test_base
{
public:
    to_container_test()
    {
        add_test(to_list_test);
        add_test(to_set_test);
        add_test(to_multiset_test);
        add_test(to_vector_test);
        add_test(to_map_test);
        add_test(to_multimap_test);
    }

private:
    void to_list_test();
    void to_set_test();
    void to_multiset_test();
    void to_vector_test();
    void to_map_test();
    void to_multimap_test();

public:
    struct pack
    {
        int index;
        int score;
    };
};
