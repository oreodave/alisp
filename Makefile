CC=cc

DIST=build
OUT=$(DIST)/alisp.out
TEST=$(DIST)/test.out

LDFLAGS=
GFLAGS=-Wall -Wextra -Wpedantic -std=c23 -I./include/
DFLAGS=-ggdb -fsanitize=address -fsanitize=undefined -DVERBOSE_LOGS=1
RFLAGS=-O3

MODE=release
ifeq ($(MODE), release)
CFLAGS=$(GFLAGS) $(RFLAGS)
else
CFLAGS=$(GFLAGS) $(DFLAGS)
endif

# Units to compile
UNITS=src/sv.c src/vec.c src/stream.c src/symtable.c src/tag.c src/lisp.c
OBJECTS:=$(patsubst src/%.c, $(DIST)/%.o, $(UNITS))

TEST_UNITS=test/main.c
TEST_OBJECTS:=$(patsubst %.c, $(DIST)/%.o, $(TEST_UNITS))

# Dependency generation
DEPFLAGS=-MT $@ -MMD -MP -MF
DEPDIR=$(DIST)/deps

all: $(OUT) $(TEST)

$(OUT): $(OBJECTS) $(DIST)/main.o | $(DIST)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DIST)/%.o: src/%.c | $(DIST) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPDIR)/$*.d -c -o $@ $<

$(TEST): $(OBJECTS) $(TEST_OBJECTS) | $(DIST)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(DIST)/test/%.o: test/%.c | $(DIST) $(DEPDIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(DEPDIR)/test/$*.d -c -o $@ $<

$(DIST):
	mkdir -p $(DIST)
	mkdir -p $(DIST)/test

$(DEPDIR):
	mkdir -p $(DEPDIR)
	mkdir -p $(DEPDIR)/test

clangd: compile_commands.json
compile_commands.json: Makefile
	bear -- $(MAKE) -B MODE=debug

.PHONY: run test clean examples
ARGS=
run: $(OUT)
	./$^ $(ARGS)

test: $(TEST)
	./$^

clean:
	rm -rf $(DIST)

DEPS:=$(patsubst src/%.c,$(DEPDIR)/%.d, $(UNITS)) $(DEPDIR)/main.d $(DEPDIR)/test/main.d
include $(wildcard $(DEPS))
