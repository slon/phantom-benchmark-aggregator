#include "phout_reader.h"

#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>

#include <iostream>

bool line_reader_t::readline(char** line) {
    while(true) {
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
    sscanf(get_token(&line), "%lf", &(result->time));
    result->tag = std::string(get_token(&line));

    sscanf(get_token(&line), "%d", &(result->total_time));
    sscanf(get_token(&line), "%d", &(result->conn_time));
    sscanf(get_token(&line), "%d", &(result->send_time));
    sscanf(get_token(&line), "%d", &(result->latency));
    sscanf(get_token(&line), "%d", &(result->recv_time));

    sscanf(get_token(&line), "%d", &(result->interval_event));

    sscanf(get_token(&line), "%d", &(result->size_out));
    sscanf(get_token(&line), "%d", &(result->size_in));

    sscanf(get_token(&line), "%d", &(result->err));
    sscanf(get_token(&line), "%d", &(result->proto_code));

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

bool phout_reader_t::log_writer_stopped() {
    int res = flock(fd, LOCK_EX | LOCK_NB);
    bool stopped;

    if(res == 0) {
        stopped = true;

        if(-1 == flock(fd, LOCK_UN))
            err(4, NULL);
    } else if(errno == EWOULDBLOCK) {
        stopped = false;
    } else {
        err(3, NULL);
    }

    return stopped;
}

phout_reader_t::~phout_reader_t() {
    close(fd);
}
