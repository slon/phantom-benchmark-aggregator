#pragma once

#include <string>
#include <vector>

struct result_t {
    double time;

    std::string tag;

    int total_time, conn_time, send_time, latency, recv_time;

    int interval_event;

    int size_out, size_in;

    int err, proto_code;
};

class line_reader_t {
public:
    line_reader_t()
        : fd(-1),
          rpos(0),
          wpos(0),
          line_size(0),
          buffer(64 * 1024 * 1024) {}

    bool readline(char** line);

    void set_fd(int _fd) { fd = _fd; }

private:
    int fd;

    int read_some();

    int rpos, wpos, line_size;
    std::vector<char> buffer;
};

class phout_reader_t {
public:
    phout_reader_t(char const* filename);
    ~phout_reader_t();

    bool read(result_t* result);

private:
    int fd;
    line_reader_t line_reader;

    bool log_writer_stopped();

    char* get_token(char** line);
    void parse_line(char* line, result_t* result);
};
