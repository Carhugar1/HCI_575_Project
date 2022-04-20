#pragma once
#include <string>
#include <vector>

/* Struct Definitions */
struct Test_Result_s {
	std::string testName;
	bool result;
	std::string errorMessage;
};

/* Type Definitions */

/**
 *	Test Result :
 *		testName: Unique Name for the test case
 *		result: Boolean for the test's result (pass = true; fail = false)
 *		errorMessage: Error message string
 */
typedef struct Test_Result_s TestResult;

/**
 *	Vector of Test Results
 */
typedef std::vector<TestResult> VecTestResult;