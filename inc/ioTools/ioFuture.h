#ifndef IOTOOLS_IOFUTURE_H
#define IOTOOLS_IOFUTURE_H
#pragma once

#include <cstdint>

#include <memory>
#include <exception>
#include "ioMutex.h"

struct ioResult
{
	unsigned error;
	unsigned warning;
	uint64_t length;
	const char* data;
};

struct ioFuture_SharedSate
{
	ioMutex shared_mutex;
	std::exception_ptr exception;
	std::unique_ptr<ioResult> result;
};

class ioPromise
{
public:
	ioPromise():m_future { std::make_shared<ioFuture_SharedSate>() } { m_state = m_future.m_state; }
	ioPromise(ioPromise&&) = default;
	ioPromise& operator=(ioPromise&&) = default;
	~ioPromise() = default;

	ioPromise(const ioPromise&) = delete;
	ioPromise& operator= (const ioPromise&) = delete;

	void swap(ioPromise& other) { m_state.swap(other.m_state); }

	void set_value(const ioResult& result)
	{
		const state_guard _sg { m_state };
		m_state->result = std::make_unique<ioResult>(result);
	}

	void set_exception(std::exception_ptr& p)
	{
		const state_guard _sg{ m_state };
		m_state->exception = p;
	}

	ioFuture get_future() {
		return std::move(m_future);
	}

private:
	typedef std::shared_ptr<ioFuture_SharedSate> StatePtr;
	
	struct state_guard {
		state_guard(StatePtr& _p):p(_p) { 
			if (!p) { /* throw no-state */ }
			if (!p->shared_mutex.Lock(false)) { /* throw future error */ }
			if (p->result || p->exception) { /* throw already set */ }
		}
		~state_guard() { 
			if (p)
				p->shared_mutex.Release();
		}

		state_guard(const state_guard&) = delete;
		state_guard(state_guard&&) = delete;
		state_guard& operator=(const state_guard&) = delete;
		state_guard& operator=(state_guard&&) = delete;

	private:
		StatePtr& p;
	};

	StatePtr m_state;
	ioFuture m_future;
};

class ioFuture
{
public:
	ioFuture(ioFuture&&) = default;
	ioFuture& operator= (ioFuture&&) = default;
	ioFuture(const ioFuture&) = delete;
	ioFuture& operator= (const ioFuture&) = delete;

	void swap(ioFuture& other) { m_state.swap(other.m_state); }

	ioResult get() {
		if (!m_state) { /* throw no-state */ }
		struct Lock { 
			Lock(ioMutex& r):m(r) { m.Lock(); }
			~Lock() { m.Release(); }
			ioMutex& m;
		} guard(m_state->shared_mutex);
		if (m_state->exception)
			std::rethrow_exception(m_state->exception);
	}

private:
	typedef std::shared_ptr<ioFuture_SharedSate> StatePtr;
	
	StatePtr m_state;

	ioFuture(StatePtr&& ptr):m_state(std::move(ptr)) { if (!m_state) { /* throw aleady gotten */ } }
	~ioFuture() = default;

	ioFuture Clone() { 
		ioFuture clone(nullptr);
		clone.m_state = m_state;
		return std::move(clone);
	}

	friend class ioPromise;
};

inline void swap(ioPromise& lhs, ioPromise& rhs) { lhs.swap(rhs); }
inline void swap(ioFuture& lhs, ioFuture& rhs) { lhs.swap(rhs); }

#endif // IOTOOLS_IOFUTURE_H
