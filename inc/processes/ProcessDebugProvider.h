#ifndef CODETOOLS_PROCESSES_PROCESS_DEBUG_PROVIDER_H
#define CODETOOLS_PROCESSES_PROCESS_DEBUG_PROVIDER_H
#pragma once

#include "process_idebug.h"

BEGIN_PROCESSES_NS

class DebugProvider : public DebugInterface
{
public:
	DebugProvider(ProcessID processId);
	~DebugProvider();

	virtual bool CheckDebugger();

	virtual bool DebugStart();
	virtual bool DebugStop();

	virtual bool Break();
	virtual bool ContinueDebugEvent(ThreadID thread, DWORD continuationStatus);

	virtual bool FlushInstructionCache();

	virtual bool GetThreadContext(ThreadDescriptor thread, ThreadContext* pContext);
	virtual bool SetThreadContext(ThreadDescriptor thread, ThreadContext* pContext);

	virtual bool GetThreadSelectorEntry(ThreadDescriptor thread, DWORD selector, LDT_ENTRY* pEntry);

protected:
	ProcessDescriptor m_hProcess;
	ProcessID m_procID;
};

END_PROCESSES_NS

#endif // CODETOOLS_PROCESSES_PROCESS_DEBUG_PROVIDER_H
