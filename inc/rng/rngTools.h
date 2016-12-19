#ifndef CODETOOLS_RNGTOOLS_H
#define CODETOOLS_RNGTOOLS_H
#pragma once

#ifdef EXPORT
#undef EXPORT
#endif // EXPORT

#ifdef RNGTOOLS_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif // RNGTOOLS_EXPORTS

#define BEGIN_RNGTOOLS_NS namespace codetools { namespace rngtools {
#define END_RNGTOOLS_NS }}

#include "rng_lcg.h"

BEGIN_RNGTOOLS_NS

END_RNGTOOLS_NS

#endif // CODETOOLS_RNGTOOLS_H
