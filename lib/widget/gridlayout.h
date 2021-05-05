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
#include "widget.h"

struct GridLocation
{
public:
	GridLocation() {}

	GridLocation(uint32_t colStart, uint32_t rowStart, uint32_t colSpan, uint32_t rowSpan)
		: colStart(colStart)
		, rowStart(rowStart)
		, colSpan(colSpan)
		, rowSpan(rowSpan)
	{
	}

	uint32_t colStart = 0;
	uint32_t rowStart = 0;
	uint32_t colSpan = 1;
	uint32_t rowSpan = 1;
};

struct GridPlacement
{
public:
	GridPlacement() {}

	GridPlacement(GridLocation location, std::shared_ptr<WIDGET> widget)
		: location(location)
		, widget(widget)
	{
	}

	GridLocation location;
	std::shared_ptr<WIDGET> widget;
};

class GridLayout: public WIDGET
{
public:
	void place(GridPlacement placement);

protected:
	void run(W_CONTEXT *context) override;
	void geometryChanged() override
	{
		layoutDirty = true;
	}

	int32_t idealWidth()
	{
		updateOffsets();
		return columnOffsets.back();
	}

	int32_t idealHeight()
	{
		updateOffsets();
		return rowOffsets.back();
	}

private:
	void updateLayout();
	void updateOffsets();

	std::vector<GridPlacement> placements;
	bool layoutDirty = false;
	bool offsetsDirty = false;
    std::vector<uint32_t> columnOffsets;
    std::vector<uint32_t> rowOffsets;
};

#endif // __INCLUDED_LIB_WIDGET_GRIDLAYOUT_H__
