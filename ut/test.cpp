#include <gtest/gtest.h>

#include <sstream>

#include "stats.h"

TEST(stats, mean_counter) {
    mean_counter_t counter;

    ASSERT_DOUBLE_EQ(0.0, counter.mean());

    counter.count(10);
    ASSERT_DOUBLE_EQ(10.0, counter.mean());

    counter.count(100010);
    counter.count(-99990);
    ASSERT_DOUBLE_EQ(10.0, counter.mean());
}

TEST(stats, variance_counter) {
    variance_counter_t c;

    ASSERT_DOUBLE_EQ(0.0, c.mean());
    ASSERT_DOUBLE_EQ(0.0, c.variance());

    c.count(-3);

    ASSERT_DOUBLE_EQ(-3.0, c.mean());
    ASSERT_DOUBLE_EQ(0.0, c.variance());

    c.count(3);
    c.count(0);

    ASSERT_DOUBLE_EQ(0.0, c.mean());
    ASSERT_DOUBLE_EQ(6.0, c.variance());
}

std::string p_full(histogram_counter_t& c) {
    std::stringstream ss;
    c.print_full(ss);
    return ss.str();
}

std::string p_quantiles(histogram_counter_t& c) {
    std::stringstream ss;
    c.print_quantiles(ss);
    return ss.str();
}

std::string quantiles(int q25, int q50, int q75, int q80, int q90, int q95, int q98, int q99, int q100) {
    return "{ \"25\": " + std::to_string(q25) +
           ", \"50\": " + std::to_string(q50) +
           ", \"75\": " + std::to_string(q75) +
           ", \"80\": " + std::to_string(q80) +
           ", \"90\": " + std::to_string(q90) +
           ", \"95\": " + std::to_string(q95) +
           ", \"98\": " + std::to_string(q98) +
           ", \"99\": " + std::to_string(q99) +
           ", \"100\": " + std::to_string(q100) +
           " }";
}

TEST(stats, histogram_counter) {
    histogram_counter_t c;

    ASSERT_EQ("{  }", p_full(c));
    ASSERT_EQ(quantiles(0, 0, 0, 0, 0, 0, 0, 0, 0), p_quantiles(c));

    c.count(0);

    ASSERT_EQ("{ \"0\": 1 }", p_full(c));
    ASSERT_EQ(quantiles(0, 0, 0, 0, 0, 0, 0, 0, 0), p_quantiles(c));

    for(int i = 1; i < 100; ++i) {
        c.count(i);
    }

    ASSERT_EQ(quantiles(24, 49, 74, 79, 89, 94, 97, 98, 99), p_quantiles(c));
}
