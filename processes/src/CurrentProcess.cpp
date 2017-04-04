#include "processes\processes.h"

#include <ctexceptions.h>

BEGIN_PROCESSES_NS

typedef std::map<std::wstring, std::wstring> EnvironmentMap;
#ifdef _WINDOWS
HANDLE DuplicateLocalHandle(HANDLE in)
{
	HANDLE out;
	DuplicateHandle(::GetCurrentProcess(), in, ::GetCurrentProcess(), &out, 0, FALSE, DUPLICATE_SAME_ACCESS);
	return out;
}
#endif // _WINDOWS

class CurrentProcess : public IProcess
{
public:
	CurrentProcess() {
		m_processDesc = DuplicateLocalHandle(::GetCurrentProcess());
		m_procid = GetCurrentProcessId();
	}

	virtual ~CurrentProcess() {
		CloseHandle(m_processDesc);
	}

	virtual void SetModule(const char*) {};
	virtual void SetCommandLine(const char*) {};
	virtual void SetWorkingDir(const char* cwd) { ::SetCurrentDirectoryA(cwd); };
	virtual void SetEnvironment(void*) {}
	virtual void SetCreationFlags(unsigned) {};
	virtual void InheritHandles(bool) {};
	virtual void SetStdInput(IODescriptor newStdIn) { SetStdHandle(STD_INPUT_HANDLE, newStdIn); }
	virtual void SetStdOutput(IODescriptor newStdOut) { SetStdHandle(STD_INPUT_HANDLE, newStdOut); }
	virtual void SetStdError(IODescriptor newStdErr) { SetStdHandle(STD_ERROR_HANDLE, newStdErr); }
	virtual void SetStdHandles(IODescriptor In, IODescriptor out, IODescriptor err) {
		SetStdInput(In);
		SetStdOutput(out);
		SetStdError(err);
	}
	virtual void ClearStdHandles() {
		SetStdInput(NULL);
		SetStdOutput(NULL);
		SetStdError(NULL);
	}

	virtual ProcessDescriptor GetHandle() { return m_processDesc; }
	virtual ProcessID GetID() { return m_procid; }

	virtual std::wstring GetModule() {
		wchar_t cstr[MAX_PATH + 1] = { 0 };
		if (!GetModuleFileNameW(NULL, cstr, MAX_PATH + 1))
			RAISE_SYSTEM_ERROR("GetModuleFileNameA");
		return { cstr };
	}
	virtual std::wstring GetCommand() { return { GetCommandLineW() }; }
	virtual std::wstring GetWorkingDir() {
		wchar_t buffer[MAX_PATH + 1] = { 0 };
		GetCurrentDirectoryW(MAX_PATH + 1, buffer);
		return { buffer };
	}
	virtual EnvironmentMap GetEnvironment() {
		EnvironmentMap envmap;
		wchar_t* env = ::GetEnvironmentStringsW();
		wchar_t* start = env;
		std::wstring key, value;
		for (wchar_t* p = env; *p != L'\0' && *(p - 1) != L'\0'; ++p)
		{
			if (*p == L'=')
			{
				key = std::wstring { start, p };
				start = p + 1;
			}
			if (*p == L'\0')
			{
				value = std::wstring { start, p };
				envmap.insert(std::pair<std::wstring, std::wstring>(key, value));
			}
		}
		FreeEnvironmentStringsW(env);
		return envmap;
	}
	virtual unsigned GetCreationFlags() { return 0; }
	virtual IODescriptor GetStdInput() { return ::GetStdHandle(STD_INPUT_HANDLE); }
	virtual IODescriptor GetStdOuput() { return ::GetStdHandle(STD_OUTPUT_HANDLE); }
	virtual IODescriptor GetStdError() { return ::GetStdHandle(STD_ERROR_HANDLE); }

	virtual void Start() {};
	virtual bool Join(unsigned = -1) const { return true; }
	virtual void Terminate(unsigned exitCode) { ExitProcess(exitCode); }

	virtual bool Started() const { return true; }
	virtual bool Running() const { return true; }

	virtual ThreadResult Result() const {
		return STILL_ACTIVE;
	}

	virtual void* Alloc(void* address, unsigned size, unsigned type, unsigned protection) 
	{
		return VirtualAlloc(address, size, type, protection);
	}
	virtual unsigned ReadMemory(void* address, void* dest, unsigned cb)
	{
		SIZE_T bytesRead = 0;
		BOOL bResult = ReadProcessMemory(m_processDesc, address, dest, cb, &bytesRead);
		if (!bResult)
			RAISE_SYSTEM_ERROR("ReadProcessMemory");
		return bytesRead;
	}
	virtual unsigned WriteMemory(void* address, const void* src, unsigned cb)
	{
		SIZE_T bytesWritten = 0;
		BOOL bResult = WriteProcessMemory(m_processDesc, address, src, cb, &bytesWritten);
		if (!bResult)
			RAISE_SYSTEM_ERROR("WriteProcessMemory");
		return bytesWritten;
	}

private:
	ProcessDescriptor m_processDesc{ 0 };
	ProcessID m_procid{ 0 };
};

///////////////////////////////////////////////////////////////////////////////
// Implement GetCurrentProcess from processes.h
///////////////////////////////////////////////////////////////////////////////
namespace {
	std::shared_ptr<IProcess> g_pCurrentProcess;
	struct CurrentProcessLockTag {
		CurrentProcessLockTag() { InitializeCriticalSection(&m_cs); }
		~CurrentProcessLockTag() { DeleteCriticalSection(&m_cs); }

		void Lock() { EnterCriticalSection(&m_cs); }
		void Release() { LeaveCriticalSection(&m_cs); }
	private:
		CRITICAL_SECTION m_cs;
	} CurrentProcessLock;

}

std::shared_ptr<IProcess> GetCurrentProcess()
{
	if (g_pCurrentProcess == nullptr)
	{
		CurrentProcessLock.Lock();
		if (g_pCurrentProcess == nullptr)
			g_pCurrentProcess = std::make_shared<CurrentProcess>();
		CurrentProcessLock.Release();
	}
	return g_pCurrentProcess;
}

END_PROCESSES_NS
