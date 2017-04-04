/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

process_idebug -- Defines the process debugging interface
\*****************************************************************************/
#ifndef CODETOOLS_PROCESSES_PROCESS_IDEBUG_H
#define CODETOOLS_PROCESSES_PROCESS_IDEBUG_H
#pragma once

#include "processes.h"

BEGIN_PROCESSES_NS

class DebugInterface
{
public:
	typedef CONTEXT ThreadContext;

	virtual ~DebugInterface() = default;

	virtual bool CheckDebugger() = 0;
	
	virtual bool DebugStart() = 0;
	virtual bool DebugStop() = 0;

	virtual bool Break() = 0;
	virtual bool ContinueDebugEvent(ThreadID thread, DWORD continuationStatus) = 0;

	virtual bool FlushInstructionCache() = 0;

	virtual bool GetThreadContext(ThreadDescriptor thread, ThreadContext* pContext) = 0;
	virtual bool SetThreadContext(ThreadDescriptor thread, ThreadContext* pContext) = 0;
	
	virtual bool GetThreadSelectorEntry(ThreadDescriptor thread, DWORD selector, LDT_ENTRY* pEntry) = 0;
};

END_PROCESSES_NS

#endif // CODETOOLS_PROCESSES_PROCESS_IDEBUG_H
