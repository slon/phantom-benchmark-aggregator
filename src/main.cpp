#include <iostream>

#include "phout_reader.h"
#include "stats.h"


const std::string HELP = R"(usage:
    phantom_benchmark_aggregator <phout>
)";




int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << HELP;
        return -1;
    }

    phout_reader_t reader(argv[1]);

    full_stats_t stats(std::cout);

    result_t result;
    while(reader.read(&result)) {
        stats.count(result);
    }

    stats.flush();

    return 0;
}
