#define _CRT_SECURE_NO_WARNINGS
#include <processes/ChildProcess.h>

#include <utility>

BEGIN_PROCESSES_NS

ChildProcess::ChildProcess() {}

ChildProcess::ChildProcess(
	const char* module,
	const char* commandLine,
	const char* cwd,
	bool inheritHandles,
	unsigned createFlags,
	void* environment,
	IODescriptor stdIn,
	IODescriptor stdOut,
	IODescriptor stdErr)
{
	copyStr(m_module, module);
	copyStr(m_commandLine, commandLine);
	copyStr(m_cwd, cwd);
	m_inheritHandles = inheritHandles;
	m_createFlags = createFlags;
	m_environment = environment;
	ProcessDescriptor lp = ::GetCurrentProcess();
	if (stdIn)
		DuplicateHandle(lp, stdIn, lp, &m_childStdIn, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (stdOut)
		DuplicateHandle(lp, stdOut, lp, &m_childStdOut, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (stdErr)
		DuplicateHandle(lp, stdErr, lp, &m_childStdErr, 0, TRUE, DUPLICATE_SAME_ACCESS);
}

ChildProcess::ChildProcess(const ChildProcess& rhs):
	m_procid         { rhs.m_procid },
	m_threadid       { rhs.m_threadid },
	m_inheritHandles { rhs.m_inheritHandles },
	m_createFlags    { rhs.m_createFlags },
	m_environment    { rhs.m_environment }
{
	copyStr(m_module, rhs.m_module);
	copyStr(m_commandLine, rhs.m_commandLine);
	copyStr(m_cwd, rhs.m_cwd);

	ProcessDescriptor lp = ::GetCurrentProcess();
	if (rhs.m_childStdIn)
		DuplicateHandle(lp, rhs.m_childStdIn, lp, &m_childStdIn, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (rhs.m_childStdOut)
		DuplicateHandle(lp, rhs.m_childStdOut, lp, &m_childStdOut, 0, TRUE, DUPLICATE_SAME_ACCESS);
	if (rhs.m_childStdErr)
		DuplicateHandle(lp, rhs.m_childStdErr, lp, &m_childStdErr, 0, TRUE, DUPLICATE_SAME_ACCESS);

	DuplicateHandle(lp,	rhs.m_processDesc, lp, &m_processDesc, 0, FALSE, DUPLICATE_SAME_ACCESS);
	DuplicateHandle(lp, rhs.m_mainThreadDesc, lp, &m_mainThreadDesc, 0, FALSE, DUPLICATE_SAME_ACCESS);
}

ChildProcess::ChildProcess(ChildProcess&& rhs)
{
	swap(rhs);
}

ChildProcess::~ChildProcess()
{
	freeStrings();
	CloseHandle(m_childStdIn);
	CloseHandle(m_childStdOut);
	CloseHandle(m_childStdErr);
	CloseHandle(m_mainThreadDesc);
	CloseHandle(m_processDesc);
}

ChildProcess& ChildProcess::operator=(const ChildProcess& rhs)
{
	swap(ChildProcess { rhs });
	return *this;
}

ChildProcess& ChildProcess::operator=(ChildProcess&& rhs)
{
	swap(rhs);
	return *this;
}

void ChildProcess::SetModule(const char* module)
{
	if (!Started())
		copyStr(m_module, module);
}

void ChildProcess::SetCommandLine(const char* commandLine)
{
	if (!Started())
		copyStr(m_commandLine, commandLine);
}

void ChildProcess::SetWorkingDir(const char* cwd)
{
	if (!Started())
		copyStr(m_cwd, cwd);
}

void ChildProcess::SetEnvironment(void* pEnvironment)
{
	if (!Started())
		m_environment = pEnvironment;
}

void ChildProcess::SetCreationFlags(unsigned flags)
{
	if (!Started())
		m_createFlags = flags;
}

void ChildProcess::InheritHandles(bool bInherit)
{
	if (!Started())
		m_inheritHandles = bInherit;
}

void ChildProcess::SetStdInput(IODescriptor newStdIn)
{
	if (!Started())
	{
		if (m_childStdIn)
			CloseHandle(m_childStdIn);
		m_childStdIn = 0;
		if (newStdIn)
		{
			HANDLE lp = ::GetCurrentProcess();
			DuplicateHandle(lp, newStdIn, lp, &m_childStdIn, 0, TRUE, DUPLICATE_SAME_ACCESS);
		}
	}
}

void ChildProcess::SetStdOutput(IODescriptor newStdOut)
{
	if (!Started())
	{
		if (m_childStdOut)
			CloseHandle(m_childStdOut);
		m_childStdOut = 0;
		if (newStdOut)
		{
			HANDLE lp = ::GetCurrentProcess();
			DuplicateHandle(lp, newStdOut, lp, &m_childStdOut, 0, TRUE, DUPLICATE_SAME_ACCESS);
		}
	}
}

void ChildProcess::SetStdError(IODescriptor newStdErr)
{
	if (!Started())
	{
		if (m_childStdErr)
			CloseHandle(m_childStdErr);
		m_childStdErr = 0;
		if (newStdErr)
		{
			HANDLE lp = ::GetCurrentProcess();
			DuplicateHandle(lp, newStdErr, lp, &m_childStdErr, 0, TRUE, DUPLICATE_SAME_ACCESS);
		}
	}
}

void ChildProcess::ClearStdHandles()
{
	if (!Started())
	{
		CloseHandle(m_childStdIn);
		CloseHandle(m_childStdOut);
		CloseHandle(m_childStdErr);
		m_childStdIn = m_childStdOut = m_childStdErr = 0;
	}
}

void ChildProcess::SetStdHandles(IODescriptor In, IODescriptor out, IODescriptor err)
{
	SetStdInput(In);
	SetStdOutput(out);
	SetStdError(err);
}

void ChildProcess::Start()
{
	BOOL bResult;
	if (!Started())
	{
		PROCESS_INFORMATION pi;
		STARTUPINFOA start_info;
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
		ZeroMemory(&start_info, sizeof(STARTUPINFOA));
		start_info.cb = sizeof(STARTUPINFOA);
		if (m_childStdIn || m_childStdOut || m_childStdErr)
		{
			start_info.dwFlags |= STARTF_USESTDHANDLES;
			start_info.hStdInput = m_childStdIn;
			start_info.hStdOutput = m_childStdOut;
			start_info.hStdError = m_childStdErr;
		}

		bResult = CreateProcessA(
			m_module,
			m_commandLine,
			NULL,
			NULL, 
			m_inheritHandles,
			m_createFlags,
			m_environment,
			m_cwd,
			&start_info,
			&pi);
	
		if (!bResult)
		{

		}
		else
		{
			m_processDesc = pi.hProcess;
			m_mainThreadDesc = pi.hThread;
			m_procid = pi.dwProcessId;
			m_threadid = pi.dwThreadId;
		}
	}
}

bool ChildProcess::Join(unsigned timeout) const
{
	bool result = false;
	if (Started())
		result = (WAIT_OBJECT_0 == WaitForSingleObject(m_processDesc, timeout));
	return result;
}

void ChildProcess::Terminate(unsigned code)
{
	if (Started())
		TerminateProcess(m_processDesc, code);
}

bool ChildProcess::Started() const
{
	return (!!m_processDesc);
}

bool ChildProcess::Running() const
{
	return (Started() && (STILL_ACTIVE == Result()));
}

ThreadResult ChildProcess::Result() const
{
	ThreadResult result;
	GetExitCodeProcess(m_processDesc, &result);
	return result;
}

ChildProcess ChildProcess::Clone() const
{
	return { 
		m_module,
		m_commandLine,
		m_cwd,
		m_inheritHandles,
		m_createFlags,
		m_environment,
		m_childStdIn,
		m_childStdOut,
		m_childStdErr
	};
}

void ChildProcess::copyStr(char*& dest, const char* src)
{
	delete[] dest;
	dest = 0;
	if (src)
	{
		dest = new char[strlen(src) + 1];
		strcpy(dest, src);
	}
}

void ChildProcess::swap(ChildProcess& rhs)
{
	using std::swap;
	swap(m_module, rhs.m_module);
	swap(m_commandLine, rhs.m_commandLine);
	swap(m_cwd, rhs.m_cwd);
	swap(m_createFlags, rhs.m_createFlags);
	swap(m_inheritHandles, rhs.m_inheritHandles);
	swap(m_environment, rhs.m_environment);
	swap(m_childStdIn, rhs.m_childStdIn);
	swap(m_childStdOut, rhs.m_childStdOut);
	swap(m_childStdErr, rhs.m_childStdErr);
	swap(m_processDesc, rhs.m_processDesc);
	swap(m_mainThreadDesc, rhs.m_mainThreadDesc);
	swap(m_procid, rhs.m_procid);
	swap(m_threadid, rhs.m_threadid);
}

END_PROCESSES_NS
