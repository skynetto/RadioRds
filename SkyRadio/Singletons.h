#ifndef Singletons_h__
#define Singletons_h__

#include <stdexcept>
#include <windows.h>

/*------------------------------------------------------------------------------
no_copy - Disallow copy semantics
------------------------------------------------------------------------------*/
class no_copy
{
	protected:
		no_copy() {}
		~no_copy() {}
	private:
		no_copy(const no_copy&);
		const no_copy& operator=(const no_copy&);
};//no_copy

/*------------------------------------------------------------------------------
scoped_lock - 
	Generalized template for Acquire()/Release() operations in the constructor 
	and destructor respectively
------------------------------------------------------------------------------*/
template<class sync_t>
class scoped_lock : public no_copy
{
	sync_t &m_sync;
public:
	explicit scoped_lock(sync_t &s) : m_sync(s) {m_sync.Acquire();}
	~scoped_lock() {m_sync.Release();}
};//scoped_lock

/*------------------------------------------------------------------------------
CriticalSection - C++ version of a Win32 CS
------------------------------------------------------------------------------*/
class CriticalSection : public no_copy
{
	CRITICAL_SECTION m_cs;

public:
	typedef scoped_lock<CriticalSection> scoped_lock;

	CriticalSection(bool bUseSpinCount = true, DWORD SpinCount = 4000) 
	{
		if (bUseSpinCount && 
			!InitializeCriticalSectionAndSpinCount(&m_cs, SpinCount))
			throw std::runtime_error("InitCSAndSpinCount failure");
		else
			InitializeCriticalSection(&m_cs);
	}//constructor

	~CriticalSection() {::DeleteCriticalSection(&m_cs);}
	void Acquire() {::EnterCriticalSection(&m_cs);}
	void Release() {::LeaveCriticalSection(&m_cs);}
};//CriticalSection

//------------------------------------------------------------------------------
// Interlocked*Acquire/Release() functions only for Itanium Processor Family,
// (IPF), which we map to non-IPF Interlocked functions for other platforms
#if !defined(_M_IA64) && !defined(__IA64__)
#   define InterlockedExchangeAddAcquire  InterlockedExchangeAdd
#   define InterlockedExchangeAddRelease  InterlockedExchangeAdd
#endif

/*------------------------------------------------------------------------------
Once - 
	C++ implementation of "one-time initialization" among multiple threads. You
	must ensure that construction of a Once object is thread-safe. For example,
	Once definitions at global scope will ensures m_once will be zero'd out
	during global object construction (which is single threaded). You can then
	call one of the DoOnce() methods, or construct a Sentry object on the stack
	to determine if one-time initialization should occur.

	This is basically an implementation of double-check locking (DCL), which
	can be tricky to get right - see the following:

		"C++ and the Perils of Double-Checked Locking"
		http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf

	The proper solution is to ensure proper ordering/visibility of changes to 
	m_once. This is achieved via acquire/release memory barriers, via
	InterlockedExchangeAddAcquire and InterlockedExchangeAddRelease.
------------------------------------------------------------------------------*/
class Once : public no_copy
{
	LONG m_once;
	CriticalSection m_cs;
	friend class Sentry;

public:
	class Sentry : public no_copy
	{
		Once &m_o;
		bool m_bOwner;
		bool m_bAcquiredCS;

	public:
		explicit Sentry(Once &o) : m_o(o), m_bOwner(false), m_bAcquiredCS(false)
		{
			if (InterlockedExchangeAddAcquire(&m_o.m_once, 0))
				return; // m_once is non-zero, "once" has occurred already

			// "once" hasn't happened yet
			m_o.m_cs.Acquire();
			m_bAcquiredCS = true;
			m_bOwner = (m_o.m_once == 0);
		}//constructor

		~Sentry()
		{
			if (m_bOwner)
				InterlockedExchangeAddRelease(&m_o.m_once, 1);
			if (m_bAcquiredCS)
				m_o.m_cs.Release();
		}//destructor

		bool OnceOwner() const {return m_bOwner;}
	};//Sentry

	Once() : m_once(0) {}

	template <typename Obj_t>
	bool DoOnce(Obj_t *obj, void (Obj_t::*mfn)())
	{
		Sentry s(*this);
		if (s.OnceOwner())
			(obj->*mfn)();
		return s.OnceOwner();
	}//DoOnce

	template<class Functor_t>
	bool DoOnce(const Functor_t &f)
	{
		Sentry s(*this);
		if (s.OnceOwner())
			f();
		return s.OnceOwner();
	}//DoOnce

#ifdef POINTLESS
	// Once isn't enough for some folks...
	// NOTE - Once object should be acquired before calling this method
	void Reset() {InterlockedExchange(&m_once, 0);}

	// Use our member CriticalSection for Acquire/Release semantics
	void Acquire() {m_cs.Acquire();}
	void Release() {m_cs.Release();}
	typedef scoped_lock<Once> scoped_lock;
#endif
};//Once

/*------------------------------------------------------------------------------
SingletonConstruct - 
	Template class for constructing a singleton in an efficient and thread safe 
	manner. This object must be a static member of the class that you want to be 
	a singleton. This ensures m_once will be constructed during global object
	construction (which is single threaded).
------------------------------------------------------------------------------*/
template <class T>
class SingletonConstruct : public no_copy
{
	Once m_once;
	T *m_p;

public:
	SingletonConstruct() : m_p(0) 
	{

	}
	~SingletonConstruct() 
	{
		Unload();
	}

	// NOTE: 1) Caller must guarantee that no references are being held
	//       2) This is thread-safe with respect to calls to Instance() when 
	//          POINTLESS is defined.
	//       3) Guaranteeing 1) makes 2) pointless...
	void Unload()
	{
#ifdef POINTLESS
		Once::scoped_lock l(m_once);
		// reset the Once object, this will block any calls to Instance() 
		// while we have the Once object acquired - ensuring that construction
		// and destruction are mutually exclusive and thread-safe
		m_once.Reset();
#endif
		if (m_p) // yes, I know delete 0 doesn't hurt...
		{
			delete m_p;
			m_p = 0;
		}//if
	}//Unload

	T& Instance()
	{
		Once::Sentry s(m_once);
		if (s.OnceOwner())
			m_p = new T;
		return *m_p;
	}//Instance
};//SingletonConstruct
#endif // Singletons_h__