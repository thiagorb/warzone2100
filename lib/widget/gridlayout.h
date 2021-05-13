/*
	This file is part of Warzone 2100.
	Copyright (C) 2021  Warzone 2100 Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef __INCLUDED_LIB_WIDGET_GRIDLAYOUT_H__
#define __INCLUDED_LIB_WIDGET_GRIDLAYOUT_H__

#include <optional-lite/optional.hpp>
#include <map>
#include <set>
#include "widget.h"

class GridLayout: public WIDGET
{
public:
	struct Slot
	{
		Slot(uint32_t start = 0, uint32_t span = 1, bool expandable = true);
		uint32_t start;
		uint32_t span;
		bool expandable;
	};

	struct Placement
	{
	public:
		Placement(Slot column, Slot row, std::shared_ptr<WIDGET> widget);
		Slot column;
		Slot row;
		std::shared_ptr<WIDGET> widget;
	};

	class Allocation
	{
	public:
		struct Item
		{
			GridLayout::Slot slot;
			int32_t idealSize;

			uint32_t start() const;
			uint32_t end() const;
		};

		Allocation(std::vector<Item> items);
		std::map<uint32_t, int32_t> calculateOffsets(int32_t availableSpace);
		int getMinimumSizeRequired();

	private:
		void sortItems();
		void initializeSlots();
		void initializeExpandableSlots();
		void initializeMinimumSizes();
		void initializeRigidSlots();
		void initializeMinimumOffsets();

		std::vector<Item> items;
		std::map<std::pair<uint32_t, uint32_t>, int> minimumSizes;
		std::map<uint32_t, bool> rigidSlots;
		std::set<uint32_t> slots;
		std::map<uint32_t, int32_t> minimumOffsets;
		uint32_t expandableSlots;
	};

	GridLayout();
	void place(Slot columnAllocation, Slot rowAllocation, std::shared_ptr<WIDGET> widget);
	int32_t idealWidth();
	int32_t idealHeight();
	nonstd::optional<std::vector<uint32_t>> getScrollSnapOffsets() override;

protected:
	void run(W_CONTEXT *context) override;
	void geometryChanged() override;

private:
	void updateLayout();
	void invalidateAllocation();

	Allocation &getColumnAllocation();
	Allocation &getRowAllocation();

	nonstd::optional<Allocation> columnAllocation;
	nonstd::optional<Allocation> rowAllocation;
	std::vector<Placement> placements;
	bool layoutDirty = false;
	std::vector<uint32_t> scrollSnapOffsets;
};

#endif // __INCLUDED_LIB_WIDGET_GRIDLAYOUT_H__
