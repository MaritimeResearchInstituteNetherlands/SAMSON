// Copyright (c), MARIN, Maritime Research Institute Netherlands. All rights reserved.
// License: pending

#include <gtest/gtest.h>
#include "SamsonCore/SamsonCore.h"


// Example test case
TEST(SamsonTests, SampleTest) {
    // Arrange
    
    // Act
    int result = 5;

    // Assert
    EXPECT_EQ(result, 5);    
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
