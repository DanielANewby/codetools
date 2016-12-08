#ifndef CODETOOLS_NEW_H
#define CODETOOLS_NEW_H
#pragma once

#include "libnew.h"

void* operator new(size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void* operator new[](size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void operator delete(void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }
void operator delete[](void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }

#endif // CODETOOLS_NEW_H
