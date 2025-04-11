# project name
project_name               := $(shell basename "$(CURDIR)")



# arguments
runlink                    ?= static
platform                   ?= centos
macro                       = -DDEBUG



# sysroot
sysroot_home               ?= /
sysroot_params              = --sysroot=$(sysroot_home)
sysroot_includes            = -I$(sysroot_home)



# toolchain
build_cmd_prefix           ?= /usr/bin/
build_c                     = $(build_cmd_prefix)gcc $(sysroot_params) $(macro)
build_cxx                   = $(build_cmd_prefix)g++ $(sysroot_params) $(macro) -std=c++14
build_link                  = $(build_cmd_prefix)ar



# paths home
project_home                = .
build_dir                   = $(project_home)
bin_dir                     = $(project_home)/../../lib
install_dir                 = $(project_home)/../../include
object_dir                  = $(project_home)/.objs
system_inc                  = $(sysroot_home)/usr/include
system_lib                  = $(sysroot_home)/usr/lib/aarch64-linux-gnu



# includes of project headers
project_inc_path            = $(project_home)
project_includes            = -I$(project_inc_path)

# includes of system headers
sys_inc_path                = $(system_inc)
sys_includes                = -I$(sys_inc_path)



# all includes that project solution needs
includes                    = $(project_includes)
includes                   += $(sys_includes)



# source files of project solution
project_src_path            = $(project_home)
project_cpp_source          = $(filter %.cpp, $(shell find $(project_src_path) -depth -name "*.cpp"))
project_cc_source           = $(filter %.cc, $(shell find $(project_src_path) -depth -name "*.cc"))
project_c_source            = $(filter %.c, $(shell find $(project_src_path) -depth -name "*.c"))



# objects of project solution
project_objects             = $(project_cpp_source:$(project_home)%.cpp=$(object_dir)%.o)
project_objects            += $(project_cc_source:$(project_home)%.cc=$(object_dir)%.o)
project_objects            += $(project_c_source:$(project_home)%.c=$(object_dir)%.o)



# system libraries
sys_lib_path                = $(system_lib)
sys_libs                    = -L$(sys_lib_path) -lpthread -ldl -lrt



# project depends libraries
project_depends             = $(sys_libs)



# output libraries
ifeq ($(runlink), static)
	project_outputs = $(bin_dir)/lib$(project_name).a
else ifeq ($(platform), mac)
	project_outputs = $(bin_dir)/lib$(project_name).dylib
else
	project_outputs = $(bin_dir)/lib$(project_name).so
endif



# ignore warnings
c_no_warnings   = -Wno-error=deprecated-declarations -Wno-deprecated-declarations -Wno-unused-result

ifeq ($(platform), mac)
cxx_no_warnings = $(c_no_warnings)
else
cxx_no_warnings = $(c_no_warnings) -Wno-class-memaccess
endif



# build output command line
ifeq ($(runlink), static)
	build_command = $(build_link) -rv $(project_outputs) $^
else
	build_command = $(build_cxx) -shared -o $(project_outputs) $^ $(project_depends)
endif



# build targets
targets = project

# let 'build' be default target, build all targets
build   : $(targets)

project : $(project_objects)
	mkdir -p $(bin_dir)
	@echo
	@echo "@@@@@  start making $(project_name)  @@@@@"
	$(build_command)
	@echo "@@@@@  make $(project_name) success  @@@@@"
	@echo

# build all objects
$(object_dir)/%.o:$(project_home)/%.cpp
	@dir=`dirname $@`;		\
	if [ ! -d $$dir ]; then	\
		mkdir -p $$dir;		\
	fi
	$(build_cxx) -c -g -Wall -O1 -pipe -fPIC $(cxx_no_warnings) $(includes) -o $@ $<

$(object_dir)/%.o:$(project_home)/%.cc
	@dir=`dirname $@`;		\
	if [ ! -d $$dir ]; then	\
		mkdir -p $$dir;		\
	fi
	$(build_cxx) -c -g -Wall -O1 -pipe -fPIC $(cxx_no_warnings) $(includes) -o $@ $<

$(object_dir)/%.o:$(project_home)/%.c
	@dir=`dirname $@`;		\
	if [ ! -d $$dir ]; then	\
		mkdir -p $$dir;		\
	fi
	$(build_c) -c -g -O1 -pipe -fPIC $(c_no_warnings) $(includes) -o $@ $<

clean    :
	rm -rf $(object_dir) $(bin_dir)/lib$(project_name).*

rebuild  : clean build

install  :
	if [ ! -d $(install_dir) ]; then	\
		mkdir -p $(install_dir);		\
	fi
	cp -f $(project_inc_path)/$(project_name).h $(install_dir)/
