
.PHONY: all clean rebuild install

#export runlink = static
#export platform = centos
#export sysroot_home = /
#export build_cmd_prefix = /usr/bin/

all:
	$(MAKE) -C ./src all
	$(MAKE) -C ./test all

clean:
	rm -rf ./lib/*
	$(MAKE) -C ./src clean
	$(MAKE) -C ./test clean

rebuild:
	rm -rf ./lib/*
	$(MAKE) -C ./src rebuild
	$(MAKE) -C ./test rebuild

install:
	rm -rf ./include/*
	$(MAKE) -C ./src install
