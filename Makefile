RM = /bin/rm
PYTHON=python3
MKDIR=mkdir
GCOV=gcov
LCOV=lcov
GENHTML=genhtml

ifneq ("$(wildcard /opt/homebrew/bin/g++-12)","")
export CC=/opt/homebrew/bin/gcc-12
export CXX=/opt/homebrew/bin/g++-12
GCOV=/opt/homebrew/bin/gcov-12
else
ifneq ("$(wildcard /opt/homebrew/bin/g++-11)","")
export CC=/opt/homebrew/bin/gcc-11
export CXX=/opt/homebrew/bin/g++-11
GCOV=/opt/homebrew/bin/gcov-11
endif
endif

CMAKE=cmake
CMAKE_OPTS=-DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX)

SRC_CPP=src/main/cpp
BUILD_DIR=$(PWD)/build
TEST_BIN_DIR=$(BUILD_DIR)/test
TEST_EXEC=$(TEST_BIN_DIR)/modular_test
COV_CPP_DIR=coverage/cpp
COV_CPP=$(COV_CPP_DIR)/lcov.info
COV_PY_DIR=coverage/py
COV_PY=$(COV_PY_DIR)/lcov.info
COV_HTML=coverage/html
COV_MERGED=coverage/lcov.info

SRC_PY=src/main/python
TEST_PY=src/test/python
STUB_PY=src/main/stubs

PYTEST_OPTS=""  # --full-trace

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

PROFILE_ON ?= false
TRACE_ON ?= false

build:
	cd $(SRC_CPP) && $(CMAKE) -S . -B $(BUILD_DIR)/Release $(CMAKE_OPTS) -DCMAKE_BUILD_TYPE=Release -DPROFILE_ON=$(PROFILE_ON) -DTRACE_ON=$(TRACE_ON)
	cd $(SRC_CPP) && $(CMAKE) --build $(BUILD_DIR)/Release

test: test-cpp test-py

test-cpp:
	@echo "GTEST_FILTER: $(GTEST_FILTER)"
	cd $(SRC_CPP) && $(CMAKE) -DBUILD_TESTS=ON -S . -B $(BUILD_DIR)/Debug $(CMAKE_OPTS)
	cd $(SRC_CPP) && $(CMAKE) --build $(BUILD_DIR)/Debug
	$(TEST_EXEC) --output-on-failure $(GTEST_OPTS)
	$(MKDIR) -p $(COV_CPP_DIR)
	$(LCOV) --gcov-tool $(GCOV) -d $(TEST_BIN_DIR) -c -o "$(COV_CPP)"
	$(LCOV) -r "${COV_CPP}" "*/include/*" "*.h" "*/test/*" "*/build*/*" -o "${COV_CPP}"

test-py:
	mypy $(SRC_PY)
	$(MKDIR) -p $(COV_PY_DIR)
	$(PYTHON) -m pytest -x --cov=$(SRC_PY) --cov-report=lcov:$(COV_PY) $(PYTEST_OPTS) $(TEST_PY)

coverage: test
	$(LCOV) --add-tracefile $(COV_PY) -a $(COV_CPP) -o $(COV_MERGED)

coverage-html: coverage
	$(GENHTML) -o $(COV_HTML) $(COV_MERGED)
	open $(COV_HTML)/index.html

clean:
	@echo "Cleaning..."
	@$(RM) -rf build/* coverage/*
	@echo "Cleaning done."

lab:
	jupyter-lab

.PHONY: build test test-py test-cpp coverage coverage-html clean lab
