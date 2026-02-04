CC=cc

DIST=build
OUT=$(DIST)/alisp.out

UNITS=$(shell find ./src -type 'f')
OBJECTS:=$(patsubst %.c, $(DIST)/%.o, $(UNITS))

LDFLAGS=
GFLAGS=-Wall -Wextra -Wpedantic -std=c23 -I./include/
DFLAGS=-ggdb -fsanitize=address -fsanitize=undefined
RFLAGS=-O3

MODE=release
ifeq ($(MODE), release)
CFLAGS=$(GFLAGS) $(RFLAGS)
else
CFLAGS=$(GFLAGS) $(DFLAGS)
endif

# Dependency generation
DEPFLAGS=-MT $@ -MMD -MP -MF
DEPDIR=$(DIST)/deps

$(OUT): $(OBJECTS) | $(DIST)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DIST)/%.o: %.c | $(DIST) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPDIR)/$*.d -c -o $@ $<

$(DIST):
	mkdir -p $(DIST)/src

$(DEPDIR):
	mkdir -p $(DEPDIR)/src

clangd: compile_commands.json
compile_commands.json: Makefile
	bear -- $(MAKE) -B MODE=debug

.PHONY: run clean examples
ARGS=
run: $(OUT)
	./$^ $(ARGS)

clean:
	rm -rf $(DIST)

DEPS:=$(patsubst %.c,$(DEPDIR)/%.d, $(UNITS))
include $(wildcard $(DEPS))
