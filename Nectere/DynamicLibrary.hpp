#pragma once

#include <string>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define FUNCTION(x) (__stdcall *x)
#define stat _stat
#else
#include <dlfcn.h>
#include <unistd.h>
#define FUNCTION(x) (*x)
#endif

namespace Nectere
{
	class DynamicLibrary
	{
	private:
		std::string m_Path;
		#ifdef WIN32
			HINSTANCE m_LoadedLibrairy;
		#else
			void *m_LoadedLibrairy;
		#endif
		unsigned long m_Timestamp{ 0 };

	public:
		DynamicLibrary(const std::string &path) : m_Path(path)
		{
			const char *cPath = path.c_str();
			struct stat result;
			if (stat(cPath, &result) == 0)
			{
				m_Timestamp = static_cast<unsigned long>(result.st_mtime);
				#ifdef WIN32
					m_LoadedLibrairy = LoadLibrary(cPath);
				#else
					m_LoadedLibrairy = dlopen(cPath, RTLD_NOW);
				#endif
			}
		}

		bool CanReload()
		{
			const char *cPath = m_Path.c_str();
			struct stat result;
			if (stat(cPath, &result) == 0)
			{
				if (result.st_mtime != m_Timestamp)
					return true;
			}
			else
			{
				if (m_LoadedLibrairy != nullptr)
					return true;
			}
			return false;
		}

		void Reload()
		{
			const char *cPath = m_Path.c_str();
			struct stat result;
			if (stat(cPath, &result) == 0)
			{
				if (result.st_mtime != m_Timestamp)
				{
					m_Timestamp = static_cast<unsigned long>(result.st_mtime);
					if (m_LoadedLibrairy != nullptr)
					{
						#ifdef WIN32
							FreeLibrary(m_LoadedLibrairy);
						#else
							dlclose(m_LoadedLibrairy);
						#endif
						m_LoadedLibrairy = nullptr;
					}
					#ifdef WIN32
						m_LoadedLibrairy = LoadLibrary(cPath);
					#else
						m_LoadedLibrairy = dlopen(cPath, RTLD_NOW);
					#endif
				}
			}
			else
			{
				if (m_LoadedLibrairy != nullptr)
				{
					#ifdef WIN32
						FreeLibrary(m_LoadedLibrairy);
					#else
						dlclose(m_LoadedLibrairy);
					#endif
					m_LoadedLibrairy = nullptr;
				}
			}
		}

		template <typename t_FunctionType>
		t_FunctionType Load(const std::string &function)
		{
			#ifdef WIN32
				return (t_FunctionType)GetProcAddress(m_LoadedLibrairy, function.c_str());
			#else
				return (t_FunctionType)dlsym(m_LoadedLibrairy, function.c_str());
			#endif
		}

		unsigned long GetTimestamp() const { return m_Timestamp; }

		std::string GetPath() const { return m_Path; }

		explicit operator bool() const
		{
			return m_LoadedLibrairy != nullptr;
		}

		~DynamicLibrary()
		{
			#ifdef WIN32
				FreeLibrary(m_LoadedLibrairy);
			#else
				dlclose(m_LoadedLibrairy);
			#endif
		}
	};
}