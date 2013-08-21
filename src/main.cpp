#include <iostream>

#include "phout_reader.h"
#include "stats.h"


const std::string HELP = R"(usage:
    phantom_benchmark_aggregator <phout>
)";

extern "C" void term_signal_handler(int signal) {
    stop_phantom_aggregator = true;
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << HELP;
        return -1;
    }

    stop_phantom_aggregator = false;
    std::signal(SIGTERM, term_signal_handler);

    phout_reader_t reader(argv[1]);

    full_stats_t stats(std::cout);

    result_t result;
    while(reader.read(&result)) {
        stats.count(result);
    }

    stats.flush();

    return 0;
}
