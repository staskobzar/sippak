.PHONY: build clean ctags tests

BUILD_DIR=build
all: build

build:
	@test -d ${BUILD_DIR} || mkdir ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake .. && make

test:
	@cd build && env CTEST_OUTPUT_ON_FAILURE=1 ctest

ctags:
	@echo -n 'Generating ctags...'
	@ctags -R src 2>/dev/null
	@echo ' done.'

clean:
	@rm -rf ${BUILD_DIR}
