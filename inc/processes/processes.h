#ifndef CODETOOLS_PROCESSES_H
#define CODETOOLS_PROCESSES_H
#pragma once

#include <cstdint>

#ifdef EXPORT
#undef EXPORT
#endif // EXPORT

#ifdef PROCESSES_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif // PROCESSES_EXPORTS

#define BEGIN_PROCESSES_NS namespace codetools { namespace hashtools {
#define END_PROCESSES_NS }}

#ifdef _WINDOWS
#include <Windows.h>
typedef HANDLE ProcessDescriptor;
typedef HANDLE ThreadDescriptor;
typedef DWORD ProcessID;
typedef DWORD ThreadID;
typedef DWORD ThreadResult;
typedef HANDLE IODescriptor;
#endif // _WINDOWS

#endif // CODETOOLS_PROCESSES_H
