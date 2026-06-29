// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <gtest/gtest.h>

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <tdb/AreaModel.h>
#include <tdb/AreaManager.h>

#define CURRENT_TEST_NAME (::testing::UnitTest::GetInstance()->current_test_info()->name())

bool loadInput(tdb::AreaModel& areaDataBase)
{
	std::unordered_map<std::string, std::vector<std::string>> filenames;
	filenames.insert({ "vertices", std::vector<std::string>({ "vertices.csv" }) });
	filenames.insert({ "waypoints", std::vector<std::string>({ "waypoints.csv" }) });
	filenames.insert({ "links", std::vector<std::string>({ "links.csv" }) });
	filenames.insert({ "cells", std::vector<std::string>({ "cells.csv" }) });
	filenames.insert({ "objects", std::vector<std::string>({ "objects.csv" }) });
	std::string areaPath = std::filesystem::current_path().string() + "\\input\\area\\";
	return areaDataBase.loadModel(areaPath, filenames);
}




TEST(trafficdb_test, load_area) {
	tdb::AreaModel areaModel;
	EXPECT_TRUE(loadInput(areaModel));
}



TEST(trafficdb_test, load_traffic) {
	// TODO
	EXPECT_TRUE(true);
}



TEST(trafficdb_test, addGrid)
{
	tdb::AreaModel areaModel;
	EXPECT_TRUE(loadInput(areaModel));
	tdb::AreaManager areaManager(areaModel);
	int numVerticesInitial = areaModel.getVertices().size();
	int numCellsInitial = areaModel.getCells().size();

	sml::Coordinate2D leftBottom = { 0.0, 0.0 };
	sml::Coordinate2D topRight = { 1.0, 1.0 };
	int nlat = 11;
	int nlon = 11;

	// Add a grid
	std::stringstream inputA;
	inputA << "add grid G1 " << leftBottom.lat << " " << leftBottom.lon << " " << topRight.lat << " " << topRight.lon << " " << nlat << " " << nlon;
	EXPECT_TRUE(areaManager.command(inputA));
	EXPECT_TRUE(areaManager.apply());
	EXPECT_TRUE(areaModel.validate());
	EXPECT_TRUE(areaModel.getVertices().size() == numVerticesInitial + nlat * nlon);
	EXPECT_TRUE(areaModel.getCells().size() == numCellsInitial + (nlat - 1) * (nlon - 1));
	std::stringstream inputB;
	inputB << "add grid G1 " << leftBottom.lat << " " << leftBottom.lon << " " << topRight.lat << " " << topRight.lon << " " << nlat << " " << nlon;
	EXPECT_TRUE(areaManager.command(inputB));	// Give the command to add the grid again
	EXPECT_FALSE(areaManager.apply());			// It is not allowed to add the same grid twice
}




TEST(trafficdb_test, removeGrid)
{
	tdb::AreaModel areaModel;
	EXPECT_TRUE(loadInput(areaModel));
	int numVerticesInput = areaModel.getVertices().size();
	tdb::AreaManager areaManager(areaModel);
	std::stringstream commandA("remove grid G123");
	EXPECT_TRUE(areaManager.command(commandA));		// Trying to remove non-existing grid
	EXPECT_FALSE(areaManager.apply());				// Which is not allowed
	
	// Add a grid to test the relabling of the added grid
	sml::Coordinate2D leftBottom = { 0.0, 0.0 };
	sml::Coordinate2D topRight = { 1.0, 1.0 };
	int nlat = 11;
	int nlon = 11;
	std::stringstream commandB;
	commandB << "add grid G1 " << leftBottom.lat << " " << leftBottom.lon << " " << topRight.lat << " " << topRight.lon << " " << nlat << " " << nlon;
	EXPECT_TRUE(areaManager.command(commandB));
	EXPECT_TRUE(areaManager.apply());
	
	// Remove the initial grid
	std::stringstream commandC("remove grid G0");
	EXPECT_TRUE(areaManager.command(commandC));
	EXPECT_TRUE(areaManager.apply());
	EXPECT_TRUE(areaModel.getVertices().size() == numVerticesInput + 121 - 9); // new 11 x 11 grid added and removed 9 vertices from the G0 grid
	EXPECT_TRUE(areaModel.getCells().size() == 100);
}



TEST(trafficdb_test, removeCell)
{

}



TEST(trafficdb_test, addCell)
{

}



//TEST(trafficdb_test, find_intersection_with_dot_angle_5) {
//	auto th = 5.0;
//	auto thr = th * GeoUtils::toRadian;
//	auto cosTh = cos(thr);
//	auto sinTh = sin(thr);
//
//	auto A = Point2D{ 0,0 };
//	auto B = Point2D{ 1,0 };
//	auto C = Point2D{ cosTh , sinTh };
//	auto D = Point2D{ cosTh+B.x, sinTh+B.y };
//
//
//	//A->B and B->D
//	auto AB_BD = GeoUtils::calculateIntersectionAngleDotProduct(A, B, B, D);
//	EXPECT_NEAR_FLOAT(AB_BD, th);
//
//	//A->B and C->B
//	auto AB_DB = GeoUtils::calculateIntersectionAngleDotProduct(A, B, D, B);
//	EXPECT_NEAR_FLOAT(AB_DB, 180-th);
//
//	//A->B and A->C
//	auto AB_AC = GeoUtils::calculateIntersectionAngleDotProduct(A, B, A, C);
//	EXPECT_NEAR_FLOAT(AB_AC, th);
//
//	//A->B and C->A
//	auto AB_CA = GeoUtils::calculateIntersectionAngleDotProduct(A, B, C, A);
//	EXPECT_NEAR_FLOAT(AB_CA, 180 - th);
//
//}
//
//
//TEST(trafficdb_test, find_intersection_with_dot_angle_30) {
//	auto th = 30.0;
//	auto thr = th * std::numbers::pi / 180.0;
//	auto cosTh = cos(thr);
//	auto sinTh = sin(thr);
//
//	auto A = Point2D{ 0,0 };
//	auto B = Point2D{ 1,0 };
//	auto C = Point2D{ cosTh, sinTh };
//
//	//from A->B to A->C 
//	auto AB_AC = GeoUtils::calculateIntersectionAngleDotProduct(A, B, A, C);
//	EXPECT_DOUBLE_EQ(AB_AC, th);
//
//	//from A->B to C->A
//	auto AB_CA = GeoUtils::calculateIntersectionAngleDotProduct(A, B, C, A);
//	EXPECT_DOUBLE_EQ(AB_CA, 180-th);
//
//	//from B->A to C->A
//	auto BA_CA = GeoUtils::calculateIntersectionAngleDotProduct(B, A, C, A);
//	EXPECT_DOUBLE_EQ(BA_CA, th);
//
//	//from B->A to A->C
//	auto BA_AC = GeoUtils::calculateIntersectionAngleDotProduct(B, A, A, C);
//	EXPECT_DOUBLE_EQ(BA_AC, 180-th);
//
//
//	//from A->C to A->B
//	auto AC_AB = GeoUtils::calculateIntersectionAngleDotProduct(A, C, A, B);
//	EXPECT_DOUBLE_EQ(AC_AB, th);
//
//	//from C->A to A->B
//	auto CA_AB = GeoUtils::calculateIntersectionAngleDotProduct(C, A, A, B);
//	EXPECT_DOUBLE_EQ(CA_AB, 180-th);
//
//	//from C->A to B->A
//	auto CA_BA = GeoUtils::calculateIntersectionAngleDotProduct(C, A, B, A);
//	EXPECT_DOUBLE_EQ(CA_BA, th);
//
//	//from A->C to B->A
//	auto AC_BA = GeoUtils::calculateIntersectionAngleDotProduct(A, C, B, A);
//	EXPECT_DOUBLE_EQ(AC_BA, 180-th);
//}
//
//TEST(trafficdb_test, find_intersection_angle_30) {
//	auto th = 30.0;
//	auto thr = th * std::numbers::pi / 180.0;
//	auto cosTh = cos(thr);
//	auto sinTh = sin(thr);
//
//	auto A = Point2D{ 0,0 };
//	auto B = Point2D{ 1,0 };
//	auto C = Point2D{ cosTh, sinTh };
//	
//	//from A->B to A->C 
//	auto AB_AC = GeoUtils::calculateIntersectionAngle(A, B, A, C);
//	EXPECT_DOUBLE_EQ(AB_AC, th);
//
//	//from A->B to C->A
//	auto AB_CA = GeoUtils::calculateIntersectionAngle(A, B, C, A);
//	EXPECT_DOUBLE_EQ(AB_CA, th+180);
//
//	//from B->A to C->A
//	auto BA_CA = GeoUtils::calculateIntersectionAngle(B, A, C, A);
//	EXPECT_DOUBLE_EQ(BA_CA, th );
//
//	//from B->A to A->C
//	auto BA_AC = GeoUtils::calculateIntersectionAngle(B, A, A, C);
//	EXPECT_DOUBLE_EQ(BA_AC, th+180);
//
//
//	//from A->C to A->B
//	auto AC_AB = GeoUtils::calculateIntersectionAngle(A, C, A, B);
//	EXPECT_DOUBLE_EQ(AC_AB, 360-th);
//
//	//from C->A to A->B
//	auto CA_AB = GeoUtils::calculateIntersectionAngle(C, A, A, B);
//	EXPECT_DOUBLE_EQ(CA_AB, 180-th);
//
//	//from C->A to B->A
//	auto CA_BA = GeoUtils::calculateIntersectionAngle(C, A, B, A);
//	EXPECT_DOUBLE_EQ(CA_BA, 360-th);
//
//	//from A->C to B->A
//	auto AC_BA = GeoUtils::calculateIntersectionAngle(A, C, B, A);
//	EXPECT_DOUBLE_EQ(AC_BA, 180 - th);
//}
//
//TEST(trafficdb_test, polygon_creation) {
//	Point2D startCenterLine{ 0, 0 };
//	Point2D endCenterLine{ 10, 0 };
//	double halfW = 6;
//
//	Polygon p(startCenterLine, endCenterLine, halfW*2);
//
//	EXPECT_DOUBLE_EQ(p[0].x, 0);
//	EXPECT_DOUBLE_EQ(p[0].y, halfW);
//	EXPECT_DOUBLE_EQ(p[1].x, 10);
//	EXPECT_DOUBLE_EQ(p[1].y, halfW);
//	EXPECT_DOUBLE_EQ(p[2].x, 10);
//	EXPECT_DOUBLE_EQ(p[2].y, -halfW);
//	EXPECT_DOUBLE_EQ(p[3].x, 0);
//	EXPECT_DOUBLE_EQ(p[3].y, -halfW);
//}
//
//TEST(trafficdb_test, polygon_creation_45degree) {
//	Point2D startCenterLine{ 0, 0 };
//	Point2D endCenterLine{ 10, 10 };
//	double halfW = 6;
//
//	Polygon p(startCenterLine, endCenterLine, halfW*2);
//
//	double v= sqrt(halfW * halfW / 2);
//
//	EXPECT_DOUBLE_EQ(p[0].x, -v);
//	EXPECT_DOUBLE_EQ(p[0].y, v);
//	EXPECT_DOUBLE_EQ(p[1].x, 10-v);
//	EXPECT_DOUBLE_EQ(p[1].y, 10+v);
//	EXPECT_DOUBLE_EQ(p[2].x, 10+v);
//	EXPECT_DOUBLE_EQ(p[2].y, 10-v);
//	EXPECT_DOUBLE_EQ(p[3].x, v);
//	EXPECT_DOUBLE_EQ(p[3].y, -v);
//}
//
//TEST(trafficdb_test, polygon_intersect) {
//	Point2D s1{0, 0 };
//	Point2D e1{10, 0 };
//	double w1 = 6;
//	Polygon p1(s1, e1, w1);
//
//	Point2D s2{ 9, 5 };
//	Point2D e2{ 12, 5 };
//	double w2 = 5;
//	Polygon p2(s2, e2, w2);
//	
//	EXPECT_TRUE(p1.doIntersectWith(p2));
//	EXPECT_TRUE(p2.doIntersectWith(p1));
//}
//
//TEST(trafficdb_test, polygon_intersect_not) {
//	Point2D s1{ 0, 0 };
//	Point2D e1{ 10, 0 };
//	double w1 = 6;
//	Polygon p1(s1, e1, w1);
//
//	Point2D s2{ 9, 5 };
//	Point2D e2{ 12, 5 };
//	double w2 = 3;
//	Polygon p2(s2, e2, w2);
//
//	EXPECT_FALSE(p1.doIntersectWith(p2));
//	EXPECT_FALSE(p2.doIntersectWith(p1));
//}
//
//
//TEST(trafficdb_test, Waypoint_Serialize_xml) {
//	// Arrange
//	std::string fname = "Waypoint_Serialize_xml.xml";	
//	WaypointCoord w{"W1", 5.65, 51.98, "Wageningen" };
//
//	// Act
//	// save data to archive
//	{
//		std::ofstream ofs(fname);
//		boost::archive::xml_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(w);
//	}
//
//	// read data from archive
//	WaypointCoord neww;
//	{
//		std::ifstream ifs(fname);
//		boost::archive::xml_iarchive ia(ifs);
//		ia >> BOOST_SERIALIZATION_NVP(neww);
//	}
//
//	// Assert
//	EXPECT_EQ(neww.getUID(), w.getUID());
//	EXPECT_DOUBLE_EQ(neww.getPosition().lat, w.getPosition().lat);
//	EXPECT_DOUBLE_EQ(neww.getPosition().lon, w.getPosition().lon);
//	EXPECT_EQ(neww.getName(), w.getName());
//}
//
//TEST(trafficdb_test, Waypoint_Serialize_csv) {
//	// Arrange
//	std::string fname = "Waypoint_Serialize_csv.csv";
//	const WaypointCoord w{ "W1", 5.65, 51.98, "Wageningen" };
//
//	// Act
//	// save data to archive
//	{
//		std::ofstream ofs(fname);
//		boost::archive::csv_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(w);
//	}
//	// read data from archive
//	WaypointCoord neww;
//	{
//		std::ifstream ifs(fname);
//		boost::archive::csv_iarchive ia(ifs);
//		ia >> BOOST_SERIALIZATION_NVP(neww);
//	}
//
//	// Assert
//	EXPECT_EQ(neww.getUID(), w.getUID());
//	//TBD: expect double fails after chaning double lat, double lon to class Coordinate{double lat, double lon}
//	// Most likely double fields are threated as float somewhere  in boost serialization of structs
//	//EXPECT_DOUBLE_EQ(neww.position.lat , w.position.lat);
//	//EXPECT_DOUBLE_EQ(neww.position.lon, w.position.lon);
//	EXPECT_FLOAT_EQ(neww.getPosition().lat, w.getPosition().lat);
//	EXPECT_FLOAT_EQ(neww.getPosition().lon, w.getPosition().lon);
//	EXPECT_EQ(neww.getName(), w.getName());
//}
//
//TEST(trafficdb_test, Waypoint_Serialize_csv_with_header) {
//	// Arrange
//	std::string fname = "input/Waypoint_Serialize_csv_with_header.csv";	
//	const WaypointCoord w{ "W1", 5.65, 51.98, "Wageningen" };
//
//	std::string header = "uid,lat,lon,name";
//	// Act
//	// save data to archive
//	{
//		std::ofstream ofs(fname);
//		boost::archive::csv_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(header);
//
//		oa << BOOST_SERIALIZATION_NVP(w);
//		oa << BOOST_SERIALIZATION_NVP(w);
//		oa << BOOST_SERIALIZATION_NVP(w) << BOOST_SERIALIZATION_NVP(w);
//	}
//
//	//
//	// read data from archive
//	WaypointCoord neww;
//	{
//		std::ifstream ifs(fname);
//		boost::archive::csv_iarchive ia(ifs);
//		ia >> BOOST_SERIALIZATION_NVP(header); //throw header away
//		ia >> BOOST_SERIALIZATION_NVP(neww);
//		//TBD: verify that we have all the serialzed waypoints in the file
//	}
//
//	// Assert
//	EXPECT_EQ(neww.getUID(), w.getUID());
//	//EXPECT_DOUBLE_EQ(neww.position.lat, w.position.lat);
//	//EXPECT_DOUBLE_EQ(neww.position.lon, w.position.lon);
//	EXPECT_FLOAT_EQ(neww.getPosition().lat, w.getPosition().lat);
//	EXPECT_FLOAT_EQ(neww.getPosition().lon, w.getPosition().lon);
//	EXPECT_EQ(neww.getName(), w.getName());
//}
//
//
////TBD: Add a serialization test for each data table
//
//template<class T>
//T ReadFile(std::string f_name) {
//	std::string header;
//	std::string path = "./input/Rotterdam/";
//	std::string f_path = path + f_name;
//	std::ifstream ifs(f_path);
//	boost::archive::csv_iarchive ia(ifs);
//	ia >> header; //throw header away
//	T ret_;
//	ia >> ret_;
//	return ret_;
//}
//
//struct UidPairHash {
//	template <class T1, class T2>
//	std::size_t operator () (const std::pair<T1, T2>& p) const {
//		auto h1 = std::hash<T1>{}(p.first);
//		auto h2 = std::hash<T2>{}(p.second);
//		return h1 ^ h2;
//	}
//};
//
//struct UidPairEqual {
//	template <class T1, class T2>
//	bool operator () (const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2) const {
//		return (p1.first == p2.first && p1.second == p2.second) ||
//			(p1.first == p2.second && p1.second == p2.first);
//	}
//};
//
//
//class Right {
//	public:
//		using UidType = int;
//		Right(UidType uid) : uid_(uid) {}
//		UidType getUID() const { return uid_;}
//	private:
//		UidType uid_;
//};
//
//
//TEST(leftmerge_test, single_merge) {
//
//	DataTable<Right> rights = {
//		std::make_shared<Right>(1),
//		std::make_shared<Right>(2),
//		std::make_shared<Right>(3),
//		std::make_shared<Right>(2)
//	};
//
//	Right::UidType leftOn = 2;
//
//	DataTable<Right> leftMergeList;
//
//	ILeftMergable<Right> mergable;
//
//	mergable.leftMerge(rights, leftOn, leftMergeList);
//
//	EXPECT_EQ(leftMergeList.size(), 2);
//	EXPECT_EQ(leftMergeList[0]->getUID(), 2);
//	EXPECT_EQ(leftMergeList[1]->getUID(), 2);
//}
//
//TEST(leftmerge_test, no_merge) {
//
//	DataTable<Right> rights = {
//		std::make_shared<Right>(1),
//		std::make_shared<Right>(2),
//		std::make_shared<Right>(3)
//	};
//
//	Right::UidType leftOn = 4;
//	DataTable<Right> leftMergeList;
//	ILeftMergable<Right> mergable;
//
//
//	mergable.leftMerge(rights, leftOn, leftMergeList);
//	EXPECT_EQ(leftMergeList.size(), 0);
//}
//
//TEST(leftmerge_test, single_fetch) {
//
//	DataTable<Right> rights = {
//		std::make_shared<Right>(1),
//		std::make_shared<Right>(2),
//		std::make_shared<Right>(3),
//	};
//
//	Right::UidType leftOn = 2;
//
//	Data<Right> leftMergeType;
//
//	ILeftMergable<Right> mergable;
//
//	mergable.leftMerge(rights, leftOn, leftMergeType);
//
//	EXPECT_EQ(leftMergeType->getUID(), 2);
//}
//
//TEST(leftmerge_test, single_fetch_no_match) {
//	DataTable<Right> rights = {
//		std::make_shared<Right>(1),
//		std::make_shared<Right>(2),
//	};
//
//	Right::UidType leftOn = 3;
//
//	Data<Right> leftMergeType;
//
//	ILeftMergable<Right> mergable;
//
//	mergable.leftMerge(rights, leftOn, leftMergeType);
//
//	EXPECT_EQ(leftMergeType, nullptr);
//}
//
//
//
//TEST(trafficdb_test, load_roterdam_traffic_db) {
//	//TBD: this is not an actual unit test, but a playground to put traffic db implementation together
//	// This unit test should be removed once encounter model module is in place
//	using namespace std;
//	auto vecW = ReadFile<DataTable<WaypointCoord>>("Waypoints.csv");
//	auto vecL = ReadFile<DataTable<Link>>("Links.csv");
//	auto vecTN = ReadFile<DataTable<TrafficNetwork>>("TrafficNetworks.csv");
//	auto vecSC = ReadFile<DataTable<ShipCategory>>("ShipCategories.csv");
//	auto vecHS = ReadFile<DataTable<HarmonicSpeed>>("HarmonicSpeeds.csv");
//	auto vecSL = ReadFile<DataTable<ShipLink>>("Ship_Links.csv");
//	auto vecDB = ReadFile<DataTable<TrafficDB>>("TrafficDBs.csv");
//
//	//db<-network
//	MergeDataTable<TrafficDB::UidType, TrafficNetwork> leftmergedTN;
//	for (const auto& db : vecDB) {
//		auto temp = db->merge(vecTN);
//		leftmergedTN[db->getUID()].insert(leftmergedTN[db->getUID()].end(), temp.begin(), temp.end());
//	}
//
//	//network<-link
//	MergeDataTable<TrafficNetwork::UidType, Link> leftmergedL;
//	for (auto& pair : leftmergedTN) {
//		//pair.first is the traffic database uid
//		for (auto& lm_tn : pair.second) {
//			//lm_tn is the left merged traffic data table element
//			auto temp = lm_tn->merge(vecL);
//			leftmergedL[lm_tn->getUID()].insert(leftmergedL[lm_tn->getUID()].end(), temp.begin(), temp.end());
//		}
//	}
//
//	//link<-waypoint
//	for (auto& pair : leftmergedL) {
//		//pair.first is the link uid
//		for (auto& lm_l : pair.second) {
//			//lm_l is the left merged link data table element
//			lm_l->merge(vecW);
//			//do not need a data table for merged wapoints, start and end waypoints are kep in class member of links
//		}
//	}
//
//	//TBD: A temporary type to reflect the organization of the calculated encounters
//	using DBEncounter = std::unordered_map<TrafficDB::UidType,
//		std::unordered_map<TrafficNetwork::UidType,    //Encounters
//			std::unordered_map<std::pair<Link::UidType, Link::UidType>,//EncouterLinkPairs
//					std::unordered_map< std::pair<ShipCategory::UidType, ShipCategory::UidType>, //EncouterShipPairs
//		             double,
//		            UidPairHash, UidPairEqual >
//		    , UidPairHash, UidPairEqual>>>;
//
//	DBEncounter encounters;
//	//For all the traffic DBs under examination
//	for (const auto& db : vecDB) {
//		//For all the networks of a traffic DB
//		for (const auto& tn : leftmergedTN[db->getUID()]) {
//
//			//TBD: Assuming for now all links are crossing
//			for (const auto& linkA : leftmergedL[tn->getUID()]) {
//				for (const auto& linkB : leftmergedL[tn->getUID()]) {
//					if (linkA != linkB)
//					{
//						//TBD: Also check if linkB and linkA already calculated -> use UidPairEqual
//
//						//get ship categories on the link A
//						DataTable<ShipLink> ShipsOnLinkA;
//						std::copy_if(vecSL.begin(), vecSL.end(),
//							std::back_inserter(ShipsOnLinkA), [uid = linkA->getUID()](const Data<ShipLink>& item) {
//								return item->getLinkUid() == uid;
//							});
//
//						for (const auto& scA : ShipsOnLinkA) {
//							//Find the harmonic speed of the ship category
//							auto findSpeed = std::find_if(vecHS.begin(), vecHS.end(), [uid = scA->getShipUid()](const Data<HarmonicSpeed>& obj) {
//								return obj->getUid() == uid;
//								});
//							if (findSpeed != vecHS.end()) {
//								//Get speed and count on for the current category on link A, let's call it as i
//								auto harmonic_i = findSpeed->get();
//								auto Vi = harmonic_i->getSog();
//								auto ni = scA->getCount();
//
//								//get ship categories on the link B
//								DataTable<ShipLink> ShipsOnLinkB;
//								std::copy_if(vecSL.begin(), vecSL.end(),
//									std::back_inserter(ShipsOnLinkB), [uid = linkB->getUID()](const Data<ShipLink>& item) {
//										return item->getLinkUid() == uid;
//									});
//								for (const auto& scB : ShipsOnLinkB) {
//
//									//TBD: do not re calculate already calcualted category pairs
//
//									//Find the harmonic speed of the ship categories
//									auto findSpeed = std::find_if(vecHS.begin(), vecHS.end(), [uid = scB->getShipUid()](const Data<HarmonicSpeed>& obj) {
//										return obj->getUid() == uid;
//										});
//									if (findSpeed != vecHS.end()) {
//										//Get speed and count on for the current category on link B, let's call it as j
//										auto harmonic_j = findSpeed->get();
//										auto Vj = harmonic_j->getSog();
//										auto nj = scB->getCount();
//
//										constexpr double R = 463; // 0.25 nm -> TBD: will be discussed further
//
//										double th = 30.0 * (3.1415 / 180.0); //TBD: will be calculated
//										double Vij = std::sqrt(Vi * Vi + Vj * Vj - (2 * Vi * Vj * std::cos(th)));
//										auto encounter = ((2 * R) * ((ni * nj) / (Vi * Vj)) * Vij);
//										encounters[db->getUID()][tn->getUID()][std::make_pair(linkA->getUID(), linkB->getUID())][std::make_pair(scA->getShipUid(), scB->getShipUid())] = encounter;
//									}//TBD: if we cannot find the harmonic speed?
//								}
//							} //TBD: if we cannot find the harmonic speed?
//						}
//					}
//				}
//			}
//		}
//	}
//
//
//	std::cout << "End of : " << CURRENT_TEST_NAME << "\n";
//}
//
//TEST(trafficdb_test, load_DataTables) {
//	DataTables tables;
//	tables.readAllTables("./input/Rotterdam/");
//}
//
//TEST(trafficdb_test, create_area) {
//
//
//	Coordinate2D rotterdamNW{ .lon = 4.045295, .lat = 52.021433 }; //A random point on north-west of the Rotterdam harbour.
//	Coordinate2D rotterdamSE{ .lon = 4.226532, .lat = 51.904000 }; //A random point on sout-east of the Rotterdam harbour.
//	Area area{ "Area001", rotterdamNW , rotterdamSE, {LonDir::West, LatDir::South} };
//
//	//Test updating start and end points for different origins
//	auto area_start = area.getOriginCoord();
//	auto area_end = area.getEndCoord();
//
//	EXPECT_EQ(area_start.lat, rotterdamSE.lat); // most south
//	EXPECT_EQ(area_start.lon, rotterdamNW.lon); // most west
//	EXPECT_EQ(area_end.lat, rotterdamNW.lat); // most north
//	EXPECT_EQ(area_end.lon, rotterdamSE.lon); // most east
//
//	//Test setting cell numbers & cells are created
//	auto dimension = area.updateCells(10, 20);
//	
//	std::cout << std::format("extend x/lon: {}, y/lat: {}\n", area.getExtent().x, area.getExtent().y);
//	std::cout << std::format("dimension x/lon: {} , y/lat: {}\n", dimension.x, dimension.y);
//}
//
//TEST(trafficdb_test, create_link_cells) {
//	std::string folder = "./input/Rotterdam/";
//	DataTables tables;
//	tables.readAllTables(folder);
//
//	Coordinate2D rotterdamNW{ .lon = 4.045295, .lat = 52.021433 }; //A random point on north-west of the Rotterdam harbour.
//	Coordinate2D rotterdamSE{ .lon = 4.226532, .lat = 51.904000 }; //A random point on sout-east of the Rotterdam harbour.
//	Area area{ "A001", rotterdamNW , rotterdamSE, {LonDir::West, LatDir::South} };
//	area.updateCells(10, 10);
//
//	//
//	{
//		std:string header = "uid,sart_lon,start_lat,end_lon,end_lat,dir_from_lon,dir_from_lat,extend_x,extend_y,cellsize_x,cellsize_y";
//		std::ofstream ofs(folder + "Area.csv");
//		boost::archive::csv_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(header);
//		oa << BOOST_SERIALIZATION_NVP(area);
//	}
//
//
//
//	DataTable<WaypointPoint> areaWaypoints;
//
//	//Create Table: Convert waypoints lat lon to -> distance table regarding an area	
//	for (const auto& w : tables.waypoints) {
//		//auto point = GeoUtils::toPoint(area.start, w->position);
//		areaWaypoints.emplace_back(std::make_shared<WaypointPoint>(w->getUID(), GeoUtils::toPoint(area.getOriginCoord(), w->getPosition()), w->getName()));
//	}
//	// save data to archive	
//	{
//		std::string header = "uid,x,y,name";
//		std::ofstream ofs(folder + "AreaWaypoints.csv");
//		boost::archive::csv_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(header);
//		oa << BOOST_SERIALIZATION_NVP(areaWaypoints);
//	}
//
//
//	//Grid<CellObj> cellObjsWaypoint{ area.grid.getCols(), area.grid.getRows() };
//	SparseGrid<CellObj> cellObjsWaypoint;
//	double meterX = 0.0;
//	double meterY = 0.0;
//	bool wpFound = false;
//	for (const auto& aw : areaWaypoints) {
//		size_t x = static_cast<size_t>(aw->getPosition().x / area.getCellSizeMeter().x);
//		size_t y = static_cast<size_t>(aw->getPosition().y / area.getCellSizeMeter().y);
//		//cellObjsWaypoint(x, y).getObjUIDS().push_back(aw->getUID());
//		cellObjsWaypoint(x, y).add(aw->getUID());
//	}
//
//	IndexedDataTable<WaypointPoint, WaypointPoint::UidType> indexedAreaWp;
//	//Index area waypoints that are in cartesian coordinate system
//	indexedAreaWp.add(areaWaypoints);
//
//	SparseGrid<CellObj> cellObjsLinks;
//	//Go through all links to attach them to cells that they pass through
//	for (const auto& l : tables.links) {
//
//		//Get start and end WPs of the link
//		auto p1 = indexedAreaWp.get(l->getStart());
//		auto p2 = indexedAreaWp.get(l->getEnd());
//		//Find cells of WPs
//		auto [cellP1_x, cellP1_y] = GeoUtils::getCellOfPoint(p1->getPosition(), area.getCellSizeMeter());
//		auto [cellP2_x, cellP2_y] = GeoUtils::getCellOfPoint(p2->getPosition(), area.getCellSizeMeter());
//
//		//TBD: could add link to wp cells already here (optimization possibility).
//
//		//We are creating a bounding box by sorting cell indexes
//		if (cellP1_x > cellP2_x) {
//			std::swap(cellP1_x, cellP2_x);
//		}
//
//		if (cellP1_y > cellP2_y) {
//			std::swap(cellP1_y, cellP2_y);
//		}
//
//		//For each possible cell check if the link is passing through
//		for (auto x = cellP1_x; x <= cellP2_x; ++x) {
//			for (auto y = cellP1_y; y <= cellP2_y; ++y) {
//				if (GeoUtils::doesLineInOrIntersectCell(p1->getPosition(), p2->getPosition(), area.getGrid()(x, y), area.getCellSizeMeter())) {
//					cellObjsLinks(x, y).add(l->getUID());
//				}
//			}
//		}
//		std::cout << "Get points\n";
//	}
//
//	//Now we have, links attached to cells save them for encounter calculations		
//	{
//		std::string header = "x,y,link_uid";
//		std::ofstream ofs(folder + "SparseGrid_Links.csv");
//		boost::archive::csv_oarchive oa(ofs);
//		oa << BOOST_SERIALIZATION_NVP(header);
//		oa << BOOST_SERIALIZATION_NVP(cellObjsLinks);
//	}
//
//
//	// read data from archive
//	SparseGrid<CellObj> cellObjsLinksIn;
//	{
//		std::string header;
//		std::ifstream ifs(folder + "SparseGrid_Links.csv");
//		boost::archive::csv_iarchive ia(ifs);
//		ia >> BOOST_SERIALIZATION_NVP(header); //throw header away
//		ia >> BOOST_SERIALIZATION_NVP(cellObjsLinksIn);
//		//TBD: verify that we have all the serialized waypoints in the file
//		std::cout << "End of test case\n";
//	}
//
//
//	std::cout << "End of test case\n";
//}
//
//TEST(trafficdb_test, create_db_with_area_calc_encounter) {
//	//TBD: Go through all cells and calculate encounter
//}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(print_time) = true;
	::testing::GTEST_FLAG(output) = "xml:results.xml";
	return RUN_ALL_TESTS();
}

