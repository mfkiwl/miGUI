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
#ifndef _MG_FRAMEWORK_H_
#define _MG_FRAMEWORK_H_

#include "mgfConfig.h"

// VS: Zc:__cplusplus compiler option
#if __cplusplus > 199711L
#define MGF_NOEXCEPT noexcept
#define MGF_CXX11_RVALUE_REFERENCES
#else
#define MGF_NOEXCEPT
#endif

#ifdef _MSC_VER
#define MGF_FORCEINLINE __forceinline
#else
#define MGF_FORCEINLINE inline
#endif

// For static library ('mgf' project) use preprocessor: MG_NO_DLL

#include "mgDefs.h"
#include "mgForward.h"
#include "mgPoint.h"
#include "mgRect.h"
#include "mgVec4.h"
#include "mgColor.h"
#include "mgFont.h"
#include "mgImage.h"
#include "mgTexture.h"
#include "mgTexture.h"
#include "mgStyle.h"
#include "mgInputContex.h"
#include "mgSystemWindowData.h"


#include "framework/forward.h"
#include "framework/BaseClass.h"
#include "framework/Ptr.h"
#include "framework/SystemWindow.h"
#include "framework/Backend.h"
#include "framework/Context.h"
#include "framework/Framework.h"

#endif