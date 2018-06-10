.PHONY: build build-cov clean ctags tests

BUILD_DIR=build
all: build

install:
	@cd ${BUILD_DIR} && make install

package: build
	@cd build && make package
	@cp build/sippak-*.rpm dist

build:
	@test -d ${BUILD_DIR} || mkdir ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake .. && make

build-debug:
	@test -d ${BUILD_DIR} || mkdir ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake .. -DCMAKE_BUILD_TYPE=Debug && make

build-cov: clean
	@mkdir ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake ..  -DCMAKE_BUILD_TYPE=Coverage \
		-DCMAKE_C_FLAGS="-Werror -fprofile-arcs -ftest-coverage -g -O0"
	@cd ${BUILD_DIR} && cmake --build .
	@cd ${BUILD_DIR} && ctest .
	@lcov --capture --directory "${BUILD_DIR}/src" --output-file ${BUILD_DIR}/cov_full.info \
		--gcov-tool gcov
	@lcov -r ${BUILD_DIR}/cov_full.info '/usr/local/include/pj*' -o ${BUILD_DIR}/cov.info
	@mkdir ${BUILD_DIR}/coverage
	@genhtml --output-directory ${BUILD_DIR}/coverage --title "sippak utility" \
		--legend --show-details ${BUILD_DIR}/cov.info
	@xdg-open ${BUILD_DIR}/coverage/index.html

test:
	@cd build && env CTEST_OUTPUT_ON_FAILURE=1 ctest

.PHONY: dev-test
# watchman-make -p 'src/**/*' 'tests/**/*' 'CMakeLists.txt' 'Makefile' -t dev-test
dev-test: build
	@./build/tests/unit/test_getopts
	@./build/tests/unit/test_dns_helper
	@./build/tests/unit/test_auth_calc

ctags:
	@echo -n 'Generating ctags...'
	@ctags -R src 2>/dev/null
	@echo ' done.'

clean:
	@echo "Clean build directory..."
	@rm -rf ${BUILD_DIR}
