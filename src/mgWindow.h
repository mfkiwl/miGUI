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

#pragma once
#ifndef _MG_WINDOW_H_
#define _MG_WINDOW_H_

struct mgElementNode_s;

enum mgWindowFlag
{
	mgWindowFlag_withTitlebar = 0x1, /*default*/
	mgWindowFlag_canMove = 0x2, /*default*/
	mgWindowFlag_closeButton = 0x4,  /*default*/
	mgWindowFlag_drawBG = 0x8,  /*default*/
	mgWindowFlag_collapseButton = 0x10,  /*default*/
	mgWindowFlag_canResize = 0x20,  /*default*/
	mgWindowFlag_canDock = 0x40,
	mgWindowFlag_noToTop = 0x80,
	mgWindowFlag_noMenuBG = 0x100,

	mgWindowFlag_internal_scrollMode = 0x1000000,
	mgWindowFlag_internal_canScroll = 0x2000000,
	mgWindowFlag_internal_updateContentHeight = 0x4000000,
	mgWindowFlag_internal_isResizeRB = 0x8000000,
	mgWindowFlag_internal_isExpand   = 0x10000000,
	mgWindowFlag_internal_isCloseButton = 0x20000000,
	mgWindowFlag_internal_isMove = 0x40000000,
	mgWindowFlag_internal_visible = 0x80000000
};

enum mgWindowCursorInfo
{
	mgWindowCursorInfo_out,
	mgWindowCursorInfo_titlebar,
	mgWindowCursorInfo_client,
	mgWindowCursorInfo_closeButton,
	mgWindowCursorInfo_collapseButton,
	mgWindowCursorInfo_resizeRB,
};

typedef struct mgWindow_s {
	mgStyle* userStyle; /*optional*/

	void* userData;
	int id;

	mgPoint position;
	mgPoint size;
	mgRect  rect; /*auto calculation in mgUpdateWindow*/
	mgRect  rectTitlebar;

	mgPoint sizeMinimum;

	int flags;
	int flagsInternal;
	int titlebarHeight; /*10*/

	int cursorInfo;
	int cursorInfoOld;

	struct mgContext_s* context;
	struct mgElement_s* rootElement;

	//struct mgFont_s* titlebarFont; /*titlebar text active only with font*/
	struct mgTextProcessor_s* textProcessor;
	const mgUnicodeChar* titlebarText; /*use mgSetWindowTitle function*/
	size_t titlebarTextLen;

	mgRect closeButtonRect;
	mgRect collapseButtonRect;
	mgRect resizeRBRect;

	/*must return != 0 for hide*/
	int(*onClose)(struct mgWindow_s*);

	struct mgWindow_s* left;
	struct mgWindow_s* right;

	struct mgDockPanelWindow_s* dockPanelWindow;
	mgRect dockPanelTabRect;

	int scrollbarWidth;
	int contentHeight;
	int clientHeight;
	mgRect scrollbarBGRect;
	mgRect scrollbarElementRect;

	mgMenu* menu;
	mgRect  menuRect;

	struct mgTexture_s* bgTexture;
} mgWindow;


#endif