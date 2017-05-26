/*****************************************************************************\
Copyright (c) 2017 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

processes.h -- Core header for the code tools process tools library

Defines the IProcess interface class, and related factory functions, and
includes the debugger and psapi interface headers to act as a single port
of entry for the library
\*****************************************************************************/
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

#define BEGIN_PROCESSES_NS namespace codetools { namespace processtools {
#define END_PROCESSES_NS }}

#include <memory>
#include <vector>
#include <map>

#ifdef _WINDOWS
#include <Windows.h>
typedef HANDLE ProcessDescriptor;
typedef HANDLE ThreadDescriptor;
typedef DWORD ProcessID;
typedef DWORD ThreadID;
typedef DWORD ThreadResult;
typedef HANDLE IODescriptor;
#endif // _WINDOWS

BEGIN_PROCESSES_NS

// Error types

// Processes generic interface
class DebugInterface;
class PSAPIInterface;

class EXPORT IProcess
{
public:
	virtual ~IProcess() = default;

	virtual void SetModule(const char* module) = 0;
	virtual void SetCommandLine(const char* commandLine) = 0;
	virtual void SetWorkingDir(const char* cwd) = 0;
	virtual void SetEnvironment(void* pEnvironment) = 0;
	virtual void SetCreationFlags(unsigned flags) = 0;
	virtual void InheritHandles(bool bInherit) = 0;
	virtual void SetStdInput(IODescriptor newStdIn) = 0;
	virtual void SetStdOutput(IODescriptor newStdOut) = 0;
	virtual void SetStdError(IODescriptor newStdErr) = 0;
	virtual void SetStdHandles(IODescriptor In, IODescriptor out, IODescriptor err) = 0;
	virtual void ClearStdHandles()= 0;

	virtual ProcessDescriptor GetHandle() = 0;
	virtual ProcessID GetID() = 0;

	virtual std::wstring GetModule() = 0;
	virtual std::wstring GetCommand() = 0;
	virtual std::wstring GetWorkingDir() = 0;
	virtual std::map<std::wstring, std::wstring> GetEnvironment() = 0;
	virtual unsigned GetCreationFlags() = 0;
	virtual IODescriptor GetStdInput() = 0;
	virtual IODescriptor GetStdOuput() = 0;
	virtual IODescriptor GetStdError() = 0;

	virtual void Start() = 0;
	virtual bool Join(unsigned timeout = -1) const = 0;
	virtual void Terminate(unsigned exitCode) = 0;

	virtual bool Started() const = 0;
	virtual bool Running() const = 0;

	virtual ThreadResult Result() const = 0;

	// Memory management
	virtual void* Alloc(void* address, unsigned size, unsigned type, unsigned protection) = 0;
	virtual unsigned ReadMemory(void* address, void* dest, unsigned cb) = 0;
	virtual unsigned WriteMemory(void* address, const void* src, unsigned cb) = 0;

	virtual std::shared_ptr<DebugInterface> GetDebugInterface() = 0;
	virtual std::shared_ptr<PSAPIInterface> GetPSAPIInterface() = 0;
};

// Factory functions
std::shared_ptr<IProcess> GetCurrentProcess();

std::shared_ptr<IProcess> CreateChildProcess(
	const char* module,
	const char* commandLine,
	const char* cwd = 0,
	bool inheritHandles = false,
	unsigned createFlags = 0,
	void* environment = 0,
	IODescriptor stdIn = 0,
	IODescriptor stdOut = 0,
	IODescriptor stdErr = 0
);

std::shared_ptr<IProcess> OpenProcess();

// Utility functions
std::vector<ProcessID> GetAllProcesses();

END_PROCESSES_NS

#include "process_idebug.h"

#endif // CODETOOLS_PROCESSES_H
