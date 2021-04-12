#pragma once

#ifdef WIN32
#include <windows.h>
#define FUNCTION(x) (__stdcall *x)

namespace Nectere
{
	class DynamicLibrary
	{
	private:
		HINSTANCE m_LoadedLibrairy;

	public:
		DynamicLibrary(const std::string &path)
		{
			m_LoadedLibrairy = LoadLibrary(path.c_str());
		}

		template <typename T>
		T Load(const std::string &function)
		{
			return (T)GetProcAddress(m_LoadedLibrairy, function.c_str());
		}

		explicit operator bool() const
		{
			return m_LoadedLibrairy != nullptr;
		}

		~DynamicLibrary()
		{
			FreeLibrary(m_LoadedLibrairy);
		}
	};
}
#else
#include <dlfcn.h>
#define FUNCTION(x) (*x)

namespace Nectere
{
	class DynamicLibrary
	{
	private:
		void *m_LoadedLibrairy;

	public:
		DynamicLibrary(const std::string &path)
		{
			m_LoadedLibrairy = dlopen(path.c_str(), RTLD_NOW);
		}

		template <typename T>
		T Load(const std::string &function)
		{
			return (T)dlsym(m_LoadedLibrairy, function.c_str());
		}

		explicit operator bool() const
		{
			return m_LoadedLibrairy != nullptr;
		}

		~DynamicLibrary()
		{
			dlclose(m_LoadedLibrairy);
		}
	};
}
#endif