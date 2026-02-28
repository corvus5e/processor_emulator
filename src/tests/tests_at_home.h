/* 
 * Test framework "Tests at Home"
 * */

#ifndef _TESTS_AT_HOME_H_
#define _TESTS_AT_HOME_H_

#include <stdio.h>

#define COLOR_NORMAL "\x1B[0m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"

enum TestResult {
	TEST_PASSED,
	TEST_FAILED
};

typedef enum TestResult (*test_case_ptr)(void);

typedef struct TestInfo {
	test_case_ptr fun_ptr;
	const char *name;
} TestInfo;

#define T TestInfo
#include "common/vector.h"
#undef T

typedef const char* str;
#define T str
#include "common/vector.h"
#undef T


static struct vec_TestInfo test_cases_list;
void __attribute__((constructor(101))) test_at_home_init() {
	vec_init_TestInfo(&test_cases_list);
}

int test_at_home_run() {
	struct vec_str failed_tests;
	vec_init_str(&failed_tests);

	int status = 0;
	for(int i = 0; i < test_cases_list.size; ++i) {
		TestInfo *info = vec_at_TestInfo(&test_cases_list, i);
		printf("=== Running [%s]\n", info->name);
		enum TestResult test_result = info->fun_ptr();
		if(test_result == TEST_PASSED){
			printf("%s[PASSED]%s\n", COLOR_GREEN, COLOR_NORMAL);
		}
		else {
			printf("%s[FAILED]%s\n", COLOR_RED, COLOR_NORMAL);
			vec_push_back_str(&failed_tests, info->name);
			status = TEST_FAILED;
		}
	}

	for(int i = 0; i < failed_tests.size; ++i) {
		printf("%s%s [FAILED]%s\n", COLOR_RED, *vec_at_str(&failed_tests, i), COLOR_NORMAL);
	}

	vec_free_str(&failed_tests);

	return status;
}

#define STR(x) #x
#define PREFIX(prefix, word) prefix ## word
#define VOID_ARGS (void)
#define TEST(test_name) \
	enum TestResult PREFIX(TEST_, test_name)(void); \
	static void __attribute__((constructor(102))) PREFIX(register_, test_name)(void){ \
		TestInfo ti; \
		ti.name = STR(test_name);\
		ti.fun_ptr = PREFIX(TEST_, test_name); \
		vec_push_back_TestInfo(&test_cases_list, ti); } \
	enum TestResult PREFIX(TEST_, test_name)(void) \

#define TEST_DISABLED(test_name) \
	enum TestResult PREFIX(TEST_, test_name)(void); \
	enum TestResult PREFIX(TEST_, test_name)(void) \

#define EXPECT_EQ(a, b) if ((a) != (b)) \
{ \
	printf("== Not equal: %d != %d\n", (a), (b)); \
	return TEST_FAILED; \
}


#endif
