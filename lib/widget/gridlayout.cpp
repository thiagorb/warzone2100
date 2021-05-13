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

#include "gridlayout.h"
#include "widgbase.h"
#include "lib/ivis_opengl/pieblitfunc.h"
#include <algorithm>
#include <glm/gtx/compatibility.hpp>
#include <optional-lite/optional.hpp>
#include <map>
#include <set>

static std::vector<GridLayout::Allocation::Item> getAllocationItems(
	const std::vector<GridLayout::Placement> &placements,
	std::function<GridLayout::Allocation::Item(const GridLayout::Placement)> placementTransformer
)
{
	auto items = std::vector<GridLayout::Allocation::Item>();
	for (const auto &placement: placements)
	{
		items.push_back(placementTransformer(placement));
	}

	return items;
}

static GridLayout::Allocation::Item placementColumn(GridLayout::Placement placement)
{
	return {placement.column, placement.widget->idealWidth()};
}

static GridLayout::Allocation::Item placementRow(GridLayout::Placement placement)
{
	return {placement.row, placement.widget->idealHeight()};
}

GridLayout::GridLayout(): WIDGET()
{
}

void GridLayout::place(Slot column, Slot row, std::shared_ptr<WIDGET> widget)
{
	attach(widget);
	placements.emplace_back(GridLayout::Placement {column, row, widget});
	layoutDirty = true;
	invalidateAllocation();
}

int32_t GridLayout::idealWidth()
{
	return getColumnAllocation().getMinimumSizeRequired();
}

int32_t GridLayout::idealHeight()
{
	return getRowAllocation().getMinimumSizeRequired();
}

nonstd::optional<std::vector<uint32_t>> GridLayout::getScrollSnapOffsets()
{
	updateLayout();
	return scrollSnapOffsets;
}

void GridLayout::run(W_CONTEXT *context)
{
	updateLayout();
}

void GridLayout::geometryChanged()
{
	layoutDirty = true;
}

void GridLayout::updateLayout()
{
	if (!layoutDirty)
	{
		return;
	}

	layoutDirty = false;

	auto columnOffsets = getColumnAllocation().calculateOffsets(width());
	auto rowOffsets = getRowAllocation().calculateOffsets(height());
	scrollSnapOffsets.resize(rowOffsets.size());
	size_t i = 0;
	for (auto offset: rowOffsets)
	{
		scrollSnapOffsets[i++] = offset.second;
	}

	for (auto &placement: placements)
	{
		auto left = columnOffsets[placement.column.start];
		auto right = columnOffsets[placement.column.start + placement.column.span];
		auto top = rowOffsets[placement.row.start];
		auto bottom = rowOffsets[placement.row.start + placement.row.span];
		placement.widget->setGeometry(left, top, right - left, bottom - top);
	}
}

void GridLayout::invalidateAllocation()
{
	columnAllocation.reset();
	rowAllocation.reset();
}

GridLayout::Slot::Slot(uint32_t start, uint32_t span, bool expandable)
	: start(start)
	, span(span)
	, expandable(expandable)
{
}

GridLayout::Placement::Placement(Slot column, Slot row, std::shared_ptr<WIDGET> widget)
	: column(column)
	, row(row)
	, widget(widget)
{
}

GridLayout::Allocation &GridLayout::getColumnAllocation()
{
	if (!columnAllocation)
	{
		columnAllocation.emplace(getAllocationItems(placements, placementColumn));
	}

	return columnAllocation.value();
}

GridLayout::Allocation &GridLayout::getRowAllocation()
{
	if (!rowAllocation)
	{
		rowAllocation.emplace(getAllocationItems(placements, placementRow));
	}

	return rowAllocation.value();
}

uint32_t GridLayout::Allocation::Item::start() const
{
	return slot.start;
}

uint32_t GridLayout::Allocation::Item::end() const
{
	return slot.start + slot.span;
}

GridLayout::Allocation::Allocation(std::vector<Item> items): items(items)
{
	sortItems();
	initializeSlots();
	initializeRigidSlots();
	initializeExpandableSlots();
	initializeMinimumSizes();
	initializeMinimumOffsets();
}

std::map<uint32_t, int32_t> GridLayout::Allocation::calculateOffsets(int32_t availableSpace)
{
	std::map<uint32_t, int32_t> offsets;
	auto minimumTotalSize = getMinimumSizeRequired();
	auto freeSpace = std::max(0, availableSpace - minimumTotalSize);

	auto currentSlot = slots.begin();
	auto lastSlot = std::prev(slots.end());
	auto ratio = availableSpace / (float)minimumTotalSize;
	auto stretchScale = expandableSlots > 0 ? std::min(1.0f, ratio) : ratio;
	offsets[*currentSlot] = 0;
	while (currentSlot != lastSlot)
	{
		auto nextSlot = std::next(currentSlot);
		auto left = minimumOffsets[*currentSlot];
		auto right = minimumOffsets[*nextSlot];
		auto minimumSize = right - left;
		auto normalSize = minimumSize * stretchScale;
		auto expandSize = rigidSlots[*currentSlot] ? 0: freeSpace * (*nextSlot - *currentSlot) / expandableSlots;
		offsets[*nextSlot] = offsets[*currentSlot] + normalSize + expandSize;

		currentSlot = nextSlot;
	}

	return offsets;
}

int GridLayout::Allocation::getMinimumSizeRequired()
{
	return std::prev(minimumOffsets.end())->second;
}

void GridLayout::Allocation::sortItems()
{
	std::sort(items.begin(), items.end(), [](Item &a, Item &b) {
		return a.start() == b.start() ? a.end() >= b.end() : a.start() < b.start();
	});
}

void GridLayout::Allocation::initializeSlots()
{
	for (const auto &item: items)
	{
		slots.insert(item.start());
		slots.insert(item.end());
	}
}

void GridLayout::Allocation::initializeRigidSlots()
{
	for (const auto slot: slots)
	{
		rigidSlots[slot] = false;
	}

	for (const auto &item: items)
	{
		if (!item.slot.expandable)
		{
			for (auto slot = slots.find(item.start()); *slot < item.end(); slot++)
			{
				rigidSlots[*slot] = true;
			}
		}
	}
}

void GridLayout::Allocation::initializeExpandableSlots()
{
	expandableSlots = 0;
	auto currentSlot = slots.begin();
	auto lastSlot = std::prev(slots.end());
	while (currentSlot != lastSlot)
	{
		auto nextSlot = std::next(currentSlot);
		if (!rigidSlots[*currentSlot])
		{
			expandableSlots += *nextSlot - *currentSlot;
		}

		currentSlot = nextSlot;
	}
}

void GridLayout::Allocation::initializeMinimumSizes()
{
	for (auto item = items.begin(); item != items.end(); item++)
	{
		auto existing = minimumSizes.find({item->start(), item->end()});
		if (existing != minimumSizes.end() && existing->second >= item->idealSize)
		{
			continue;
		}

		for (auto previousItem = item; previousItem != items.begin() && previousItem->end() >= item->start(); previousItem--)
		{
			auto slotStart = std::min(previousItem->start(), item->start());
			auto slotEnd = std::max(previousItem->end(), item->end());
			minimumSizes[{slotStart, slotEnd}] = std::max(previousItem->idealSize, item->idealSize);
		}
	}
}

void GridLayout::Allocation::initializeMinimumOffsets()
{
	for (const auto slot: slots)
	{
		minimumOffsets[slot] = 0;
	}

	for (const auto &item: items)
	{
		auto minimumSize = minimumSizes[{item.start(), item.end()}];
		auto offset = minimumOffsets[item.start()] + minimumSize;
		for (auto slot = slots.find(item.end()); slot != slots.end(); slot++)
		{
			minimumOffsets[*slot] = std::max(minimumOffsets[*slot], offset);
		}
	}
}