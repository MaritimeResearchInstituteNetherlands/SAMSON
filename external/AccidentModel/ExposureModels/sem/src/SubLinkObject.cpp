// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#include <sem/SubLinkObject.h>
#include <sem/LinkObject.h>
#include <sml/Segment.h>
#include <sml/Functions.h>
#include <sml/PolyLine.h>
#include <algorithm>
#include <numbers>

namespace sem {

SubLinkObject::SubLinkObject(const sml::Vector2d p0, const sml::Vector2d p1, const double offset,  const LinkObject& linkObject)
	: m_linkObject(linkObject)
	, m_line(p0, p1)
	, m_offset(offset)
{
}



void SubLinkObject::setup()
{
	// Model parameter
	constexpr double maxBinSize = GeneralModelData::maxAngularBinSize();

	// Link input data
	const auto& nonShieldingObjects = m_linkObject.getNonShieldingObjects();

	gatherObjects();

	// Compute the bounds of the objects
	m_bounds.resize(m_objectIndices.size());
	for (size_t i = 0; i < m_bounds.size(); i++) {
		m_bounds[i] = sml::computeAngleBounds(m_line.getStart(), m_line.getEnd(), *m_vertices[i]);
	}

	computeProjections();

	//m_boundsShielded.resize(shieldingObjects.size());
	//const auto& verticesShieldingObjects = m_linkObject.getVerticesShieldingObjects();
	//for (size_t i = 0; i < shieldingObjects.size(); i++) {
	//	m_boundsShielded[i] = sml::computeAngleBounds(m_line.getStart(), m_line.getEnd(), verticesShieldingObjects[i]);
	//}

	// Discretise each nonshielding object individually by determining the number of bins
	//m_numBins.resize(nonShieldingObjects.size());
	//for (size_t i = 0; i < nonShieldingObjects.size(); i++) {
	//	m_numBins[i] = std::max(1, (int)std::ceil(m_bounds[i].getExtent() / maxBinSize));
	//}

	// Discretise shielding object
	// Note: These angles are the _bin_ angles, not the shooting angles
	//for (size_t i = 0; i < shieldingObjects.size(); i++) {
	//	size_t numBins = std::max(1, (int)std::ceil(m_boundsShielded[i].getExtent() / maxBinSize));
	//	double binSize = m_boundsShielded[i].getExtent() / numBins;
	//	for (size_t j = 0; j < numBins + 1; j++) {
	//		double angle = sml::normalisedBalancedAngles(m_boundsShielded[i].getLowerBound() + j * binSize);
	//		//std::cout << "angle: " << angle << std::endl;
	//		m_binBoundaries.push_back(angle);
	//	}
	//}
	//
	//// Generate a unique set of bin boundaries
	//std::sort(m_binBoundaries.begin(), m_binBoundaries.end());
	//auto last = std::unique(m_binBoundaries.begin(), m_binBoundaries.end());
	//m_binBoundaries.erase(last, m_binBoundaries.end());
}



void SubLinkObject::cleanup()
{
	m_localVertices = std::deque<std::vector<sml::Vector2d>>();
	m_objectIndices = std::vector<int>();
	m_vertices = std::vector<const std::vector<sml::Vector2d>*>();
	m_bounds = std::vector<sml::AngleBounds>();
	m_projectionsDrift = std::vector<ProjectionDrift>();
	m_projectionsRam = std::vector<ProjectionRam>();
}



void SubLinkObject::gatherObjects()
{
	const auto& objects = m_linkObject.getNonShieldingObjects();
	const auto& objectVertices = m_linkObject.getVerticesNonShieldingObjects();
	sml::PolylineClipper clipper;
	for (size_t i = 0; i < objects.size(); i++) {
		// Check intersection
		std::vector<std::vector<sml::Vector2d>> newPolyLines;
		if (clipper.clip(objectVertices[i], m_line, newPolyLines)) {
			LogWarning("Object " << objects[i]->getScheme()->getUID() 
				<< " is clipping with a subLink of link " << m_linkObject.getLink().getLinkScheme()->getUID()
				<< ". Proper handling of this case requires object shielding. Please move your object away from the link traffic.");
			for (auto& vertices: newPolyLines) {
				m_localVertices.push_back(vertices);
				m_objectIndices.push_back(i);
				m_vertices.push_back(&m_localVertices.back());
			}
		}
		else {
			m_objectIndices.push_back(i);
			m_vertices.push_back(&objectVertices[i]);
		}
	}
}



// This function generates a cache of segments which each ship has to evaluate based on its elevation
// Non shielded segments are filtered such that there is no overlap
// Situations with multiple shielding objects will have segment projection with overlapping segments
// This way a specific ship can filter certain objects due to elevation constraints, and after that a filter step is applied
// to generate a unique set of non-overlapping segments
void SubLinkObject::computeProjections()
{
	// Non-shielding projections
	// TODO the current projections assume that the object is a polygon. This is not necessarily true
	computeDriftProjections();
	computeRamProjections();

	// Shielding projections
	// Currently disabled
	// TODO the local index here needs the offset of the non-shield objects
	//if (m_binBoundaries.size() > 0) {
		//for (size_t i = 0; i < m_binBoundaries.size() - 1; i++) {
			//double lowerBound = m_binBoundaries[i];
			//double upperBound = m_binBoundaries[i + 1];
			//double binSize = sml::normalisedPositiveAngles(std::abs(upperBound - lowerBound));
			//double angle = sml::normalisedPositiveAngles(m_binBoundaries[i] + 0.5 * binSize);
			//
			//// Construct a set of applicable objects that have bounds within this bin
			//std::vector<size_t> objectIndices;
			//for (size_t j = 0; j < shieldingObjects.size(); j++) {
			//	if (m_boundsShielded[j].isInRange(m_binBoundaries[i]) || m_boundsShielded[j].isInRange(m_binBoundaries[i + 1])) {
			//		objectIndices.push_back(j);
			//	}
			//}
			//
			//// Collect all vertices
			//std::vector<const std::vector<sml::Vector2d>*> verticesOfObjects;
			//for (size_t index : objectIndices)
			//{
			//	verticesOfObjects.push_back(&verticesShieldingObjects[index]);
			//}
			//
			//// Projects objects onto the link with identical discretisation points
			//std::vector<std::vector<sml::Segment>> objectSegments = sml::projectPolygons(link, angle, verticesOfObjects);
			//
			//// Filter redudent segments per object
			//// Note, after this step, the total set of segments might still overlap. However this filter step is delayed _after_ the object elevation filter is applied
			//for (auto& segments : objectSegments) {
			//	segments = sml::filterSegments(segments);
			//}
			//
			//// For the ram distribution, the local angles are required
			//double localAngle = sml::normalisedBalancedAngles(angle - linkAngle);
			//double localLowerAngle = sml::normalisedBalancedAngles(localAngle - 0.5 * binSize);
			//double localUpperAngle = sml::normalisedBalancedAngles(localAngle + 0.5 * binSize);
			//
			//// Check if the object is within the applicable area
			//bool inDriftArea = true;	// Drift includes all objects, it is the lest restrictive filter should be in place during the LinkObject creation
			//bool inRamArea = false;
			//for (const auto& i : objectIndices) {
			//	if (isInRamArea(verticesShieldingObjects[i], localLowerAngle, localUpperAngle)) {
			//		inRamArea = true;
			//	}
			//}
			//
			//// Create drift projection
			//if (inDriftArea) {
			//	Projection projection;
			//	projection.p = m_linkObject->driftDirectionProbability(upperBound, lowerBound);
			//	std::cout << "Drift projection chance: " << projection.p << std::endl;
			//	projection.objectSegments = objectSegments;
			//	projection.objectIndices = objectIndices;
			//	m_projectionsDrift.push_back(projection);
			//}
			//
			//// Create ram projection
			//if (inRamArea) {
			//	Projection projection;
			//	projection.p = sml::cdf_normalDistribution(localUpperAngle, 0.0, GeneralModelData::ramSigma()) - sml::cdf_normalDistribution(localLowerAngle, 0.0, GeneralModelData::ramSigma());
			//	projection.objectSegments = objectSegments;
			//	projection.objectIndices = objectIndices;
			//	m_projectionsRam.push_back(projection);
			//}
		//}
	//}

	// Generate sublink meetings for each ship-object-projection
	// The number of objects to be stored is the number of non-shielding projections (each contain a unique object) 
	// And the sum of the objects of shielding projections. For now this is not implemented and it might require a slightly different indexing strategy
}



void SubLinkObject::computeDriftProjections()
{
	// Non shielding projections
	for (size_t i = 0; i < m_objectIndices.size(); i++) {
		const sml::AngleBounds& totalBounds = m_bounds[i];

		//std::cout << "Total bounds=" << totalBounds << std::endl;
		std::vector<sml::AngleBounds> bins = getBins(totalBounds, m_linkObject.getDriftDiscretisation());
		for (const auto& bin : bins) {
			//std::cout << "bin=" << bin << std::endl;
			// Create local bin
			double driftAngle = sml::normalisedPositiveAngles(bin.getLowerBound() + 0.5 * bin.getExtent());

			// Compute the projection segments
			std::vector<sml::Segment> segments = sml::computeSegments(m_line, driftAngle, *m_vertices[i]);

			// Create a drift projection
			ProjectionDrift projection;
			projection.bin = bin;
			projection.objectSegments.push_back(segments);
			projection.localObjectIndices.push_back(m_objectIndices[i]);
			m_projectionsDrift.push_back(projection);
		}
	}
}



void SubLinkObject::computeRamProjections()
{
	// Grab the data
	const sml::AngleBounds& localRamBound = GeneralModelData::get().localRamBounds();

	for (size_t i = 0; i < m_objectIndices.size(); i++) {
		const sml::AngleBounds& objectBounds = m_bounds[i];
		std::vector<sml::AngleBounds> bins = getBins(objectBounds, m_linkObject.getRamDiscretisation());

		for (const auto& bin : bins) {
			// Create local bin
			double driftAngle = sml::normalisedPositiveAngles(bin.getLowerBound() + 0.5 * bin.getExtent());

			// Compute the projection segments
			std::vector<sml::Segment> segments = sml::computeSegments(m_line, driftAngle, *m_vertices[i]);

			// Create ram projection
			sml::AngleBounds localBin;
			localBin.setLowerBound(sml::normalisedPositiveAngles(bin.getLowerBound() - m_linkObject.getLinkAngle()));
			localBin.setUpperBound(sml::normalisedPositiveAngles(bin.getUpperBound() - m_linkObject.getLinkAngle()));
			localBin.setExtent(bin.getExtent());
			bool inRamArea = isInRamArea(*m_vertices[i], localRamBound, localBin);
			if (inRamArea) {
				ProjectionRam projection;
				projection.probability = sml::cdf_normalDistribution(sml::normalisedBalancedAngles(localBin.getUpperBound()), 0.0, GeneralModelData::ramSigma())
					- sml::cdf_normalDistribution(sml::normalisedBalancedAngles(localBin.getLowerBound()), 0.0, GeneralModelData::ramSigma());
				projection.objectSegments.push_back(segments);
				projection.localObjectIndices.push_back(m_objectIndices[i]);
				m_projectionsRam.push_back(projection);
			}
		}
	}
}



void SubLinkObject::computeExposuresDrift(const double pSubLink, const double elevationMin, const double elevationMax, const DriftModel& driftModel, LinkObjectMeetingData& meeting_i)
{
	// Collect the meetings between ship i and its objects from the projections
	const auto& objects = m_linkObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projectionsDrift.size(); iProjection++) {
		const auto& projection = m_projectionsDrift[iProjection];
		std::vector<std::vector<sml::Segment>> validSegments; // [object][segments]
		getValidSegments(validSegments, projection.localObjectIndices, projection.objectSegments, elevationMin, elevationMax);
		if (validSegments.size() == 0) {
			continue;	// Nothing to do
		}

		// Compute the exposure and place it in the right bin
		for (size_t iObject = 0; iObject < validSegments.size(); iObject++) {
			const auto& segments = validSegments[iObject];
			if (segments.size() > 0) {
				size_t localObjectIndex = projection.localObjectIndices[iObject];
				size_t globalObjectIndex = objects[localObjectIndex]->getIndex();
				// Compute the exposure
				double exposure = 0.0;
				for (size_t iSegment = 0; iSegment < segments.size(); iSegment++) {
					const auto& segment = segments[iSegment];
					double targetDistance = 0.5 * (segment.distanceStart + segment.distanceEnd);
					double probabilityDrift = driftModel.computeExposureProbability(projection.bin, targetDistance);
					exposure += probabilityDrift * segment.line.getLength();
					//if (probabilityDrift != 0.0) {
						//std::cout << "p=" << probabilityDrift << ", targetDistance=" << targetDistance << ", length=" << segment.line.getLength() << std::endl;
					//}
				}

				// Set the data proper values
				meeting_i.exposures[localObjectIndex] += pSubLink * exposure;
				meeting_i.globalObjectIndex[localObjectIndex] = globalObjectIndex;
			}
		}
	}
}



void SubLinkObject::computeExposuresRam(const double pSubLink, const double elevationMin, const double elevationMax, LinkObjectMeetingData& meeting_i)
{
	const auto& objects = m_linkObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projectionsRam.size(); iProjection++) {
		const auto& projection = m_projectionsRam[iProjection];
		std::vector<std::vector<sml::Segment>> validSegments; // [object][segments]
		getValidSegments(validSegments, projection.localObjectIndices, projection.objectSegments, elevationMin, elevationMax);
		if (validSegments.size() == 0) {
			continue;	// Nothing to do
		}

		// Compute the exposure and place it in the right bin
		for (size_t iObject = 0; iObject < validSegments.size(); iObject++) {
			const auto& segments = validSegments[iObject];
			if (segments.size() > 0) {
				size_t localObjectIndex = projection.localObjectIndices[iObject];
				size_t globalObjectIndex = objects[localObjectIndex]->getIndex();

				// Compute the exposure
				double exposure = 0.0;
				for (size_t iSegment = 0; iSegment < segments.size(); iSegment++) {
					const auto& segment = segments[iSegment];
					assert(projection.probability <= 1.0);
					exposure += projection.probability * segment.line.getLength();
				}

				// Set the data proper values
				meeting_i.exposures[localObjectIndex] += pSubLink * exposure;
				meeting_i.globalObjectIndex[localObjectIndex] = globalObjectIndex;
			}
		}
	}
}




// The computation of drift measures gets a bit tricky.
// events = causation_factor * pdistribution * pExposure * q * segment.line.getLength();
// collisions = causation_factor * pdistribution * pMeasure * q * segment.line.getLength();
// therefore 
// collisions = events * pMeasure / pExposure
void SubLinkObject::computeCollisionsDrift(const double pSubLink, const double elevationMin, const double elevationMax, const DriftModel& driftModel, LinkObjectMeetingData& meeting)
{
	// Collect the meetings between ship i and its objects from the projections
	const auto& objects = m_linkObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projectionsDrift.size(); iProjection++) {
		const auto& projection = m_projectionsDrift[iProjection];
		std::vector<std::vector<sml::Segment>> validSegments; // [object][segments]
		getValidSegments(validSegments, projection.localObjectIndices, projection.objectSegments, elevationMin, elevationMax);
		if (validSegments.size() == 0) {
			continue;	// Nothing to do
		}

		// Compute the exposure and place it in the right bin
		for (size_t iObject = 0; iObject < validSegments.size(); iObject++) {
			const auto& segments = validSegments[iObject];
			if (segments.size() > 0) {
				size_t localObjectIndex = projection.localObjectIndices[iObject];
				const sim::Object* object = objects[localObjectIndex];

				// Evaluate the segments
				double collision = 0;
				for (size_t iSegment = 0; iSegment < segments.size(); iSegment++) {
					const auto& segment = segments[iSegment];
					double targetDistance = 0.5 * (segment.distanceStart + segment.distanceEnd);
					if (m_linkObject.usePassingDistance(object)) {
						targetDistance = std::max(targetDistance, GeneralModelData::minimumPassingDistance());
					}
					collision += meeting.causationFactor * driftModel.computeMeasureProbability(projection.bin, targetDistance) * segment.line.getLength();
				}

				// Set the data
				meeting.collisions[localObjectIndex] += pSubLink * collision;
			}
		}
	}
}



void SubLinkObject::computeCollisionsRam(const double pSubLink, const double elevationMin, const double elevationMax, const double length, LinkObjectMeetingData& meeting)
{
	const auto& objects = m_linkObject.getNonShieldingObjects();
	for (size_t iProjection = 0; iProjection < m_projectionsRam.size(); iProjection++) {
		const auto& projection = m_projectionsRam[iProjection];
		std::vector<std::vector<sml::Segment>> validSegments; // [object][segments]
		getValidSegments(validSegments, projection.localObjectIndices, projection.objectSegments, elevationMin, elevationMax);
		if (validSegments.size() == 0) {
			continue;
		}

		for (size_t iObject = 0; iObject < validSegments.size(); iObject++) {
			const auto& segments = validSegments[iObject];
			size_t localObjectIndex = projection.localObjectIndices[iObject];

			double collisions = 0;
			for (size_t iSegment = 0; iSegment < segments.size(); iSegment++) {
				const auto& segment = segments[iSegment];

				// Default no effect
				double pMeasures = 1.0;

				// Avoidance function
				// Old version
				//double targetDistance = 0.5 * (segment.distanceStart + segment.distanceEnd);
				//pMeasures *= std::exp(-0.1 * targetDistance / length);
				// New version: integrated over segment
				// a) This makes the result independent of the discretisation of the link and object
				// b) This formulation takes into account that the function scales non-linear with distance, evaluating at all points a long the link
				if (m_linkObject.useAvoidance()) {
					double factor = length / (-0.1 * (segment.distanceEnd - segment.distanceStart));
					if (m_linkObject.usePassingDistance(objects[localObjectIndex])) {
						// Distance can be linearly reconstructed as D = D0 + t * (D1 - D0) / L, find tPassing such that D(tPassing) = minimumPassingDistance
						double d0 = segment.distanceStart;
						double d1 = segment.distanceEnd;
						double dMin = GeneralModelData::minimumPassingDistance();
						double a = -0.1; // TODO place in GeneralModelData; // TODO replace by a / length
						double pAvoidance = 0.0;
						if (d0 <= dMin && d1 <= dMin) {
							// Completely inside passing distance
							pAvoidance = std::exp(a * GeneralModelData::minimumPassingDistance() / length);
						}
						else if (d0 > dMin && d1 > dMin) {
							// Standard case without minimum passing distance
							pAvoidance = factor * (std::exp(a * segment.distanceEnd / length) - std::exp(a * segment.distanceStart / length));
						}
						else {
							// Partially integrate the part without distance limiting
							double Lseg = segment.line.getLength();
							double x = (dMin - d0) / (d1 - d0) * segment.line.getLength();
							double dx = d0 + (d1 - d0) / segment.line.getLength() * x; // Cross over point
							if (d0 > dMin) {
								pAvoidance += factor * (std::exp(a * dx / length) - std::exp(a * d0 / length));
								pAvoidance += (Lseg - x) * std::exp(a * GeneralModelData::minimumPassingDistance() / length);
							}
							else {
								pAvoidance += factor * (std::exp(a * d1 / length) - std::exp(a * dx / length));
								pAvoidance += x * std::exp(a * GeneralModelData::minimumPassingDistance() / length);
							}
							pAvoidance /= segment.line.getLength();
						}
						pMeasures *= pAvoidance;
					}
					else {
						pMeasures *= factor * (std::exp(-0.1 * segment.distanceEnd / length) - std::exp(-0.1 * segment.distanceStart / length));
					}
				}


				// Pilot assistance
				// TODO SEM Model is not yet finalised on this part

				collisions += meeting.causationFactor * projection.probability * segment.line.getLength() * pMeasures;
			}

			// Store the data
			meeting.collisions[localObjectIndex] += pSubLink * collisions;
		}
	}
}



bool SubLinkObject::isInRamArea(const std::vector<sml::Vector2d>& vertices, const sml::AngleBounds& localRamBound, const sml::AngleBounds& binBounds)
{
	// Angle criteria
	if (!localRamBound.hasOverlapWith(binBounds)) {
		return false;
	}

	// Distance criteria
	bool radiusCriteria = false;
	for (size_t i = 0; i < vertices.size(); i++) {
		sml::Line edge(vertices[i], vertices[(i + 1) % vertices.size()]);
		if (edge.distanceToPoint(m_line.getStart()) < GeneralModelData::ramRadius()) {
			radiusCriteria = true;
			break;
		}
		if (edge.distanceToPoint(m_line.getEnd()) < GeneralModelData::ramRadius()) {
			radiusCriteria = true;
			break;
		}
		if (m_line.distanceToPoint(vertices[i]) < GeneralModelData::ramRadius()) {
			radiusCriteria = true;
			break;
		}
	}

	return radiusCriteria;
}



std::vector<sml::AngleBounds> SubLinkObject::getBins(const sml::AngleBounds& bounds, const std::vector<double>& discretisation)
{
	std::vector<sml::AngleBounds> bins;
	if (bounds.getLowerBound() > bounds.getUpperBound()) {
		constexpr double twopi = 2.0 * std::numbers::pi;
		createBins(bins, bounds.getLowerBound(), 2.0 * std::numbers::pi, discretisation);
		createBins(bins, 0, bounds.getUpperBound(), discretisation);
	}
	else {
		createBins(bins, bounds.getLowerBound(), bounds.getUpperBound(), discretisation);
	}
	return bins;
}



void SubLinkObject::createBins(std::vector<sml::AngleBounds>& bins, const double lowerBound, const double upperBound, const std::vector<double>& discretisation)
{
	assert(upperBound > lowerBound);
	assert(upperBound <= 2.0 * std::numbers::pi && upperBound >= 0.0);
	assert(lowerBound <= 2.0 * std::numbers::pi && lowerBound >= 0.0);

	auto lower = std::lower_bound(discretisation.begin(), discretisation.end(), lowerBound);
	auto upper = std::upper_bound(lower, discretisation.end(), upperBound);

	if (lower == discretisation.end() || upper == lower) {
		//The bounds are beyond the last discretisation entry or the lowerBound and upperbound are both between two discretisation points
		bins.push_back({ lowerBound, upperBound, upperBound - lowerBound });
		return;
	}
	else {
		upper -= 1;	// The upper angle is not part of the bin, get the last angle that is inside the bin
		// Start bin
		double lowerangle = *lower;
		if (lowerangle != lowerBound) {
			bins.push_back({ lowerBound, lowerangle, lowerangle - lowerBound });
		}

		// Middle bins
		size_t numMiddleBins = std::distance(lower, upper);
		for (size_t i = 0; i < numMiddleBins; i++) {
			double lowerAngle = *(lower + i);
			double upperAngle = *(lower + i + 1);
			double extent = upperAngle - lowerAngle;
			bins.push_back({ lowerAngle, upperAngle, extent });
		}

		// End bin 
		double upperangle = *upper;
		bins.push_back({ upperangle, upperBound, upperBound - upperangle });
	}
}



void SubLinkObject::getValidSegments(ObjectSegments& validSegments, const std::vector<size_t>& objectIndices, const ObjectSegments& objectSegments, const double elevationMin, const double elevationMax)
{
	validSegments.clear();
	size_t numObjects = objectIndices.size();

	// Elevation mask
	size_t numActiveObjects = 0;
	std::vector<int> mask(numObjects, 0);
	for (size_t i = 0; i < numObjects; i++) {
		const auto& object = m_linkObject.getObject(objectIndices[i]);
		double objectElevation = object->getScheme()->getElevation();
		if ((objectElevation >= elevationMin) && (objectElevation) <= elevationMax) {
			mask[i] = 1;
			numActiveObjects++;
		}
	}

	// Generate a set of valid segments, if the set is larger than one, perform shielding, generating a custom set of segments on the fly
	if (numActiveObjects > 1) {
		validSegments = filterSegments(objectSegments, mask);
	}
	else if (numActiveObjects == 1) {
		validSegments.resize(numObjects);
		// Only one active object filter out the rest
		for (size_t i = 0; i < numObjects; i++) {
			if (mask[i] == 1) {
				validSegments[i] = objectSegments[i];
			}
		}
	}
}
} // namespace sem

