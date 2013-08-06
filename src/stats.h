#pragma once

#include <cstdint>
#include <iostream>
#include <map>

#include "phout_reader.h"

class mean_counter_t {
public:
    mean_counter_t() : n_samples(0), sum(0) {}

    void count(int value);
    double mean();

private:
    int64_t n_samples, sum;
};

class variance_counter_t {
public:
    variance_counter_t() : n_samples(0),
                           sum(0),
                           squares_sum(0.0) {}

    void count(int value);
    double mean();
    double variance();

private:
    int64_t n_samples, sum;
    double squares_sum;
};

class histogram_counter_t {
public:
    histogram_counter_t() : n_samples(0) {}

    void count(int value);

    void print_full(std::ostream& out);
    void print_quantiles(std::ostream& out);

private:
    std::map<int, int64_t> distribution;
    int64_t n_samples;
};

class stats_t {
public:
    stats_t() : resps(0), input(0), output(0) {}

    void print(std::ostream& out);

    void count(const result_t& result);

private:
    int64_t resps;
    variance_counter_t overall_time;
    mean_counter_t connect_time, send_time, latency, receive_time;

    mean_counter_t accuracy;

    int64_t input, output;

    histogram_counter_t net_codes, http_codes;

    histogram_counter_t quantiles;
};


class full_stats_t {
public:
    full_stats_t(std::ostream& out)
        : out(out), this_second(-1), results_in_this_second(0) {}

    void count(const result_t& result);

    void flush();

private:
    std::ostream& out;

    int this_second;
    int64_t results_in_this_second;

    stats_t cumulative, overall;
    std::map<std::string, stats_t> tags;

    void print();
};
