#include "stats.h"

#include <vector>

void mean_counter_t::count(int value) {
    ++n_samples;
    sum += value;
}

double mean_counter_t::mean() {
    return n_samples == 0 ? 0.0 : (static_cast<double>(sum) / n_samples);
}

void variance_counter_t::count(int value) {
    ++n_samples;
    sum += value;
    squares_sum += value * value;
}

double variance_counter_t::mean() {
    return n_samples == 0 ? 0.0 : (static_cast<double>(sum) / n_samples);
}

double variance_counter_t::variance() {
    if(n_samples < 2) {
        return 0.0;
    }

    return (squares_sum / n_samples) - mean() * mean();
}

void histogram_counter_t::count(int value) {
    ++n_samples;

    auto bucket = distribution.find(value);
    if(bucket == distribution.end()) {
        bucket = distribution.insert(std::make_pair(value, 0)).first;
    }

    ++bucket->second;
}

void histogram_counter_t::print_full(std::ostream& out) {
    out << "{ ";

    bool preceding_coma = false;
    for(auto& point : distribution) {
        if(preceding_coma) { out << ", "; }

        out << "\"" << point.first << "\": "
            << point.second;

        preceding_coma = true;
    }

    out << " }";
}

const std::vector<int> QUANTILES = { 25, 50, 75, 80, 90, 95, 98, 99, 100 };

void histogram_counter_t::print_quantiles(std::ostream& out) {
    int64_t cdf = 0;
    auto point = distribution.begin();

    out << "{ ";

    bool preceding_coma = false;
    for(int quantile : QUANTILES) {
        if(preceding_coma) { out << ", "; }

        while(100 * (cdf + point->second) < quantile * n_samples) {
            cdf += point->second;
            ++point;
        }

        out << "\"" << quantile << "\": " << static_cast<double>(point->first) / 1000;
        preceding_coma = true;
    }

    out << " }";
}

void stats_t::print(std::ostream& out) {
    out << "{ ";

    out << "\"resps\": " << resps << ", ";

    out << "\"overall_time\": " << overall_time.mean() / 1000 << ", ";
    out << "\"connect_time\": " << connect_time.mean() / 1000 << ", ";
    out << "\"send_time\": " << send_time.mean() / 1000 << ", ";
    out << "\"latency\": " << latency.mean() / 1000 << ", ";
    out << "\"receive_time\": " << receive_time.mean() / 1000 << ", ";

    out << "\"disper\": " << overall_time.variance() / 1000000 << ", ";

    out << "\"input\": " << input << ", ";
    out << "\"output\": " << output << ", ";

    out << "\"net_codes\": ";
    net_codes.print_full(out);
    out << ", ";

    out << "\"http_codes\": ";
    http_codes.print_full(out);
    out << ", ";

    out << "\"quantiles\": ";
    quantiles.print_quantiles(out);
    out << " }";
}

void stats_t::count(const result_t& result) {
    ++resps;

    overall_time.count(result.total_time);

    connect_time.count(result.conn_time);
    send_time.count(result.send_time);
    latency.count(result.latency);
    receive_time.count(result.recv_time);

    input += result.size_in;
    output += result.size_out;

    net_codes.count(result.err);
    http_codes.count(result.proto_code);

    quantiles.count(result.total_time);
}

void full_stats_t::count(const result_t& result) {
    int second = result.time;

    if(second > this_second) {
        if(this_second != -1)
            print();

        overall = stats_t();
        this_second = second;
        results_in_this_second = 0;
    }

    ++results_in_this_second;

    cumulative.count(result);
    overall.count(result);

    if(!result.tag.empty()) {
        tags[result.tag].count(result);
    }
}

void full_stats_t::flush() {
    if(results_in_this_second > 0) {
        print();
    }
}

void full_stats_t::print() {
    out << "{ ";

    out << "\"timestamp\": " << this_second << ", ";

    out << "\"cumulative\": ";
    cumulative.print(out);
    out << ", ";

    out << "\"overall\": ";
    overall.print(out);
    out << ", ";

    out << "\"tags\": { ";
    bool preceding_coma = false;
    for(auto& tag : tags) {
        if(preceding_coma) out << ", ";

        out << "\"" << tag.first << "\": ";
        tag.second.print(out);
    }

    out << " } }" << std::endl;
}
