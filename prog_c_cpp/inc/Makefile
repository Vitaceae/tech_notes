# example usage:
#   make SRC_FILE=clib_qsort
#   make clean

SRC_FILE ?=
ELF_NAME = $(SRC_FILE)
src_dir := $(shell pwd)
src = $(SRC_FILE).c
lib_dir := $(src_dir)/lib
lib_list := -L$(lib_dir) $(addprefix -l, $(patsubst lib%.a, %, $(notdir $(wildcard $(lib_dir)/*.a))))
obj_dir = $(shell pwd)/obj
objs := $(addprefix $(obj_dir)/, $(patsubst %.c, %.o, $(notdir $(subst ./,,$(src)))))

AR = ar
CC = gcc
CXX = g++
RM = rm
LOCAL_CFLAGS = $(CFLAGS) -I$(src_dir) -D_REENTRANT
LOCAL_LFLAGS = $(LDFLAGS) -lpthread -lrt $(lib_list)

.NOTPARALLEL:
all: prepare_env $(ELF_NAME) clean_objs

prepare_env:
	@echo "preparing make environment..."
	@mkdir -p $(obj_dir)
	@echo CC=$(CC)
	@echo AR=$(AR)

$(ELF_NAME): $(objs)
	@echo "making $(ELF_NAME)"
	@$(CC) $^ -o $(ELF_NAME) $(LOCAL_LFLAGS)

$(objs): $(obj_dir)/%.o: $(src_dir)/%.c
	@$(CC) $(LOCAL_CFLAGS) -c -o $@ $<

.PHONY: clean clean_objs
clean_objs:
	@echo "clean objects"
	@$(RM) -rf $(obj_dir)

clean: clean_objs
	@echo "clean all"
	@$(RM) -rf $(ELF_NAME)
