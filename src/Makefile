BUILD_DIR     = ../build
BUILD_COV_DIR     = ../build_coverage
TARGET        = $(BUILD_DIR)/main
FLAGS         = -Wall -Wextra -O2
TO_FORMAT     = $(shell find . -type f -name '*.cpp')

all: build
	$(TARGET)
	

build:
	cmake -S ../ -B ../build/
	cmake --build ../build/ --target main

test:
	cmake -S ../ -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --target test_model
	ctest --test-dir $(BUILD_DIR) --output-on-failure

clean:
	rm -rf $(BUILD_DIR)

cf:
	clang-format -n $(TO_FORMAT)

gdb: $(TARGET)
	gdb $(TARGET)

gcov:
	cmake -S ../ -B $(BUILD_COV_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage
	cmake --build $(BUILD_COV_DIR) --target test_model
	ctest --test-dir $(BUILD_COV_DIR) --output-on-failure
	mkdir -p $(BUILD_COV_DIR)/report
	gcovr -r .. \
		--object-directory $(BUILD_COV_DIR) \
		--gcov-exclude-directories '.*CompilerId(C|CXX).*' \
		--html-details -o $(BUILD_COV_DIR)/report/index.html \
		--print-summary \
		--exclude ".*src/tests/.*" \
		--exclude-throw-branches \
		--exclude-unreachable-branches
	@echo "Отчет успешно сгенерирован: $(BUILD_COV_DIR)/report/index.html"


.PHONY: all build test clean cf gdb gcov
