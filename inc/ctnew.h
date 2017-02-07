/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

ctnew.h -- ctnew operators declaration and libnew entry points
\*****************************************************************************/
#ifndef CODETOOLS_NEW_H
#define CODETOOLS_NEW_H
#pragma once

#include "libnew.h"

void* operator new(size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void* operator new[](size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void operator delete(void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }
void operator delete[](void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }

#endif // CODETOOLS_NEW_H
