#include <gtest/gtest.h>
#include "trlc/enum.hpp"
#include <algorithm>
#include <vector>
#include <iterator>

namespace {

TRLC_ENUM(Fruit, Apple, Banana, Cherry, Date, Elderberry);

TEST(EnumIterationTest, RangeBasedFor) {
    std::vector<std::string_view> names;
    
    for (auto fruit : Fruit::all()) {
        names.push_back(fruit.name());
    }
    
    ASSERT_EQ(names.size(), 5);
    EXPECT_EQ(names[0], "Apple");
    EXPECT_EQ(names[1], "Banana");
    EXPECT_EQ(names[2], "Cherry");
    EXPECT_EQ(names[3], "Date");
    EXPECT_EQ(names[4], "Elderberry");
}

TEST(EnumIterationTest, ExplicitIterators) {
    auto it = Fruit::begin();
    auto end = Fruit::end();
    
    EXPECT_NE(it, end);
    
    std::size_t count = 0;
    for (; it != end; ++it) {
        count++;
    }
    
    EXPECT_EQ(count, 5);
}

TEST(EnumIterationTest, IteratorDereference) {
    auto it = Fruit::begin();
    
    auto fruit = *it;
    EXPECT_EQ(fruit.name(), "Apple");
    EXPECT_EQ(fruit.index(), 0);
}

TEST(EnumIterationTest, IteratorArrow) {
    auto it = Fruit::begin();
    
    EXPECT_EQ(it->name(), "Apple");
    EXPECT_EQ(it->index(), 0);
}

TEST(EnumIterationTest, IteratorIncrement) {
    auto it = Fruit::begin();
    
    ++it;
    EXPECT_EQ(it->name(), "Banana");
    
    it++;
    EXPECT_EQ(it->name(), "Cherry");
}

TEST(EnumIterationTest, IteratorDecrement) {
    auto it = Fruit::end();
    --it;
    
    EXPECT_EQ(it->name(), "Elderberry");
    
    it--;
    EXPECT_EQ(it->name(), "Date");
}

TEST(EnumIterationTest, IteratorArithmetic) {
    auto it = Fruit::begin();
    
    auto it2 = it + 2;
    EXPECT_EQ(it2->name(), "Cherry");
    
    auto it3 = it2 - 1;
    EXPECT_EQ(it3->name(), "Banana");
    
    it += 3;
    EXPECT_EQ(it->name(), "Date");
    
    it -= 2;
    EXPECT_EQ(it->name(), "Banana");
}

TEST(EnumIterationTest, IteratorComparison) {
    auto it1 = Fruit::begin();
    auto it2 = Fruit::begin();
    auto it3 = Fruit::begin() + 1;
    
    EXPECT_TRUE(it1 == it2);
    EXPECT_FALSE(it1 != it2);
    EXPECT_TRUE(it1 != it3);
    EXPECT_TRUE(it1 < it3);
    EXPECT_TRUE(it3 > it1);
    EXPECT_TRUE(it1 <= it2);
    EXPECT_TRUE(it1 <= it3);
}

TEST(EnumIterationTest, IteratorDistance) {
    auto dist = std::distance(Fruit::begin(), Fruit::end());
    EXPECT_EQ(dist, 5);
}

TEST(EnumIterationTest, IteratorAdvance) {
    auto it = Fruit::begin();
    std::advance(it, 3);
    
    EXPECT_EQ(it->name(), "Date");
}

TEST(EnumIterationTest, IteratorNext) {
    auto it = Fruit::begin();
    auto next_it = std::next(it, 2);
    
    EXPECT_EQ(next_it->name(), "Cherry");
}

TEST(EnumIterationTest, IteratorPrev) {
    auto it = Fruit::end();
    auto prev_it = std::prev(it, 2);
    
    EXPECT_EQ(prev_it->name(), "Date");
}

TEST(EnumIterationTest, StdFindIf) {
    auto it = std::find_if(Fruit::begin(), Fruit::end(), [](auto f) {
        return f.name() == "Cherry";
    });
    
    ASSERT_NE(it, Fruit::end());
    EXPECT_EQ(it->name(), "Cherry");
    EXPECT_EQ(it->index(), 2);
}

TEST(EnumIterationTest, StdCount) {
    auto count = std::count_if(Fruit::begin(), Fruit::end(), [](auto f) {
        return f.name().length() > 5;
    });
    
    EXPECT_EQ(count, 3); // Banana(6), Cherry(6), Elderberry(10)
}

TEST(EnumIterationTest, StdTransform) {
    std::vector<std::string_view> names;
    
    std::transform(Fruit::begin(), Fruit::end(), std::back_inserter(names),
                   [](auto f) { return f.name(); });
    
    ASSERT_EQ(names.size(), 5);
    EXPECT_EQ(names[0], "Apple");
    EXPECT_EQ(names[4], "Elderberry");
}

TEST(EnumIterationTest, ConstexprIteration) {
    constexpr auto it = Fruit::begin();
    constexpr auto it2 = it + 2;
    constexpr auto idx = it2.index();
    
    static_assert(idx == 2, "Iterator should point to index 2");
}

TEST(EnumIterationTest, RangeSize) {
    auto range = Fruit::all();
    auto count = std::distance(range.begin(), range.end());
    
    EXPECT_EQ(count, 5);
}

TEST(EnumIterationTest, MutableContainer) {
    // Can store in vector and modify
    std::vector<Fruit::Enum> basket;
    basket.push_back(Fruit::Apple);
    basket.push_back(Fruit::Banana);
    basket.push_back(Fruit::Cherry);
    
    EXPECT_EQ(basket.size(), 3);
    EXPECT_EQ(basket[0], Fruit::Apple);
    
    // Can reassign elements
    basket[0] = Fruit::Date;
    EXPECT_EQ(basket[0], Fruit::Date);
    EXPECT_EQ(basket[0].name(), "Date");
    
    // Can use algorithms that modify elements
    std::fill(basket.begin(), basket.end(), Fruit::Elderberry);
    EXPECT_TRUE(std::all_of(basket.begin(), basket.end(), 
        [](auto f) { return f == Fruit::Elderberry; }));
}

} // namespace
