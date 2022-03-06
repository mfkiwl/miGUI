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

#include "framework/mgf.h"
#include "framework/Context.h"
#include "framework/SystemWindowImpl.h"
#include "framework/BackendGDI.h"
#include "framework/Font.h"
#include "framework/FontImpl.h"

#ifdef MG_PLATFORM_WINDOWS

using namespace mgf;

extern mgf::Backend* g_backend;

void BackendGDI_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	g_backend->GetTextSize(text, font, sz);
}

mgTexture BackendGDI_createTexture(mgImage* img)
{
	return g_backend;
}

void BackendGDI_destroyTexture(mgTexture t)
{
}

void BackendGDI_beginDraw()
{
	g_backend->BeginDraw();
}

void BackendGDI_endDraw()
{
	g_backend->EndDraw();
}

void BackendGDI_drawRectangle(
	int reason,
	void* object,
	mgRect* rct,
	mgColor* color1,
	mgColor* color2,
	mgTexture texture, /*optional*/
	mgVec4* UVRegion)
{
	g_backend->DrawRectangle(reason, object, rct, color1, color2, texture, UVRegion);
}

void BackendGDI_drawText(
	int reason,
	void* object,
	mgPoint* position,
	const wchar_t* text,
	int textLen,
	mgColor* color,
	mgFont* font)
{
	g_backend->DrawText(reason, object, position, text, textLen, color, font);
}

void BackendGDI_drawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	g_backend->DrawLine(reason, object, position, where, color, size);
}

mgRect BackendGDI_setClipRect(mgRect* r)
{
	return g_backend->SetClipRect(r);
}

BackendGDI::BackendGDI()
{
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	m_getTextSize = BackendGDI_getTextSize;
	g_backend = this;
	m_gpu = new mgVideoDriverAPI;
	((mgVideoDriverAPI*)m_gpu)->createTexture = BackendGDI_createTexture;
	((mgVideoDriverAPI*)m_gpu)->destroyTexture = BackendGDI_destroyTexture;
	((mgVideoDriverAPI*)m_gpu)->beginDraw = BackendGDI_beginDraw;
	((mgVideoDriverAPI*)m_gpu)->endDraw = BackendGDI_endDraw;
	((mgVideoDriverAPI*)m_gpu)->drawRectangle = BackendGDI_drawRectangle;
	((mgVideoDriverAPI*)m_gpu)->drawText = BackendGDI_drawText;
	((mgVideoDriverAPI*)m_gpu)->setClipRect = BackendGDI_setClipRect;
}

BackendGDI::~BackendGDI()
{
	if (m_defaultFont)
		m_defaultFont->Release();

	if (m_gdiimage_defaultIcons)
		delete m_gdiimage_defaultIcons;
	
	if (m_gdigraphics)
		delete m_gdigraphics;

	if(m_gdiplusToken)
		Gdiplus::GdiplusShutdown(m_gdiplusToken);

	delete m_gpu;
}

void* BackendGDI::GetDefaultIcons()
{
	if (m_gdiimage_defaultIcons)
		return m_gdiimage_defaultIcons;

	m_gdiimage_defaultIcons = new Gdiplus::Image(L"../data/icons.png");
	return m_gdiimage_defaultIcons;
}

void* BackendGDI::GetVideoDriverAPI()
{
	return m_gpu;
}

void BackendGDI::BeginDraw()
{
	m_window->m_hbmOld = (HBITMAP)SelectObject(m_window->m_hdcMem, m_window->m_hbmMem);
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = m_window->m_size.x + m_window->m_borderSize.x;
	r.bottom = m_window->m_size.y + m_window->m_borderSize.y;
	FillRect(m_window->m_hdcMem, &r, (HBRUSH)(COLOR_WINDOW + 1));
}

void BackendGDI::EndDraw()
{
	HDC dc = GetWindowDC(m_window->m_hWnd);
	BitBlt(dc,
		0, 0,
		m_window->m_size.x + m_window->m_borderSize.x,
		m_window->m_size.y + m_window->m_borderSize.y,
		m_window->m_hdcMem,
		0, 0,
		SRCCOPY);
	ReleaseDC(m_window->m_hWnd, dc);
}

mgTexture BackendGDI::CreateTexture(mgImage* img)
{
	return 0;
}

void BackendGDI::DestroyTexture(mgTexture)
{

}

void BackendGDI::DrawRectangle(int reason, void* object, mgRect* rct, mgColor* color1, mgColor* color2,
	mgTexture texture, mgVec4* UVRegion)
{
	unsigned int c1 = mgColorGetAsIntegerARGB(color1);
	unsigned int c2 = mgColorGetAsIntegerARGB(color2);
	HBRUSH brsh = CreateSolidBrush(RGB(c1 & 0xff, (c1 & 0xff00) >> 8, (c1 & 0xff0000) >> 16));
	SelectObject(m_window->m_hdcMem, brsh);

	RECT r;
	r.left = rct->left + m_window->m_borderSize.x;
	r.top = rct->top + m_window->m_borderSize.y;
	r.right = rct->right + m_window->m_borderSize.x;
	r.bottom = rct->bottom + m_window->m_borderSize.y;

	HRGN rgn = 0;

	switch (reason)
	{
	default:
	{
		rgn = CreateRectRgn(
			m_clipRect.left + m_window->m_borderSize.x, 
			m_clipRect.top + m_window->m_borderSize.y,
			m_clipRect.right + m_window->m_borderSize.x,
			m_clipRect.bottom + m_window->m_borderSize.y);
		SelectClipRgn(m_window->m_hdcMem, rgn);
		FillRect(m_window->m_hdcMem, &r, brsh);
	}break;
	case mgDrawRectangleReason_windowCloseButton:
	case mgDrawRectangleReason_windowCollapseButton:
	case mgDrawRectangleReason_popupNextIcon:
	{
		if (texture)
		{
			Gdiplus::Rect gdirct;
			gdirct.X = rct->left + m_window->m_borderSize.x;
			gdirct.Y = rct->top + m_window->m_borderSize.y;
			gdirct.Width = rct->right - rct->left;
			gdirct.Height = rct->bottom - rct->top;
			m_gdigraphics->DrawImage((Gdiplus::Image*)texture, gdirct,
				m_context->m_gui_context->currentIcon.left,
				m_context->m_gui_context->currentIcon.top,
				m_context->m_gui_context->currentIcon.right,
				m_context->m_gui_context->currentIcon.bottom,
				Gdiplus::UnitPixel);
		}
	}break;
	case mgDrawRectangleReason_windowTitlebar:
	case mgDrawRectangleReason_windowBG:
	{
		int roundRectBtm = r.bottom + 1;
		if (reason == mgDrawRectangleReason_windowTitlebar)
			roundRectBtm += 7;

		rgn = CreateRoundRectRgn(r.left, r.top, r.right + 1, roundRectBtm, 7, 7);

		SelectClipRgn(m_window->m_hdcMem, rgn);
		{
			TRIVERTEX vertex[2];
			vertex[0].x = r.left;
			vertex[0].y = r.top;
			vertex[0].Red = (c1 & 0x000000ff) << 8;
			vertex[0].Green = (c1 & 0x0000ff00);
			vertex[0].Blue = (c1 & 0x00ff0000) >> 8;
			vertex[0].Alpha = (c1 & 0xff000000) >> 16;

			vertex[1].x = r.right;
			vertex[1].y = r.bottom;
			vertex[1].Red = (c2 & 0x000000ff) << 8;
			vertex[1].Green = (c2 & 0x0000ff00);
			vertex[1].Blue = (c2 & 0x00ff0000) >> 8;
			vertex[1].Alpha = (c2 & 0xff000000) >> 16;

			GRADIENT_RECT gRect;
			gRect.UpperLeft = 0;
			gRect.LowerRight = 1;
			GradientFill(m_window->m_hdcMem, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
		}
	}break;
	}

	DeleteObject(rgn);
	DeleteObject(brsh);
	SelectClipRgn(m_window->m_hdcMem, 0);
}

void BackendGDI::DrawText(int reason, void* object, mgPoint* position, const wchar_t* text, int textLen,
	mgColor* color, mgFont* font)
{
	SelectObject(m_window->m_hdcMem, font->implementation);
	SetTextColor(m_window->m_hdcMem, mgColorGetAsIntegerBGR(color));
	SetBkMode(m_window->m_hdcMem, TRANSPARENT);

	HRGN rgn = CreateRectRgn(
		m_clipRect.left + m_window->m_borderSize.x,
		m_clipRect.top + m_window->m_borderSize.y,
		m_clipRect.right + m_window->m_borderSize.x,
		m_clipRect.bottom + m_window->m_borderSize.y);

	SelectClipRgn(m_window->m_hdcMem, rgn);
	TextOutW(m_window->m_hdcMem, 
		position->x + m_window->m_borderSize.x, 
		position->y + m_window->m_borderSize.y, 
		text, textLen);
	DeleteObject(rgn);
	SelectClipRgn(m_window->m_hdcMem, 0);
}

void BackendGDI::DrawLine(
	int reason,
	void* object,
	mgPoint* position,
	mgPoint* where,
	mgColor* color,
	int size)
{
	/*Gdiplus::Graphics graphics(hdc);*/
	Gdiplus::Pen      pen(Gdiplus::Color(255, 0, 0, 255));
	m_gdigraphics->DrawLine(&pen, 0, 0, 200, 100);
}

mgRect BackendGDI::SetClipRect(mgRect* r)
{
	mgRect old = m_clipRect;
	m_clipRect = *r;
	return old;
}


void BackendGDI::_createBackbuffer(mgf::SystemWindowImpl* impl)
{
	HDC dc = GetWindowDC(impl->m_hWnd);
	impl->m_hdcMem = CreateCompatibleDC(dc);
	RECT r;
	GetWindowRect(impl->m_hWnd, &r);
	impl->m_hbmMem = CreateCompatibleBitmap(dc,
		r.right - r.left,
		r.bottom - r.top);
	ReleaseDC(impl->m_hWnd, dc);

	if (m_gdigraphics)
		delete m_gdigraphics;
	m_gdigraphics = new Gdiplus::Graphics(impl->m_hdcMem);
}

void BackendGDI::InitWindow(mgf::SystemWindow* w)
{
	if (m_window->m_hdcMem)
		return;
	_createBackbuffer(m_window);
}

void BackendGDI::SetActiveWindow(mgf::SystemWindow* w)
{
	m_window = (mgf::SystemWindowImpl*)w;
}

void BackendGDI::SetActiveContext(mgf::Context* c)
{
	m_context = c;
}

void BackendGDI::UpdateBackbuffer()
{
	if (m_window->m_hdcMem)
		DeleteDC(m_window->m_hdcMem);
	if (m_window->m_hbmMem)
		DeleteObject(m_window->m_hbmMem);
	
	_createBackbuffer(m_window);
}

void BackendGDI::GetTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	SelectObject(m_window->m_hdcMem, font->implementation);
	int c = wcslen(text);
	if (!c)
		return;
	SIZE s;
	GetTextExtentPoint32W(m_window->m_hdcMem, text, c, &s);
	sz->x = s.cx;
	sz->y = s.cy;
}

Font* BackendGDI::CreateFont(const wchar_t* file, int size, bool bold, bool italic)
{
	FontImpl* newFont = new FontImpl;
	newFont->m_context = this->m_context;
	{
		mgFont* f = (mgFont*)malloc(sizeof(mgFont));
		HDC g_dc = GetWindowDC(this->m_window->m_hWnd);
		f->implementation = CreateFontW(
			-MulDiv(size, GetDeviceCaps(g_dc, LOGPIXELSY), 72),
			0, 0, 0,
			bold ? FW_BOLD : FW_NORMAL,
			italic ? 1 : 0,
			0,
			0,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			VARIABLE_PITCH,
			file);
		ReleaseDC(this->m_window->m_hWnd, g_dc);
		newFont->m_font = f;
	}
	newFont->m_backend = this;
	return newFont;
}

void BackendGDI::DestroyFont(Font* f)
{
	if (!f)
		return;
	FontImpl* fi = (FontImpl*)f;
	if (fi->m_font)
	{
		if (fi->m_font->implementation)
			DeleteObject(fi->m_font->implementation);
		free(fi->m_font);
	}
}

Font* BackendGDI::GetDefaultFont()
{
	if(!m_defaultFont)
		m_defaultFont = this->CreateFont(L"Arial", 10, 1, 0);

	return m_defaultFont;
}

#endif