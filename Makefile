PYTHON=python3

SRC_PY=src/python/main
TEST_PY=src/python/test
STUB_PY=src/python/stubs

export PYTHONPATH=$(SRC_PY)
export MYPYPATH=$(STUB_PY)

test:
	mypy $(SRC_PY)
	$(PYTHON) -m pytest -x --cov=. --cov-report=xml:cov.xml $(TEST_PY)

.PHONY: test
