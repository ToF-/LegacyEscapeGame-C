#include "unity_fixture.h"
#include "unity.h"
#include "unity_memory.h"
#include "stdlib.h"
TEST_GROUP(rent_tests);

int *an_int;

TEST_SETUP(rent_tests) {
    UnityMalloc_StartTest(); // see unity/extras/memory/readme.md
    an_int = (int *)malloc(sizeof(int));
}

TEST_TEAR_DOWN(rent_tests) {
    free(an_int); // commenting this => test fails with : this test leaks
    UnityMalloc_EndTest();
}

TEST(rent_tests,dummy_test){
    *an_int = 2;
    TEST_ASSERT_EQUAL(4, 2+2);
}

TEST(rent_tests,anothor_test) {
    TEST_ASSERT_EQUAL(6,3+3);
}
