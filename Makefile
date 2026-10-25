# Build system for the multi-threaded loader (sorting / string-search work will plug into src/).
#
#   make                 optimised build            -> build_cpp/release/mthcss
#   make debug|asan|tsan other flavours (each in its own directory, so flags never mix)
#   make data            raw Reviews.csv -> dataset/Reviews_clean.csv + .expected reference values
#   make verify          load the clean CSV and compare against the pandas reference values
#   make run THREADS=2   load only, with a chosen thread count
#   make test            loader unit tests under ASan+UBSan
#   make test-tsan       same tests under ThreadSanitizer (detects data races)
#   make check           test + test-tsan
#   make clean
#
# Override examples:  make CXX=clang++   |   make data PY=python3   |   make verify THREADS=1

CXX      ?= g++
BUILD    := build_cpp
HEADERS  := $(wildcard src/*.h src/*/*.h)

RAW      ?= dataset/Reviews.csv
DATA     ?= dataset/Reviews_clean.csv
EXPECT   ?= $(DATA:.csv=.expected)
THREADS  ?= 4
PY       ?= $(if $(shell command -v uv 2>/dev/null),uv run python,python3)

COMMON   := -std=c++17 -Wall -Wextra -Wpedantic -pthread -Isrc

# Per-flavour flags. Sanitizer builds are -O1 so stack traces stay readable but runs stay fast.
FLAGS_release := -O2 -DNDEBUG
FLAGS_debug   := -O0 -g
FLAGS_asan    := -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
FLAGS_tsan    := -O1 -g -fno-omit-frame-pointer -fsanitize=thread

.PHONY: all debug asan tsan run verify data test test-tsan check clean
.DEFAULT_GOAL := all

all:   $(BUILD)/release/mthcss
debug: $(BUILD)/debug/mthcss
asan:  $(BUILD)/asan/mthcss
tsan:  $(BUILD)/tsan/mthcss

# Header-only library: one translation unit per binary, rebuilt whenever any header changes.
$(BUILD)/%/mthcss: src/main.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON) $(FLAGS_$*) $< -o $@

$(BUILD)/%/test_loader: tests/test_loader.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CXX) $(COMMON) $(FLAGS_$*) $< -o $@

run: all
	$(BUILD)/release/mthcss --data $(DATA) --threads $(THREADS)

verify: all
	$(BUILD)/release/mthcss --data $(DATA) --threads $(THREADS) --expect $(EXPECT)

data:
	$(PY) tools/preprocess.py $(RAW) $(DATA)

test: $(BUILD)/asan/test_loader
	$< 

test-tsan: $(BUILD)/tsan/test_loader
	$<

check: test test-tsan

clean:
	rm -rf $(BUILD)
