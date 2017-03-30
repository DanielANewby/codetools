#ifndef CODETOOLS_PROCESSES_CHILDPROCESS_H
#define CODETOOLS_PROCESSES_CHILDPROCESS_H
#pragma once

#include "processes.h"

#include <utility>

BEGIN_PROCESSES_NS

class EXPORT ChildProcess
{
public:
	ChildProcess();
	ChildProcess(const char* module,
		const char* commandLine,
		const char* cwd = 0,
		bool inheritHandles = false,
		unsigned createFlags = 0,
		void* environment = 0,
		IODescriptor stdIn = 0,
		IODescriptor stdOut = 0,
		IODescriptor stdErr = 0);
	ChildProcess(const ChildProcess&); 	// Copy semantics duplicate open handles
	ChildProcess(ChildProcess&&); // Move semantics swap (possibly open) handles
	~ChildProcess();

	ChildProcess& operator=(const ChildProcess&); // Copy semantics duplicate open handles
	ChildProcess& operator=(ChildProcess&&); // Move semantics swap (possibly open) handles

	void SetModule(const char* module);
	void SetCommandLine(const char* commandLine);
	void SetWorkingDir(const char* cwd);
	void SetEnvironment(void* pEnvironment);
	void SetCreationFlags(unsigned flags);
	void InheritHandles(bool bInherit);
	void SetStdInput(IODescriptor newStdIn);
	void SetStdOutput(IODescriptor newStdOut);
	void SetStdError(IODescriptor newStdErr);
	void SetStdHandles(IODescriptor In, IODescriptor out, IODescriptor err);
	void ClearStdHandles();

	void Start();
	bool Join(unsigned timeout = -1) const;
	void Terminate(unsigned exitCode);

	bool Started() const;
	bool Running() const;
	
	ThreadResult Result() const;

	// Creates a new unstarted ChildProcess based on the current one
	ChildProcess Clone() const;

	void swap(ChildProcess& rhs);
	void swap(ChildProcess&& rhs) { swap(rhs); }

private:
	void copyStr(char*& dest, const char* src);
	void freeStrings() {
		delete [] m_module;
		delete [] m_commandLine;
		delete [] m_cwd;
		m_module = m_commandLine = m_cwd = 0;
	}

	char* m_module { 0 };
	char* m_commandLine { 0 };
	char* m_cwd { 0 };

	bool m_inheritHandles { true };
	unsigned m_createFlags { 0 };
	void* m_environment { 0 };

	IODescriptor m_childStdIn  { 0 };
	IODescriptor m_childStdOut { 0 };
	IODescriptor m_childStdErr { 0 };

	ProcessDescriptor m_processDesc { 0 };
	ThreadDescriptor m_mainThreadDesc { 0 };
	ProcessID m_procid { 0 };
	ThreadID m_threadid { 0 };
};

void swap(ChildProcess& lhs, ChildProcess& rhs) { lhs.swap(rhs); }

END_PROCESSES_NS

#endif // CODETOOLS_PROCESSES_CHILDPROCESS_H
