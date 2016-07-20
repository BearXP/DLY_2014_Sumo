extern "C++"
{
	namespace std
	{
	  typedef unsigned int size_t;
	  typedef void (*new_handler)();
	  new_handler set_new_handler(new_handler) throw();
	}

	void operator delete(void*) throw();
	void operator delete[](void*) throw();
	void* operator new(std::size_t) throw();
	void* operator new[](std::size_t) throw();
	void operator delete(void*) throw();
	void operator delete[](void*) throw();
	// Default placement versions of operator new.
	inline void* operator new(std::size_t, void* __p) throw() { return __p; }
	inline void* operator new[](std::size_t, void* __p) throw() { return __p; }
	// Default placement versions of operator delete.
	inline void  operator delete  (void*, void*) throw() { }
	inline void  operator delete[](void*, void*) throw() { }
}

using std::new_handler;
extern "C" void *malloc (std::size_t);
extern "C" void free(void *);
extern void abort() __attribute__((noreturn));
new_handler __new_handler;

void* operator new (std::size_t sz) throw()
{
    void *p;
    if (sz == 0)
        sz = 1;
    p = (void*) malloc(sz);

    while (p == 0)
    {
        new_handler handler = __new_handler;
        if (!handler)
            return 0;
        handler();
        p = (void *) malloc(sz);
    }

    return p;
}

void* operator new[] (std::size_t sz) throw()
{
	return ::operator new(sz);
}

void operator delete (void *ptr) throw ()
{
	if (ptr)
		free (ptr);
}

void operator delete[] (void *ptr) throw ()
{
	::operator delete (ptr);
}

void __cxa_pure_virtual()
{
	//abort();
}
