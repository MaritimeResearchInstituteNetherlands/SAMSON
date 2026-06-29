// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <gtest/gtest.h>

#include <sem/LinkObject.h>
#include <sem/Link.h>
#include <numbers>

#define CURRENT_TEST_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(print_time) = true;
	::testing::GTEST_FLAG(output) = "xml:results.xml";
	return RUN_ALL_TESTS();
}
 
