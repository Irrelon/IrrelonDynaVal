#include <string>
#include <unity.h>
#include "Irrelon/DynaVal.h"
#include "Irrelon/dynaLog.h"

void test_object_assignment() {
	try {
		Irrelon::DynaVal obj;
		obj.becomeObject();
		obj["foo"] =  "bar";
		obj["baz"] =  123;
		obj["qux"] =  123.456;
		obj["quux"] =  true;
		obj["corge"] =  false;

		const std::string val = obj.toJson();

		TEST_ASSERT_EQUAL_STRING("{\"corge\":false,\"qux\":123.456,\"quux\":true,\"baz\":123,\"foo\":\"bar\"}", val.c_str());
	} catch (const std::exception &e) {
		Irrelon::dynaLogLn("Exception", e.what());
		TEST_FAIL_MESSAGE(e.what());
	}
}

void test_array_assignment() {
	try {
		Irrelon::DynaVal obj;
		obj.becomeArray();
		obj.push("bar");
		obj.push(123);
		obj.push(123.456);
		obj.push(true);
		obj.push(false);

		const std::string val = obj.toJson();

		TEST_ASSERT_EQUAL_STRING("[\"bar\",123,123.456,true,false]", val.c_str());
	} catch (const std::exception &e) {
		Irrelon::dynaLogLn("Exception", e.what());
		TEST_FAIL_MESSAGE(e.what());
	}
}

int main() {
	UNITY_BEGIN();
	RUN_TEST(test_object_assignment);
	RUN_TEST(test_array_assignment);
	UNITY_END();
}
