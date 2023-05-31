#include "unity_fixture.h"
#include "unity.h"
#include "unity_memory.h"
#include "stdlib.h"
TEST_GROUP(my_code);

int *an_int;

TEST_SETUP(my_code) {
    UnityMalloc_StartTest(); // see unity/extras/memory/readme.md
    an_int = (int *)malloc(sizeof(int));
}

TEST_TEAR_DOWN(my_code) {
    free(an_int); // commenting this => test fails with : this test leaks
    UnityMalloc_EndTest();
}

TEST(my_code,dummy_test){
    *an_int = 2;
    TEST_ASSERT_EQUAL(4, 2+2);
}
