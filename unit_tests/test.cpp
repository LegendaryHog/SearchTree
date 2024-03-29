#include <gtest/gtest.h>
#include <string>
#define DEBUG
#include "search_tree.hpp"
#include "boost_set.hpp"
#include "set.hpp"

using namespace Container;

TEST(Set, default_ctor)
{
    Set set0 {};
}

TEST(Set, insert)
{
    Set set {};
    for (auto i = 0; i < 40; i++)
    {
        if (i % 2)
            set.insert(i);
        else
            set.insert(i + 20);
    }

    Set<int> set1 {12, 1, 4, 5, -4, 8, 7, -9};
    Set<int> set2 {12, 1, 4, 5, -4, 8, 7, -9};
    std::array<int, 10> arr1 {2, 7, 12, 100, -12, 56, 10, 9, 11, 5};
    std::array<int, 15> res {-12, -9, -4, 1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 56, 100};

    for (auto x: arr1)
        set1.insert(x);
    
    set2.insert(arr1.cbegin(), arr1.cend());

    auto i = 0;
    for (auto x: set1)
        EXPECT_EQ(x, res[i++]);

    i = 0;
    for (auto x: set2)
        EXPECT_EQ(x, res[i++]);
}

TEST(Set, init_list_and_cont_ctor)
{
    Set<int> set0 {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    std::array<int, 14> arr {1, -4, -2, 8, 9, -20, 10, 34, 17, 1, 1, 9, 17, 21};
    Set set1 (arr.cbegin(), arr.cend());
    std::array<int, 10> res {-20, -4, -2, 1, 8, 9, 10, 17, 21, 34};

    auto i = 0;
    for (auto x: set0)
        EXPECT_EQ(x, res[i++]);

    i = 0;
    for (auto x: set1)
        EXPECT_EQ(x, res[i++]);

}

TEST(Set, Iterators)
{
    static_assert(std::bidirectional_iterator<Set<>::Iterator>);
    static_assert(std::bidirectional_iterator<Set<>::ConstIterator>);

    std::array<int, 8> arr {-2, 1, 2, 4, 5, 6, 7, 9};

    Set<int> set1 {1, 2, 4, 7, 9, -2, 5, 6};
    std::size_t i = 0;
    for (auto x: set1)
        EXPECT_EQ(x, arr[i++]);

    const Set<int> cset {1, 2, 4, 7, 9, -2, 5, 6};
    i = 0;
    for (auto x: cset)
        EXPECT_EQ(x, arr[i++]);

    Set<int> set {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    EXPECT_EQ(11, *std::prev(set.end()));
    EXPECT_EQ(11, *std::prev(set.cend()));

    auto itr  = set.begin();
    auto citr = set.cbegin();
    EXPECT_EQ(*itr++, 0);
    EXPECT_EQ(*citr++, 0);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*++itr, 2);
    EXPECT_EQ(*++citr, 2);
    EXPECT_EQ(*itr--, 2);
    EXPECT_EQ(*citr--, 2);
    EXPECT_EQ(*itr, 1);
    EXPECT_EQ(*citr, 1);
    EXPECT_EQ(*--itr, 0);
    EXPECT_EQ(*--citr, 0);

    EXPECT_EQ(std::next(set.begin(), 5), std::prev(set.end(), 7));
    EXPECT_EQ(std::distance(set.begin(), set.end()), set.size());
}

TEST(Set, equal_to)
{
    Set<int> set1 {12, 3, 5, -7, 8, 12, 5, 6, -9, -7, 21, 21, 22, 345, 7, 3, 4};
    Set<int> set2 {22, 21, 12, 21, 5, 3, 3, 3, 8, 7, 8, 6, -7, 3, 12, -9, 4, 345};

    EXPECT_EQ(set1, set2);
    EXPECT_EQ(set2, set1);

    Set set3 {};
    Set set4 {};

    EXPECT_EQ(set3, set4);
    EXPECT_EQ(set4, set3);
}

TEST(Set, big_five)
{
    Set<int> set {1, 2, 3, 4, -10, 5, 8, 9, 11};
    
    // copy ctor 
    Set set1 {set};
    EXPECT_EQ(set1, set);

    // move ctor
    Set set2 {std::move(set1)};
    EXPECT_EQ(set2, set);

    // move assign
    Set set3 {};
    EXPECT_EQ(set3.size(), 0);
    set3 = std::move(set2);
    EXPECT_EQ(set3, set);

    //copy assign
    Set<int> set4 {1, 2, 3};
    EXPECT_EQ(set4.size(), 3);
    set4 = set;
    EXPECT_EQ(set4, set);
}

TEST(Set, erase)
{
    Set<int> set {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

    auto er1 = set.erase(1);
    EXPECT_EQ(*er1, 2);
    EXPECT_EQ(*set.erase(er1), 3);

    auto citr = set.cbegin();
    EXPECT_EQ(*set.erase(citr), 3);

    Set<int> set1 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto itr1 = std::next(set1.begin(), 6);
    auto itr2 = std::next(set1.begin(), 12);

    EXPECT_EQ(*set1.erase(itr1, itr2), 12);

    Set<int> set2 {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    auto citr1 = std::next(set2.cbegin(), 6);
    auto citr2 = std::next(set2.cbegin(), 12);

    EXPECT_EQ(*set2.erase(citr1, citr2), 12);
}

TEST(Set, bounds)
{
    Set<int> set {-5, -4, -3, 6, 8, 9, 10, 11, 15, 17};

    EXPECT_EQ(*set.upper_bound(-4), -3);
    EXPECT_EQ(*set.upper_bound(6), 8);
    EXPECT_EQ(*set.upper_bound(7), 8);
    EXPECT_EQ(*set.upper_bound(13), 15);
    EXPECT_EQ(*set.upper_bound(15), 17);

    EXPECT_EQ(*set.lower_bound(-5), -5);
    EXPECT_EQ(*set.lower_bound(0), 6);
    EXPECT_EQ(*set.lower_bound(-4), -4);
    EXPECT_EQ(*set.lower_bound(6), 6);
    EXPECT_EQ(*set.lower_bound(7), 8);
}

class StandUser
{
    std::string name_, stand_name_;
    unsigned strength_, speed_, range_;
public:
    StandUser() = default;

    StandUser(const std::string& n, const std::string& sn, unsigned st, unsigned sp, unsigned r)
    :name_ {n}, stand_name_ {sn}, strength_ {st}, speed_ {sp}, range_ {r}
    {}

    StandUser(std::string&& n, std::string&& sn, unsigned st, unsigned sp, unsigned r)
    :name_ {std::move(n)}, stand_name_ {std::move(sn)}, strength_ {st}, speed_ {sp}, range_ {r}
    {}

    const std::string& name()       const {return name_;}
    const std::string& stand_name() const {return stand_name_;}
    unsigned strength() const {return strength_;}
    unsigned speed()    const {return speed_;}
    unsigned range()    const {return range_;}

    unsigned power() const
    {
        return strength_ * strength_ + speed_ * speed_ + range_ * range_;
    }
};

bool operator<(const StandUser& lhs, const StandUser& rhs)
{
    auto lhs_power = lhs.power(), rhs_power = rhs.power();
    if (lhs_power < rhs_power)
        return true;
    else if (lhs_power > rhs_power)
        return false;
    else if (lhs.strength() < rhs.strength())
        return true;
    else if (lhs.strength() > rhs.strength())
        return false;
    else if (lhs.range() < rhs.range())
        return true;
    else if (lhs.range() > rhs.range())
        return false;
    else
        return lhs.speed() < rhs.speed();
}

TEST(Set, struct_Key)
{
    Set<StandUser> set {
        {"Jotaro Kujo(Jostar)",        "Star Platinum",           100, 100, 20},
        {"Joseph Jostar",              "Hermit Purple",           48,  36,  30},
        {"Muhammad Avdol",             "Magician`s Red",          90,  47,  50},
        {"Jean Pierre Polhareff",      "Silver Chariot",          80,  89,  30},
        {"Noriaki Kakyon",             "Hierophant Green",        92,  68,  70},
        {"Iggy",                       "The Fool",                93,  72,  20},
        {"Dio Brando",                 "The World",               98,  100, 20},
        
        {"Giorno Giovanna(Brando)",    "Gold Experience Requiem", 150, 100, 20},
        {"Bruno Bucciaratti",          "Sticky Fingers",          82,  93,  25},
        {"Leone Abbachio",             "Moody Blues",             30,  75,  100},
        {"Guido Mista",                "Sex Pistols",             86,  50,  80},
        {"Narancia Ghirga",            "Aerosmith",               34,  90,  93},
        {"Pannacota Fugo",             "Purple Haze",             95,  95,  25},
        {"Trish Una",                  "Spice Girl",              78,  70,  30},
        {"Diavolo Una",                "King Crimson",            120, 89,  100},

        {"Josuke Higashikata(Jostar)", "Crazy Diamond",           99,  100, 20},
        {"Koichi Hirose",              "Echoes (Act 3)",          90,  75,  50},
        {"Okuyasu Nijimura",           "The Hand",                98,  69,  25},
        {"Yoshikage Kira",             "Killer Queen",            110, 78,  30},
    };

    EXPECT_EQ(set.find({"", "", 99, 100, 20})->name(), "Josuke Higashikata(Jostar)");
    EXPECT_EQ(set.maximum().name(), "Giorno Giovanna(Brando)");
}

TEST(BoostSet, big_five_and_insert)
{
    BoostSet<int> set {1, 2, 3, 4, 5, 6, 7, 8, 12, 34, -7, -9, 15, 3};
    set.insert(9);
    set.insert(13);
    set.insert(23);
    std::array<int, 16> arr = {-9, -7, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 15, 23, 34};
    auto i = 0;
    for (auto x: set)
        EXPECT_EQ(x, arr[i++]);
    
    // copy ctor
    BoostSet set_cpy {set};
    EXPECT_EQ(set_cpy, set);

    BoostSet<int> set_cpy1 {1, 2, 3};
    set_cpy1 = set;
    EXPECT_EQ(set_cpy1, set);

    // move ctor
    BoostSet set_move {std::move(set_cpy)};
    EXPECT_EQ(set_move, set);

    // move assign
    BoostSet<int> set_move1 {1, 2, 3};
    set_move1 = std::move(set_move);
    EXPECT_EQ(set_move1, set);
}

TEST(BoostSet, kth_smallest)
{
    BoostSet<int> set {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    set.insert(-1);
    set.insert(13);
    set.insert(0);

    std::array<int, 15> arr = {-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (auto i = 0; i < 15; i++)
        EXPECT_EQ(set[i], arr[i]);
}

TEST(BoostSet, number_less_than)
{
    BoostSet<int> set = {0, 1, 2, 3, 7, 9, 11, 15, 20, 21, 56, 70};
    EXPECT_EQ(set.number_less_than(0), 0);
    EXPECT_EQ(set.number_less_than(-1), 0);
    EXPECT_EQ(set.number_less_than(1), 1);
    EXPECT_EQ(set.number_less_than(2), 2);
    EXPECT_EQ(set.number_less_than(3), 3);
    EXPECT_EQ(set.number_less_than(4), 4);
    EXPECT_EQ(set.number_less_than(5), 4);
    EXPECT_EQ(set.number_less_than(6), 4);
    EXPECT_EQ(set.number_less_than(7), 4);
    EXPECT_EQ(set.number_less_than(8), 5);
    EXPECT_EQ(set.number_less_than(9), 5);
    EXPECT_EQ(set.number_less_than(10), 6);
    EXPECT_EQ(set.number_less_than(11), 6);
    EXPECT_EQ(set.number_less_than(12), 7);
    EXPECT_EQ(set.number_less_than(13), 7);
    EXPECT_EQ(set.number_less_than(14), 7);
    EXPECT_EQ(set.number_less_than(15), 7);
    EXPECT_EQ(set.number_less_than(16), 8);
    EXPECT_EQ(set.number_less_than(19), 8);
    EXPECT_EQ(set.number_less_than(20), 8);
    EXPECT_EQ(set.number_less_than(21), 9);
    EXPECT_EQ(set.number_less_than(22), 10);
    EXPECT_EQ(set.number_less_than(56), 10);
    EXPECT_EQ(set.number_less_than(57), 11);
    EXPECT_EQ(set.number_less_than(69), 11);
    EXPECT_EQ(set.number_less_than(70), 11);
    EXPECT_EQ(set.number_less_than(71), 12);
    EXPECT_EQ(set.number_less_than(100), 12);
    EXPECT_EQ(set.number_less_than(150), 12);
    EXPECT_EQ(set.number_less_than(420), 12);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}    
