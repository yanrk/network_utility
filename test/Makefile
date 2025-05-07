DIRS := $(wildcard *)

.PHONY: all clean

all:
	@for dir in $(DIRS); do            \
		if [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir;          \
		fi                             \
	done

clean:
	@for dir in $(DIRS); do            \
		if [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir clean;    \
		fi                             \
	done

rebuild:
	@for dir in $(DIRS); do            \
		if [ -f $$dir/Makefile ]; then \
			$(MAKE) -C $$dir rebuild;  \
		fi                             \
	done
