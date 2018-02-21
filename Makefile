.PHONY: build clean

BUILD_DIR=build
all: build

build:
	@test -d ${BUILD_DIR} || mkdir ${BUILD_DIR}
	@cd ${BUILD_DIR} && cmake .. && make

clean:
	@rm -rf ${BUILD_DIR}
