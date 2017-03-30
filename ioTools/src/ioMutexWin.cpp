/*****************************************************************************\
Copyright (c) 2017 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

ioMutex.h -- Implementation of an alertable mutex for the ioTools library

Intended for use with ioFutures
\*****************************************************************************/
#include "ioMutex.h"

ioMutex::ioMutex(bool bOwned)
{
	m_mutex = CreateMutex(NULL,                  // Security context
		                  bOwned ? TRUE : FALSE, // Initial owner
		                  NULL);                 // Name
}

ioMutex::ioMutex(const ioMutex& rhs)
{
	if (rhs.m_mutex)
	{
		DuplicateHandle(GetCurrentProcess(),    // Source process
			            rhs.m_mutex,            // Source handle
			            GetCurrentProcess(),    // Target process
			            &m_mutex,               // Target handle
			            0,                      // Desired acess (ignored)
			            FALSE,                  // Inheritable
			            DUPLICATE_SAME_ACCESS); // Options
	}
}

ioMutex::~ioMutex()
{
	if (m_mutex)
		CloseHandle(m_mutex);
}

bool ioMutex::Lock(bool wait)
{
	DWORD timeout = (wait ? INFINITE : 0);
	DWORD result = WAIT_IO_COMPLETION;
	if (m_mutex)
	{
		while (result == WAIT_IO_COMPLETION)
		{
			result = WaitForSingleObjectEx(m_mutex, timeout, TRUE);
		}
	}
	return result == WAIT_OBJECT_0 || result == WAIT_ABANDONED;
}

void ioMutex::Release()
{
	if (m_mutex)
		ReleaseMutex(m_mutex);
}
