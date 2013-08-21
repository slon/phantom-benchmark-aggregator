#include "phout_reader.h"

#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#include <iostream>
#include <stdexcept>

bool line_reader_t::readline(char** line) {
    while(true) {
        if(line_size > 16 * 1024)
            throw std::runtime_error("line size is greater than 16K");

        // end of buffer, no '\n' found, read more bytes
        if(rpos + line_size == wpos && read_some() == 0) {
            return false;
        }

        if(buffer[rpos + line_size] == '\n') break;

        ++line_size;
    }

    buffer[rpos + line_size] = '\0';

    *line = buffer.data() + rpos;

    rpos += line_size + 1;
    line_size = 0;

    return true;
}

int line_reader_t::read_some() {
    if(rpos != wpos) {
        memmove(buffer.data(), buffer.data() + rpos, wpos - rpos);
    }

    wpos -= rpos;
    rpos = 0;

    int res = read(fd, buffer.data() + wpos, buffer.size() - wpos);

    if(res < 0) {
        err(1, NULL);
    }

    wpos += res;
    return res;
}

phout_reader_t::phout_reader_t(char const* filename) : fd(-1) {
    fd = open(filename, O_RDONLY);

    if(fd == -1)
        err(2, NULL);

    line_reader.set_fd(fd);
}

bool phout_reader_t::read(result_t* result) {
    char* line;

    while(!line_reader.readline(&line)) {
        if(log_writer_stopped()) {
            return false;
        } else {
            usleep(100000);
            continue;
        }
    }

    parse_line(line, result);

    return true;
}

void phout_reader_t::parse_line(char* line,
                                result_t* result) {
    result->time = atof(get_token(&line));

    result->tag = std::string(get_token(&line));

    result->total_time = atoi(get_token(&line));
    result->conn_time = atoi(get_token(&line));
    result->send_time = atoi(get_token(&line));
    result->latency = atoi(get_token(&line));
    result->recv_time = atoi(get_token(&line));

    result->interval_event = atoi(get_token(&line));

    result->size_out = atoi(get_token(&line));
    result->size_in = atoi(get_token(&line));

    result->err = atoi(get_token(&line));
    result->proto_code = atoi(get_token(&line));

    // convert to milliseconds
    result->total_time /= 1000;
    result->conn_time /= 1000;
    result->send_time /= 1000;
    result->latency /= 1000;
    result->recv_time /= 1000;

    result->interval_event /= 1000;
}


char* phout_reader_t::get_token(char** line) {
    char* start = *line;

    while(**line != '\0' && **line != '\t') {
        ++(*line);
    }

    if(**line == '\t') {
        **line = '\0';
        ++(*line);
    }

    return start;
}

sig_atomic_t stop_phantom_aggregator;

bool phout_reader_t::log_writer_stopped() {
    return stop_phantom_aggregator;
}

phout_reader_t::~phout_reader_t() {
    close(fd);
}
