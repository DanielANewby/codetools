#include "processes\ProcessDebugProvider.h"

#include "ctexceptions.h"

BEGIN_PROCESSES_NS

DebugProvider::DebugProvider(ProcessID processId)
{
	m_procID = processId;
	m_hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_procID);
	if (!m_hProcess)
		RAISE_SYSTEM_ERROR("OpenProcess");
}

DebugProvider::~DebugProvider()
{
	CloseHandle(m_hProcess);
}

bool DebugProvider::CheckDebugger() 
{
	BOOL result = FALSE;
	CheckRemoteDebuggerPresent(m_hProcess, &result);
	return !!result;
}

bool DebugProvider::DebugStart()
{
	return !!DebugActiveProcess(m_procID);
}

bool DebugProvider::DebugStop()
{
	return !!DebugActiveProcessStop(m_procID);
}

bool DebugProvider::Break()
{
	return !!DebugBreakProcess(m_hProcess);
}

bool DebugProvider::ContinueDebugEvent(ThreadID thread, DWORD continuationStatus)
{
	return !!::ContinueDebugEvent(m_procID, thread, continuationStatus);
}

bool DebugProvider::FlushInstructionCache()
{
	return !!::FlushInstructionCache(m_hProcess, NULL, 0);
}

bool DebugProvider::GetThreadContext(ThreadDescriptor thread, ThreadContext* pContext)
{
	return !!::GetThreadContext(thread, pContext);
}

bool DebugProvider::SetThreadContext(ThreadDescriptor thread, ThreadContext* pContext)
{
	return !!::SetThreadContext(thread, pContext);
}

bool DebugProvider::GetThreadSelectorEntry(ThreadDescriptor thread, DWORD selector, LDT_ENTRY* pEntry)
{
	return !!::GetThreadSelectorEntry(thread, selector, pEntry);
}

END_PROCESSES_NS
