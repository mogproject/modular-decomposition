RM = /bin/rm
PYTHON=python3

SRC_CPP=src/main/cpp
BUILD_DIR=$(PWD)/build
TEST_BIN_DIR=$(BUILD_DIR)/test
TEST_EXEC=$(TEST_BIN_DIR)/modular_test

SRC_PY=src/main/python
TEST_PY=src/test/python
STUB_PY=src/main/stubs

PYTEST_OPTS=""  # --full-trace

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

PROFILE_ON ?= false
TRACE_ON ?= false

build:
	cd $(SRC_CPP) && cmake -S . -B $(BUILD_DIR)/Release -DCMAKE_BUILD_TYPE=Release -DPROFILE_ON=$(PROFILE_ON) -DTRACE_ON=$(TRACE_ON)
	cd $(SRC_CPP) && cmake --build $(BUILD_DIR)/Release

test: test-cpp test-py

test-py:
	mypy $(SRC_PY)
	$(PYTHON) -m pytest -x --cov=$(SRC_PY) --cov-report=lcov:./coverage/lcov.info $(PYTEST_OPTS) $(TEST_PY)

test-cpp:
	@echo "GTEST_FILTER: $(GTEST_FILTER)"
	cd $(SRC_CPP) && cmake -DBUILD_TESTS=ON -S . -B $(BUILD_DIR)/Debug
	cd $(SRC_CPP) && cmake --build $(BUILD_DIR)/Debug
	$(TEST_EXEC) --output-on-failure $(GTEST_OPTS)

clean:
	@echo "Cleaning..."
	@$(RM) -rf build/*
	@echo "Cleaning done."

lab:
	jupyter-lab

.PHONY: build test test-cpp clean lab
