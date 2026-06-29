// Copyright (c), MARIN, Maritime Research Institute Netherlands. All rights reserved.
// License: pending

#include <gtest/gtest.h>

//#include "ShipShip/RB_RB/EncLinkPair.h"

//#include "yaml-cpp/yaml.h"


//using namespace tdb;

//TEST(SamsonTests, SampleTest) {    
//    YAML::Node config = YAML::LoadFile("../Example/config.yaml");
//    
//    std::cout << "Config File:\n" << config;
//    
//    std::vector<ShipShip::EncounterAngleType>  encAngleTypes;
//
//    for (const auto& encAngleTypeYaml : config["routeBound"]["encounterTypes"]) {    
//        encAngleTypes.emplace_back(encAngleTypeYaml["name"].as<std::string>(),
//            encAngleTypeYaml["startAngle"].as<double>(),
//            encAngleTypeYaml["endAngle"].as<double>());
//    }
//
//	ShipShip::EncounterAngleTypeClassifier classifier(encAngleTypes);
//
//    EXPECT_EQ(classifier.classify(0), ShipShip::EncounterType::headOn);
//    EXPECT_EQ(classifier.classify(90), ShipShip::EncounterType::crossing);
//    EXPECT_EQ(classifier.classify(180), ShipShip::EncounterType::overTaking);
//    EXPECT_EQ(classifier.classify(270), ShipShip::EncounterType::crossing);
//
//    EXPECT_EQ(classifier.classify(356), ShipShip::EncounterType::headOn);
//    //Because crossing is added before head-on. 
//    EXPECT_EQ(classifier.classify(355), ShipShip::EncounterType::crossing);
//    //Because overtaking is added before the 2nd crossing. 
//    EXPECT_EQ(classifier.classify(247.5), ShipShip::EncounterType::overTaking);
//
//}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
