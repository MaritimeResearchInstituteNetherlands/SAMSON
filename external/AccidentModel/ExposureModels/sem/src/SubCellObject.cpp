// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#include <sem/SubCellObject.h>
#include <sem/CellObject.h>
#include <sml/AngleBounds.h>
#include <sml/Line.h>

namespace sem 
{

SubCellObject::SubCellObject(const sml::Vector2d& position, double area, const CellObject& cellObject)
	: m_cellObject(cellObject)
	, m_position(position)
	, m_area(area)
{
}



void SubCellObject::setup()
{
	// Input data
	constexpr double maxBinSize = GeneralModelData::maxAngularBinSize();
	const auto& nonShieldingObjects = m_cellObject.getNonShieldingObjects();
	const std::vector<std::vector<sml::Vector2d>>& objectVertices = m_cellObject.getVerticesNonShieldingObjects();
	
	// Compute the bounds of the objects
	m_bounds.resize(nonShieldingObjects.size());
	for (size_t i = 0; i < m_bounds.size(); i++) {
		const sim::Object* object = nonShieldingObjects[i];
		const std::vector<sml::Vector2d> &vertices = objectVertices[i]; // vertices of a polyline
		for (size_t index = 0; index < vertices.size() - 1; index++) {
			sml::AngleBounds newBound = sml::computePointBound(m_position, vertices[index], vertices[index + 1]);
			m_bounds[i] = sml::mergeBounds(m_bounds[i], newBound);
		}
		// Convert the bound from object->point to point->object
		m_bounds[i].setLowerBound(sml::normalisedPositiveAngles(m_bounds[i].getLowerBound() + std::numbers::pi));
		m_bounds[i].setUpperBound(sml::normalisedPositiveAngles(m_bounds[i].getUpperBound() + std::numbers::pi));
	}

	// Discretise the bounds
	m_totalBins = 0;
	m_numBins.resize(nonShieldingObjects.size());
	for (size_t i = 0; i < nonShieldingObjects.size(); i++) {
		size_t numBins = std::max(1, (int)std::ceil(m_bounds[i].getExtent() / maxBinSize));
		m_numBins[i] = numBins;
		m_totalBins += numBins;
	}

	computeProjections();
}



void SubCellObject::computeProjections()
{
	// Grab LinkObject data
	const auto& objects = m_cellObject.getNonShieldingObjects();
	const auto& objectVertices = m_cellObject.getVerticesNonShieldingObjects();
	
	// Non shielding projections
	m_projections.reserve(m_totalBins);
	size_t index = 0;
	for (size_t i = 0; i < objects.size(); i++) {
		const size_t numBins = m_numBins[i];
		const sml::AngleBounds& bound = m_bounds[i];
		double binSize = bound.getExtent() / numBins;
		for (size_t j = 0; j < numBins; j++) {
			// Create local bin
			double lowerBound = sml::normalisedPositiveAngles(bound.getLowerBound() + binSize * j);
			double upperBound = sml::normalisedPositiveAngles(bound.getLowerBound() + binSize * (j + 1));
			double driftAngle = sml::normalisedPositiveAngles(bound.getLowerBound() + binSize * (0.5 + j));

			// Compute the projection distance
			const std::vector<sml::Vector2d>& vertices = objectVertices[i];
			sml::Vector2d driftDir(std::cos(driftAngle), std::sin(driftAngle));
			double distance = DBL_MAX;
			for (size_t index = 0; index < vertices.size() - 1; index++) {
				sml::Line edge(vertices[index], vertices[index + 1]);
				sml::Vector2d intersectionPoint;
				if (edge.projectPointOnLine(m_position, driftDir, intersectionPoint)) {
					distance = std::min(distance, (intersectionPoint - m_position).norm());
				}
			}
			if (distance > 0.9 * DBL_MAX) {
				LogError("Internal error during projection calculation of CellObject.");
			}
			
			// Create a projection
			Projection projection;
			projection.bin.setLowerBound(lowerBound);
			projection.bin.setUpperBound(upperBound);
			projection.bin.setExtent(binSize);
			projection.objectDistances.emplace_back(distance);
			projection.objectIndices.emplace_back(objects[i]->getIndex());
			projection.localObjectIndices.emplace_back(i);
			m_projections.push_back(std::move(projection));
				
		}
	}
}



void SubCellObject::cleanUp()
{
	m_bounds = std::vector<sml::AngleBounds>();
	m_numBins = std::vector<size_t>();
	m_projections = std::vector<Projection>();
}



void SubCellObject::computeExposuresDrift(const double elevationMin, const double elevationMax, const DriftModel& driftModel, CellMeetingData& meetingData)
{
	// Collect the meetings between ship i and its objects from the projections
	for (size_t iProjection = 0; iProjection < m_projections.size(); iProjection++) {
		const auto& projection = m_projections[iProjection];
		int index = getDistanceIndex(projection.objectIndices, projection.objectDistances, elevationMin, elevationMax);
		if (index >= 0) {
			double targetDistance = projection.objectDistances[index];
			// Compute the exposure
			size_t localObjectIndex = projection.localObjectIndices[index];
			double probabilityDrift = driftModel.computeExposureProbability(projection.bin, targetDistance);
			meetingData.exposures[localObjectIndex] += probabilityDrift * m_area;
			meetingData.objectIndex[localObjectIndex] = projection.objectIndices[index];
		}
	}
}



void SubCellObject::computeExposuresRam(const double elevationMin, const double elevationMax, const double direction, CellMeetingData& meetingData)
{
	for (size_t iProjection = 0; iProjection < m_projections.size(); iProjection++) {
		const auto& projection = m_projections[iProjection];
		int index = getDistanceIndex(projection.objectIndices, projection.objectDistances, elevationMin, elevationMax);
		if (index >= 0) {
			double targetDistance = projection.objectDistances[index];
			if (isRamProjection(targetDistance, direction, projection)) {
				size_t localObjectIndex = projection.localObjectIndices[index];
				double probabilityRam = computeRamProbability(direction, projection);
				meetingData.exposures[localObjectIndex] += probabilityRam * m_area;
				meetingData.objectIndex[localObjectIndex] = projection.objectIndices[index];
			}
		}
	}
}



void SubCellObject::computeCollisionsDrift(const double elevationMin, const double elevationMax, const DriftModel& driftModel, CellMeetingData& meetingData)
{
	// Collect the meetings between ship i and its objects from the projections
	const std::vector<const sim::Object*>& objects = m_cellObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projections.size(); iProjection++) {
		const auto& projection = m_projections[iProjection];
		int index = getDistanceIndex(projection.objectIndices, projection.objectDistances, elevationMin, elevationMax);
		if (index >= 0) {
			double targetDistance = projection.objectDistances[index];
			if (targetDistance < DBL_MAX) {
				// Compute the exposure
				size_t localObjectIndex = projection.localObjectIndices[index];
				if (m_cellObject.usePassingDistance(objects[localObjectIndex])) {
					targetDistance = std::max(targetDistance, GeneralModelData::minimumPassingDistance());
				}
				double probabilityDrift = driftModel.computeMeasureProbability(projection.bin, targetDistance);
				meetingData.collisions[localObjectIndex] += meetingData.causationFactor * probabilityDrift * m_area;
			}
		}
	}
}



void SubCellObject::computeCollisionsRam(const double elevationMin, const double elevationMax, const double direction, const double length, CellMeetingData& meetingData)
{
	// Collect the meetings between ship i and its objects from the projections
	const std::vector<const sim::Object*>& objects = m_cellObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projections.size(); iProjection++) {
		const auto& projection = m_projections[iProjection];
		int index = getDistanceIndex(projection.objectIndices, projection.objectDistances, elevationMin, elevationMax);
		if (index >= 0) {
			double targetDistance = projection.objectDistances[index];
			if (isRamProjection(targetDistance, direction, projection)) {
				// Compute the measure
				size_t localObjectIndex = projection.localObjectIndices[index];
				double probabilityRam = computeRamProbability(direction, projection);
				double probabilityMeasure = 1.0;
				// Pilot assistance
				// TODO SEM Model is not yet finalised on this part

				// Avoidance function
				if (m_cellObject.useAvoidance()) {
					if (m_cellObject.usePassingDistance(objects[localObjectIndex])) {
						targetDistance = std::max(targetDistance, GeneralModelData::minimumPassingDistance());
					}
					probabilityMeasure *= std::exp(-0.1 * targetDistance / length);
				}

				meetingData.collisions[localObjectIndex] += meetingData.causationFactor * probabilityRam * probabilityMeasure * m_area;
				meetingData.objectIndex[localObjectIndex] = projection.objectIndices[index];
			}
		}
	}
}



int SubCellObject::getDistanceIndex(const std::vector<size_t>& objectIndices, const std::vector<double>& objectDistances, const double elevationMin, const double elevationMax) const
{
	double minDistance = DBL_MAX;
	int localIndex = -1;
	for (size_t i = 0; i < objectIndices.size(); i++) {
		if (objectDistances[i] < minDistance) {
			minDistance = objectDistances[i];
			localIndex = i;
		}
	}
	return localIndex;
}



double SubCellObject::computeRamProbability(const double direction, Projection projection)
{
	double probabilityRam = projection.bin.getExtent() / (2.0 * std::numbers::pi); // (TODO define this constant somewhere to avoid division)
	if (!std::isnan(direction)) {
		// Ship has a single direction, use the ram distribution probability;
		double localLowerBound = sml::normalisedBalancedAngles(projection.bin.getLowerBound() - direction);
		double localUpperBound = sml::normalisedBalancedAngles(projection.bin.getUpperBound() - direction);
		probabilityRam = sml::cdf_normalDistribution(localUpperBound, 0.0, GeneralModelData::ramSigma())
			- sml::cdf_normalDistribution(localLowerBound, 0.0, GeneralModelData::ramSigma());
	}
	return probabilityRam;
}



bool SubCellObject::isRamProjection(const double targetDistance, const double direction, const Projection& projection)
{
	bool valid = true;
	if (targetDistance > GeneralModelData::ramRadius()) {
		valid = false;
	}

	if (!std::isnan(direction)) {
		sml::AngleBounds ramBounds(direction - 3 * GeneralModelData::ramSigma(), direction + 3 * GeneralModelData::ramSigma(), 6 * GeneralModelData::ramSigma());
		if (!projection.bin.hasOverlapWith(ramBounds)) {
			valid = false;
		}
	}

	return valid;
}
} // namespace sem
