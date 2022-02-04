﻿/*
  Copyright (C) 2022 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "miGUI.h"
#include "mgFunctions.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int g_dockpanel_inSplitterRect = 0;
struct mgDockPanelElement_s* g_dockpanel_splitterMode = 0;

void
mgDockPanelRebuild(struct mgContext_s* c)
{
	c->dockPanel->elements[0].rect = c->dockPanel->rect;

	if (c->dockPanel->elementsNum > 1)
	{
		for (int i = 1; i < c->dockPanel->elementsNum; ++i)
		{
			mgRect rect;
			mgRect splitterRect;
			switch (c->dockPanel->elements[i].info.where)
			{
			default:
			case 0:
				rect.left = c->dockPanel->elements[0].rect.left;
				rect.top = c->dockPanel->elements[0].rect.top;
				rect.bottom = c->dockPanel->elements[0].rect.bottom;
				rect.right = rect.left + c->dockPanel->elements[i].info.size;
				c->dockPanel->elements[0].rect.left = rect.right;
				splitterRect.left = rect.right - c->dockPanel->splitterWidth;
				splitterRect.right = rect.right + c->dockPanel->splitterWidth;
				splitterRect.top = rect.top;
				splitterRect.bottom = rect.bottom;
				break;
			case 1:
				rect.left = c->dockPanel->elements[0].rect.left;
				rect.top = c->dockPanel->elements[0].rect.top;
				rect.right = c->dockPanel->elements[0].rect.right;
				rect.bottom = rect.top + c->dockPanel->elements[i].info.size;
				c->dockPanel->elements[0].rect.top = rect.bottom;
				splitterRect.left = rect.left;
				splitterRect.right = rect.right;
				splitterRect.top = rect.bottom - c->dockPanel->splitterWidth;
				splitterRect.bottom = rect.bottom + c->dockPanel->splitterWidth;
				break;
			case 2:
				rect.top = c->dockPanel->elements[0].rect.top;
				rect.bottom = c->dockPanel->elements[0].rect.bottom;
				rect.right = c->dockPanel->elements[0].rect.right;
				rect.left = rect.right - c->dockPanel->elements[i].info.size;
				c->dockPanel->elements[0].rect.right = rect.left;
				splitterRect.left = rect.left - c->dockPanel->splitterWidth;
				splitterRect.right = rect.left + c->dockPanel->splitterWidth;
				splitterRect.top = rect.top;
				splitterRect.bottom = rect.bottom;
				break;
			case 3:
				rect.bottom = c->dockPanel->elements[0].rect.bottom;
				rect.left = c->dockPanel->elements[0].rect.left;
				rect.right = c->dockPanel->elements[0].rect.right;
				rect.top = rect.bottom - c->dockPanel->elements[i].info.size;
				c->dockPanel->elements[0].rect.bottom = rect.top;
				splitterRect.left = rect.left;
				splitterRect.right = rect.right;
				splitterRect.top = rect.top - c->dockPanel->splitterWidth;
				splitterRect.bottom = rect.top + c->dockPanel->splitterWidth;
				break;
			}
			c->dockPanel->elements[i].rect = rect;
			c->dockPanel->elements[i].splitterRect = splitterRect;
		}
	}
}

void
mgDockPanelOnSize(struct mgContext_s* c)
{
	c->dockPanel->rect.left = c->dockPanel->indent.left;
	c->dockPanel->rect.top = c->dockPanel->indent.top;
	c->dockPanel->rect.right = c->windowSize.x - c->dockPanel->indent.right;
	c->dockPanel->rect.bottom = c->windowSize.y - c->dockPanel->indent.bottom;
	mgDockPanelRebuild(c);
}

void
mgDrawDockPanel(struct mgContext_s* c)
{
	if (c->dockPanel->flags & mgDockPanelFlag_drawBG)
	{
		c->gpu->setClipRect(&c->dockPanel->rect);
		c->gpu->drawRectangle(mgDrawRectangleReason_dockBG, &c->dockPanel->rect, 
			&c->activeStyle->dockpanelBGColor, 
			&c->activeStyle->dockpanelBGColor, 
			0, 0, 0);
	}

	for (int i = 0; i < c->dockPanel->elementsNum; ++i)
	{
		c->gpu->setClipRect(&c->dockPanel->elements[i].rect);

		if (c->dockPanel->elements[i].flags & mgDockPanelElementFlag_drawBG)
		{
			c->gpu->drawRectangle(mgDrawRectangleReason_dockElementBG, &c->dockPanel->elements[i].rect,
				&c->dockPanel->elements[i].colorBG, 
				&c->dockPanel->elements[i].colorBG, 
				0, 0, 0);
		}
	}

	if (c->dockPanel->flags & mgDockPanelFlag_drawSplitterBG)
	{
		for (int i = 0; i < c->dockPanel->elementsNum; ++i)
		{
			c->gpu->setClipRect(&c->dockPanel->elements[i].splitterRect);
			c->gpu->drawRectangle(mgDrawRectangleReason_dockSplitterBG, &c->dockPanel->elements[i].splitterRect,
				&c->activeStyle->dockpanelSplitterBGColor,
				&c->activeStyle->dockpanelSplitterBGColor,
				0, 0, 0);
		}
	}
}

void
mgDockPanelUpdate(struct mgContext_s* c)
{
	static mgPoint firstClick;
	static int sizeOnClick = 0;

	if(!g_dockpanel_splitterMode)
		g_dockpanel_inSplitterRect = 0;

	if (g_dockpanel_splitterMode)
	{
		if (c->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
			g_dockpanel_splitterMode = 0;
	}

	if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
	{
		firstClick = c->input->mousePosition;
	}

	for (int i = 0; i < c->dockPanel->elementsNum; ++i)
	{
		if (mgPointInRect(&c->dockPanel->elements[i].splitterRect, &c->input->mousePosition))
		{
			g_dockpanel_inSplitterRect = 1;
			switch (c->dockPanel->elements[i].info.where)
			{
			case 0:
			case 2:
				if (!g_dockpanel_splitterMode)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeWE], mgCursorType_Arrow);
				break;
			default:
				if (!g_dockpanel_splitterMode)
					mgSetCursor_f(c, c->defaultCursors[mgCursorType_SizeNS], mgCursorType_Arrow);
				break;
			}
			c->dockPanel->flags |= mgDockPanelFlag_cursorChanged;

			if (c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
			{
				if (!g_dockpanel_splitterMode)
				{
					g_dockpanel_splitterMode = &c->dockPanel->elements[i];
					sizeOnClick = g_dockpanel_splitterMode->info.size;
				}
			}
		}
	}
	if (!g_dockpanel_inSplitterRect && (c->dockPanel->flags & mgDockPanelFlag_cursorChanged)
		&& !g_dockpanel_splitterMode)
	{
		mgSetCursor_f(c, c->defaultCursors[mgCursorType_Arrow], mgCursorType_Arrow);
		c->dockPanel->flags ^= mgDockPanelFlag_cursorChanged;
	}

	if (g_dockpanel_splitterMode)
	{
		int px = c->input->mousePosition.x - firstClick.x;
		int py = c->input->mousePosition.y - firstClick.y;

		switch (g_dockpanel_splitterMode->info.where)
		{
		case 0:
			g_dockpanel_splitterMode->info.size = sizeOnClick + px;
			break;
		case 1:
			g_dockpanel_splitterMode->info.size = sizeOnClick + py;
			break;
		case 2:
			g_dockpanel_splitterMode->info.size = sizeOnClick - px;
			break;
		case 3:
			g_dockpanel_splitterMode->info.size = sizeOnClick - py;
			break;
		}
		if (g_dockpanel_splitterMode->info.size > g_dockpanel_splitterMode->info.sizeMaximum)
			g_dockpanel_splitterMode->info.size = g_dockpanel_splitterMode->info.sizeMaximum;
		if (g_dockpanel_splitterMode->info.size < g_dockpanel_splitterMode->info.sizeMinimum)
			g_dockpanel_splitterMode->info.size = g_dockpanel_splitterMode->info.sizeMinimum;
		mgDockPanelRebuild(c);
	}
}

mgColor colors[] = {
	{1.f, 0.f, 0.f, 1.f},
	{0.f, 1.f, 0.f, 1.f},
	{0.f, 0.f, 1.f, 1.f},

	{1.f, 1.f, 0.f, 1.f},
	{0.f, 1.f, 1.f, 1.f},
	{1.f, 0.f, 1.f, 1.f},
};

MG_API
void MG_C_DECL
mgInitDockPanel_f(
	struct mgContext_s* c, 
	int indentLeft, 
	int indentTop, 
	int indentRight, 
	int indentBottom, 
	mgDockPanelElementCreationInfo* elements, 
	int elementsSize)
{
	assert(c);
	assert(!c->dockPanel);
	assert(elementsSize > 0);
	assert(indentLeft >= 0);
	assert(indentTop >= 0);
	assert(indentRight >= 0);
	assert(indentBottom >= 0);

	c->dockPanel = calloc(1, sizeof(struct mgDockPanel_s));
	c->dockPanel->indent.left = indentLeft;
	c->dockPanel->indent.top = indentTop;
	c->dockPanel->indent.right = indentRight;
	c->dockPanel->indent.bottom = indentBottom;
	c->dockPanel->flags = mgDockPanelFlag_drawSplitterBG;
	c->dockPanel->splitterWidth = 3;

	c->dockPanel->elementsNum = elementsSize + 1;
	c->dockPanel->elements = calloc(1, sizeof(mgDockPanelElement) * c->dockPanel->elementsNum);
	mgColorSetAsIntegerRGB(&c->dockPanel->elements[0].colorBG, 0xFFFFFF);
	static int colorCount = 0;
	for (int i = 0; i < elementsSize; ++i)
	{
		c->dockPanel->elements[i + 1].info = elements[i];
		c->dockPanel->elements[i + 1].colorBG = colors[colorCount];

		colorCount++;
		if (colorCount > 5)
			colorCount = 0;
	}

	mgDockPanelOnSize(c);
}

//MG_API
//void MG_C_DECL
//mgDockPanelSetElement_f(struct mgContext_s* c, int id, int where, int size)
//{
//	assert(c);
//	assert(c->dockPanel);
//	assert((id > 0) && (id < c->dockPanel->elementsNum));
//	assert((where >= 0) && (where <= 3));
//	assert(size > 0);
//
//	//c->dockPanel->elements[id].
//}
