default: all

include /usr/share/phantom/module.mk

$(eval $(call MODULE,io_benchmark/aggregating_logger))

include /usr/share/phantom/opts.mk

all: $(targets)

clean:; @rm -f $(targets) $(tmps) deps/*.d

.PHONY: default all clean

