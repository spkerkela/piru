CC = gcc
CFLAGS = -Wall -Wextra -g -I. -I../
# Use mock SDL2 headers from tests/SDL2 instead of system SDL2
CFLAGS += -I.
LDFLAGS = -lm

SRC_DIR = ..
TEST_DIR = .
BUILD_DIR = build

TESTS = test_piru_math test_point test_direction test_pathfinding test_dungeon

.PHONY: all clean test

all: $(TESTS)

test: $(TESTS)
	@echo ""
	@echo "========================================"
	@echo "Running all tests..."
	@echo "========================================"
	@failed=0; \
	for t in $(TESTS); do \
		$(BUILD_DIR)/$$t || failed=$$((failed + 1)); \
	done; \
	echo ""; \
	echo "========================================"; \
	if [ $$failed -eq 0 ]; then \
		echo "\033[32mAll test suites passed!\033[0m"; \
	else \
		echo "\033[31m$$failed test suite(s) had failures\033[0m"; \
	fi; \
	echo "========================================"; \
	exit $$failed

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test_piru_math: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ \
		$(TEST_DIR)/test_piru_math.c \
		$(SRC_DIR)/piru_math.c \
		$(LDFLAGS)

test_point: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ \
		$(TEST_DIR)/test_point.c \
		$(SRC_DIR)/point.c \
		$(LDFLAGS)

test_direction: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ \
		$(TEST_DIR)/test_direction.c \
		$(SRC_DIR)/direction.c \
		$(LDFLAGS)

test_pathfinding: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ \
		$(TEST_DIR)/test_pathfinding.c \
		$(SRC_DIR)/pathfinding.c \
		$(SRC_DIR)/point.c \
		$(SRC_DIR)/dungeon.c \
		$(SRC_DIR)/piru_math.c \
		$(LDFLAGS)

test_dungeon: $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ \
		$(TEST_DIR)/test_dungeon.c \
		$(SRC_DIR)/dungeon.c \
		$(SRC_DIR)/piru_math.c \
		$(SRC_DIR)/point.c \
		$(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)
