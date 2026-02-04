CC=cc

DIST=build
OUT=$(DIST)/alisp.out

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

# Units to compile
UNITS=src/main.c src/sv.c src/vec.c src/stream.c src/symtable.c src/tag.c src/lisp.c src/reader.c
OBJECTS:=$(patsubst src/%.c, $(DIST)/%.o, $(UNITS))

# Dependency generation
DEPFLAGS=-MT $@ -MMD -MP -MF
DEPDIR=$(DIST)/deps

$(OUT): $(OBJECTS) | $(DIST)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DIST)/%.o: src/%.c | $(DIST) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPDIR)/$*.d -c -o $@ $<

$(DIST):
	mkdir -p $(DIST)

$(DEPDIR):
	mkdir -p $(DEPDIR)

clangd: compile_commands.json
compile_commands.json: Makefile
	bear -- $(MAKE) -B MODE=debug

.PHONY: run clean examples
ARGS=
run: $(OUT)
	./$^ $(ARGS)

clean:
	rm -rf $(DIST)

DEPS:=$(patsubst src/%.c,$(DEPDIR)/%.d, $(UNITS))
include $(wildcard $(DEPS))
