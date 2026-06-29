// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

/** @file ILeftMergable.h
 * @brief This file contains the interface for left merge operation.
 * 
 */

#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "IModel.h"

namespace tdb {

	/*
	template<class T>
	using LeftMergeType = std::shared_ptr<T>;

	template<class T>
	using LeftMergeList = std::vector<LeftMergeType<T>>;
	*/

	/**
	 * @class ILeftMergable 
	 * @brief Interface for left merge operation.
	 * 
	 *  This interface is used to provide a common way to retrieve all records from the left table 
	 * and matching records from the right table. If there is no match, the right table record is 
	 * not included.
	 */
	template<class Right>
	class ILeftMergable {
	public:
		virtual ~ILeftMergable() = default;

		using R_UidType = Right::UidType;
		
		
		/**
		 * @brief Finds matching records from the right table that is equal to the foreign key in the left table.
		 * 
		 * This function iterates through the right table and finds records that match the specified 
		 * foreign key from the left table. If there is no match, the right table record is not included 
		 * in the result. The matching records are stored in the provided list.
		 * 
		 * @param[in] rights The right table to merge.
		 * @param[in] leftOn The UID of the left table to match against.
		 * @param[in,out] leftMerged A list to store the matching records from the right table.
		 */
		void leftMerge(const DataTable<Right>& rights, const R_UidType& leftOn, DataTable<Right>& leftMerged) {
			for (const auto& r : rights) {
				if (r->getUID() == leftOn) {
					leftMerged.push_back(r);
				}
			}
		}

		/**
		 * @brief Finds the first matching record from the right table that is equal to the foreign key in the left table.
		 * 
		 * This function iterates through the right table and finds the first record that matches the specified 
		 * foreign key from the left table. If there is no match, the right table record does not update 
		 * the result. The result is stored in a single record.
		 * 
		 * @param[in] rights The right table to merge
		 * @param[in] leftOn UID of the left table
		 * @param[in,out] leftMerged Returns a single matching record
		 */
		void leftMerge(const DataTable<Right>& rights, const R_UidType& leftOn, Data<Right>& leftMerged) {
			for (const auto& r : rights) {
				if (r->getUID() == leftOn) {
					leftMerged = r;
					break;
				}
			}
		}
	};

} // namespace tdb
