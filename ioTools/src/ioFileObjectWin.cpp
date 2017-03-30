#include "ioFileObject.h"

#include <utility>

#ifdef _WINDOWS

struct ioOverlappedData {
	HANDLE hFile;
	const char* data;
	ioPromise* promise;
};

constexpr unsigned k_allocSize = sizeof(OVERLAPPED) + sizeof(ioOverlappedData) + sizeof(ioPromise);
constexpr unsigned k_promiseOffset = sizeof(OVERLAPPED) + sizeof(ioOverlappedData);

void CALLBACK ioFileCompletionRoutine(DWORD dwErrCode, DWORD dwBytesTransferred, LPOVERLAPPED pOverlapped);

ioFileObject::ioFileObject() {}

ioFileObject::ioFileObject(const char* filename) 
{
	Open(filename);
}

ioFileObject::ioFileObject(const wchar_t* filename)
{
	Open(filename);
}

ioFileObject::ioFileObject(const ioFileObject& rhs)
{
	BOOL bResult = FALSE;
	if (rhs.m_file != INVALID_HANDLE_VALUE)
		bResult = DuplicateHandle(
			GetCurrentProcess(), // Source process
			rhs.m_file, // Source handle
			GetCurrentProcess(), // Target process
			&m_file, // Target handle
			0, // ignored by DUPLICATE_SAME_ACCESS
			FALSE, // Inheritable
			DUPLICATE_SAME_ACCESS); // options
}

void ioFileObject::swap(ioFileObject& rhs)
{
	using std::swap;
	swap(m_file, rhs.m_file);
}

ioFileObject::operator bool()
{
	return !(m_file == INVALID_HANDLE_VALUE);
}

uint64_t ioFileObject::Read(char* out, uint64_t amount, uint64_t offset)
{
	return ReadAsync(out, amount, offset).get().length;
}

uint64_t ioFileObject::Write(const char* data, uint64_t amount, uint64_t offset)
{
	return WriteAsync(data, amount, offset).get().length;
}

ioFuture ioFileObject::ReadAsync(char* out, uint64_t amount, uint64_t offset)
{
	char* pOverlappedData = new char[sizeof(k_allocSize)];
	ZeroMemory(pOverlappedData, k_allocSize);
	OVERLAPPED* pOverlapped = new (pOverlappedData) OVERLAPPED;
	ioPromise* pPromise = new (pOverlappedData + k_promiseOffset) ioPromise;
	ioOverlappedData* pOD = new (pOverlappedData + sizeof(OVERLAPPED)) ioOverlappedData { m_file, out, pPromise };

	pOverlapped->Offset = (DWORD)(offset & 0xFFFFFFFF);
	pOverlapped->OffsetHigh = (DWORD)(offset >> 32);
	pOverlapped->hEvent = (HANDLE)pOD;
	BOOL bResult = ReadFileEx(m_file, out, (DWORD)amount, pOverlapped, &ioFileCompletionRoutine);
	if (!bResult)
	{
		throw 0;
	}
	return pPromise->get_future();
}

ioFuture ioFileObject::WriteAsync(const char* data, uint64_t amount, uint64_t offset)
{
	char* pOverlappedData = new char[sizeof(k_allocSize)];
	ZeroMemory(pOverlappedData, k_allocSize);
	OVERLAPPED* pOverlapped = new (pOverlappedData) OVERLAPPED;
	ioPromise* pPromise = new (pOverlappedData + k_promiseOffset) ioPromise;
	ioOverlappedData* pOD = new (pOverlappedData + sizeof(OVERLAPPED)) ioOverlappedData{ m_file, data, pPromise };

	pOverlapped->Offset = (DWORD)(offset & 0xFFFFFFFF);
	pOverlapped->OffsetHigh = (DWORD)(offset >> 32);
	pOverlapped->hEvent = (HANDLE)pOD;
	WriteFileEx(m_file, data, (DWORD)amount, pOverlapped, &ioFileCompletionRoutine);
	return pPromise->get_future();
}

void ioFileObject::Open(const char* filename)
{
	m_file = CreateFileA(
		filename,
		GENERIC_READ | GENERIC_WRITE, // Desired access
		0, // No file sharing
		NULL, // Default security descriptor
		OPEN_EXISTING, // Creation disposition
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // Flags and attributes
		NULL); // template file
}

void ioFileObject::Open(const wchar_t* filename)
{
	m_file = CreateFileW(
		filename,
		GENERIC_READ | GENERIC_WRITE, // Desired access
		0, // No file sharing
		NULL, // Default security descriptor
		OPEN_EXISTING, // Creation disposition
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // Flags and attributes
		NULL); // template file
}

void ioFileObject::Flush()
{
	if (m_file != INVALID_HANDLE_VALUE)
		FlushFileBuffers(m_file);
}

void ioFileObject::Close()
{
	if (m_file != INVALID_HANDLE_VALUE)
		CloseHandle(m_file);
}

void CALLBACK ioFileCompletionRoutine(DWORD dwErrCode, DWORD dwBytesTransferred, LPOVERLAPPED pOverlapped)
{
	ioOverlappedData* pData = (ioOverlappedData*)pOverlapped->hEvent;
	DWORD dwWarning = 0, dwResultTransfered = 0;
	SetLastError(0);
	GetOverlappedResult(pData->hFile, pOverlapped, &dwResultTransfered, FALSE);
	dwWarning = GetLastError();
	pData->promise->set_value(ioResult {dwErrCode, dwWarning, dwBytesTransferred, pData->data});
	delete (char*)pOverlapped;
}

#endif _WINDOWS
