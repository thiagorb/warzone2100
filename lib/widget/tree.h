/*
	This file is part of Warzone 2100.
	Copyright (C) 2020  Warzone 2100 Project

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

#ifndef __INCLUDED_LIB_WIDGET_TREE_H__
#define __INCLUDED_LIB_WIDGET_TREE_H__

#include "widget.h"

struct WidgetTree
{
public:
    WidgetTree(std::shared_ptr<WIDGET> root): root(root)
    {
    }

    WidgetTree(std::shared_ptr<WIDGET> root, std::initializer_list<WidgetTree> children): root(root)
    {
        for (auto child: children)
        {
            root->attach(child);
        }
    }
    
    operator std::shared_ptr<WIDGET>()
    {
        return root;
    }

private:
    std::shared_ptr<WIDGET> root;
};

#endif // __INCLUDED_LIB_WIDGET_TREE_H__