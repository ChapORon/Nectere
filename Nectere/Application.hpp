#pragma once

#include <chrono>
#include "AApplicationHandler.hpp"
#include "ACommand.hpp"
#include "Event.hpp"
#include "UIDSet.hpp"

#ifdef WIN32
#define NECTERE_APPLICATION(name) extern "C" __declspec(dllexport) const char * __stdcall Nectere_ApplicationName() {return name;}\
extern "C" void __declspec(dllexport) __stdcall Nectere_ApplicationLoader(Nectere::Application *application)
#else
#define NECTERE_APPLICATION(name) extern "C" const char * Nectere_ApplicationName() {return name;}\
extern "C" void Nectere_ApplicationLoader(Nectere::Application *application)
#endif

namespace Nectere
{
	class Application
	{
		friend class AApplicationHandler;
		friend class ApplicationManager;
	private:
		uint16_t m_ID;
		std::atomic_bool m_IsReloading;
		std::string m_Name;
		UIDSet<ACommand> m_Commands;
		AApplicationHandler *m_Handler;
		std::chrono::time_point<std::chrono::system_clock> m_UpdateElapsedTime;

	private:
		void SetName(const std::string &name) { m_Name = name; };

	public:
		Application(uint16_t, const std::string &);
		uint16_t GetID() const { return m_ID; }
		const std::string &GetName() const { return m_Name; };
		void SetHandler(AApplicationHandler *handler) { m_Handler = handler; };
		void AddCommand(ACommand *);
		bool IsEventAllowed(const Event &);
		void Treat(uint16_t, const Event &);
		void Update();
		void BeforeReloading();
		void AfterReloading();
	};
}