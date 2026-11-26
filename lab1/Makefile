CXXFLAGS ?= -std=c++11 -O2 -Wall -Wextra -Wpedantic
DEBUG_CXXFLAGS ?= -std=c++11 -O0 -g -Wall -Wextra -Wpedantic
PYTHON ?= python3
BUILD_DIR ?= build

PROGRAM := $(BUILD_DIR)/dictionary
LIBRARY := $(BUILD_DIR)/libpatricia.a

.PHONY: all check

all: $(PROGRAM)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.cpp src/patricia.hpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(LIBRARY): $(BUILD_DIR)/patricia.o
	$(AR) rcs $@ $<

$(PROGRAM): $(BUILD_DIR)/main.o $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

check: $(PROGRAM)
	$(PYTHON) tests/check_cli.py $(PROGRAM)
	$(MAKE) BUILD_DIR=$(BUILD_DIR)/debug CPPFLAGS='$(CPPFLAGS) -DPATRICIA_CHECK_INVARIANTS' CXXFLAGS='$(DEBUG_CXXFLAGS) -UNDEBUG' all
	$(PYTHON) tests/check_cli.py $(BUILD_DIR)/debug/dictionary
