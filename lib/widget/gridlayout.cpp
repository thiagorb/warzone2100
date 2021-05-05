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

void GridLayout::place(GridPlacement placement)
{
    attach(placement.widget);
    placements.emplace_back(placement);
    layoutDirty = true;
    offsetsDirty = true;
}

void GridLayout::run(W_CONTEXT *context)
{
    updateLayout();
}

void GridLayout::updateLayout()
{
    if (!layoutDirty)
    {
        return;
    }

    layoutDirty = false;
    updateOffsets();

    auto gridWidth = columnOffsets.back();
    auto gridHeight = rowOffsets.back();
    for (auto &placement: placements)
    {
        auto left = columnOffsets[placement.location.colStart] * width() / gridWidth;
        auto right = columnOffsets[placement.location.colStart + placement.location.colSpan] * width() / gridWidth;
        auto top = rowOffsets[placement.location.rowStart] * height() / gridHeight;
        auto bottom = rowOffsets[placement.location.rowStart + placement.location.rowSpan] * height() / gridHeight;
        placement.widget->setGeometry(left, top, right - left, bottom - top);
    }
}

void GridLayout::updateOffsets()
{
    if (!offsetsDirty)
    {
        return;
    }

    offsetsDirty = false;

    uint32_t cols = 0;
    uint32_t rows = 0;
    for (const auto &placement: placements)
    {
        cols = std::max(cols, placement.location.colStart + placement.location.colSpan);
        rows = std::max(rows, placement.location.rowStart + placement.location.rowSpan);
    }

    auto columnOrderedIndex = std::vector<size_t>(placements.size());
    auto rowOrderedIndex = std::vector<size_t>(placements.size());
    for (size_t i = 0; i < placements.size(); i++)
    {
        columnOrderedIndex[i] = i;
        rowOrderedIndex[i] = i;
    }

    std::sort(columnOrderedIndex.begin(), columnOrderedIndex.end(), [&](size_t a, size_t b) {
        return placements[a].location.colStart < placements[b].location.colStart;
    });
    std::sort(rowOrderedIndex.begin(), rowOrderedIndex.end(), [&](size_t a, size_t b) {
        return placements[a].location.rowStart < placements[b].location.rowStart;
    });

    columnOffsets.assign(cols + 1, 0);
    rowOffsets.assign(rows + 1, 0);
    for (auto placementIndex: columnOrderedIndex)
    {
        auto const &placement = placements[placementIndex];
        columnOffsets[placement.location.colStart + placement.location.colSpan] = std::max(
            columnOffsets[placement.location.colStart + placement.location.colSpan],
            columnOffsets[placement.location.colStart] + placement.widget->idealWidth()
        );
    }
    for (auto placementIndex: rowOrderedIndex)
    {
        auto const &placement = placements[placementIndex];
        rowOffsets[placement.location.rowStart + placement.location.rowSpan] = std::max(
            rowOffsets[placement.location.rowStart + placement.location.rowSpan],
            rowOffsets[placement.location.rowStart] + placement.widget->idealHeight()
        );
    }
}