CLEANUP = rm -f

C_COMPILER=gcc
ifeq ($(shell uname -s), Darwin)
C_COMPILER=clang
endif

UNITY_ROOT=unity

TEST_TARGET=bin/run_test

SRC_FILES=$(UNITY_ROOT)/src/unity.c $(UNITY_ROOT)/extras/memory/src/unity_memory.c $(UNITY_ROOT)/extras/fixture/src/unity_fixture.c  test/rent_tests.c test/test_runner.c test/all_tests.c

INC_DIRS=-Isrc -I$(UNITY_ROOT)/src -I$(UNITY_ROOT)/extras/fixture/src -I$(UNITY_ROOT)/extras/memory/src

tests:
	./gen_test_runner.awk test/rent_tests.c >test/test_runner.c
	./gen_all_tests.awk test/rent_tests.c >test/all_tests.c
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TEST_TARGET)
	- ./$(TEST_TARGET)

rent: src/rent.c
	mkdir -p bin
	cc src/rent.c -o bin/rent

fix: src/rent_with_fix.c
	mkdir -p bin
	cc src/rent_with_fix.c -o bin/rent

clean:
	rm bin/*

