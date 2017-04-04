#ifndef CODETOOLS_EXCEPTIONS_H
#define CODETOOLS_EXCEPTIONS_H
#pragma once

#include <string>

#ifdef _WINDOWS
#include <windows.h>
#endif // _WINDOWS

namespace codetools
{
	struct exception {
		exception() {}
		exception(const char* src, unsigned line):m_source(src), m_line(line) {}

		unsigned line() { return m_line; }
		std::string source() { return m_source; }

	protected:
		unsigned m_line;
		std::string m_source;
	};

	struct system_error : exception
	{
		system_error(unsigned error_code):m_error(error_code) {}
		system_error(unsigned error_code, std::string syscall, const char* srcfile, unsigned line):
			exception(srcfile, line),
			m_error(error_code),
			m_syscall(syscall)
		{}

		unsigned GetErrorCode() { return m_error; }
		std::string Format() {
#ifdef _WINDOWS
			DWORD flags = 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS;

			char* pBuffer = 0;
			if (!FormatMessageA(flags, NULL, m_error, 0, (LPSTR)&pBuffer, 0, NULL))
				throw system_error { GetLastError() };
			std::string result { pBuffer };
			LocalFree(pBuffer);
			return result;
#endif
		}

	private:
		unsigned m_error;
		std::string m_syscall;
	};
#ifdef _WINDOWS
#define RAISE_SYSTEM_ERROR(strFunctionName) \
	throw codetools::system_error(GetLastError(), strFunctionName, __FILE__, __LINE__)
#endif // _WINDOWS
}

#endif // CODETOOLS_EXCEPTIONS_H
