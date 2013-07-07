#include <phantom/io_benchmark/method_stream/logger.H>
#include <phantom/io_logger_file.H>

namespace phantom { namespace io_benchmark {

using method_stream::result_t;
using method_stream::logger_t;

class aggregating_logger_t : public logger_t,
                             public io_logger_file_t {
public:
    struct config_t : logger_t::config_t,
                      io_logger_file_t::config_t {
        void check(const in_t::ptr_t& ptr) const {
            logger_t::config_t::check(ptr);
            io_logger_file_t::config_t::check(ptr);
        }
    };

    aggregating_logger_t(const string_t& name,
                         const config_t& config)
        : logger_t(name, config),
          io_logger_file_t(name, config) {}

    virtual void commit(
        in_segment_t const &request, in_segment_t const &tag, result_t const &res
    );
};

namespace aggreagting_logger {
config_binding_sname(aggregating_logger_t);
config_binding_parent(aggregating_logger_t, logger_t, 1);
config_binding_parent(aggregating_logger_t, io_logger_file_t, 2);
config_binding_ctor(logger_t, aggregating_logger_t);
}

}} // namespace phantom::io_benchmark
