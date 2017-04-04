#define PSAPI_VERSION 1
#include <processes\processes.h>
#include <ctexceptions.h>

#include <psapi.h>

BEGIN_PROCESSES_NS

std::vector<ProcessID> GetAllProcesses()
{
	DWORD cids = 64, received = 0, cb;
	DWORD* pids = 0;
	do {
		delete [] pids;
		cids *= 2;
		pids = new DWORD[cids];
		cb = cids * sizeof(DWORD);
		ZeroMemory(pids, cb);
		if (!EnumProcesses(pids, cb, &received))
			RAISE_SYSTEM_ERROR("EnumProcesses");
	} while (received == cb);
	return std::vector<ProcessID>(pids, pids + (received / sizeof(DWORD)));
}

END_PROCESSES_NS
