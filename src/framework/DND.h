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
#ifndef _MGF_DND_H_
#define _MGF_DND_H_

class DNDWin32DropTarget;
class DNDWin32DropSource;
class DNDWin32DropDataObject;

namespace mgf
{
	/// <summary>
	/// Callback for Drag and Drop class
	/// </summary>
	class DNDCallback
	{
	public:
		DNDCallback() {}
		virtual ~DNDCallback() {}

		enum NatureType
		{
			NatureType_user, // probably clicked inside of app or maybe its something that I not implemented...
			NatureType_text,
			NatureType_file  // or folder
		};

		struct NatureFile
		{
			NatureFile() {}
			char** fileList;
			int fileListSize = 0;
		};

		struct Nature
		{
			Nature() {}
			union
			{
				NatureFile _f;
			};
			NatureType m_type = NatureType::NatureType_user;
		};

		/// <summary>
		/// Need to know the nature of current dnd.
		/// It can be file, or text from other app.
		/// </summary>
		virtual bool OnDragEnter(int x, int y, Nature*) = 0;

		/// <summary>
		/// Will be called when moving mouse.
		/// Return true if cursor in position what you need.
		/// x and y is cursor position.
		/// </summary>
		virtual bool OnDragOver(int x, int y) = 0;

		virtual void OnDrop(Nature*) = 0;
	};

	/// <summary>
	/// Main interface for drag and drop.
	/// Users must create their own implementation.
	/// DNDWin32 is implementation for Windows.
	/// </summary>
	class DND : public BaseClass
	{
	protected:
		DNDCallback* m_cb = 0;
	public:
		DND();
		virtual ~DND();

		/// <summary>
		/// Start drag and drop.
		/// Must call by user.
		/// </summary>
		virtual void DoDND() = 0;
	};

	/// <summary>
	/// Implementation for Windows
	/// </summary>
	class DNDWin32 : public DND
	{
		bool m_isReady = false;
		DNDWin32DropTarget* m_dropTarget = 0;
		DNDWin32DropSource* m_dropSource = 0;
		DNDWin32DropDataObject* m_dropDataObject = 0;
	public:
		DNDWin32(DNDCallback*, HWND hWnd);
		virtual ~DNDWin32();
		
		virtual void DoDND() override;
	};
}

#endif