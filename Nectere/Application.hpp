#pragma once

#include <chrono>
#include "ACommand.hpp"
#include "Event.hpp"
#include "UIDVector.hpp"

#ifdef WIN32
#define NECTERE_APPLICATION(name) extern "C" std::string __declspec(dllexport) __stdcall Nectere_ApplicationName() {return name;}\
extern "C" void __declspec(dllexport) __stdcall Nectere_ApplicationLoader(const std::shared_ptr<Nectere::Application> &application)
#else
#define NECTERE_APPLICATION(name) extern "C" std::string Nectere_ApplicationName() {return name;}\
extern "C" void Nectere_ApplicationLoader(const std::shared_ptr<Nectere::Application> &application)
#endif

namespace Nectere
{
	class Application
	{
	private:
		uint16_t m_ID;
		std::string m_Name;
		UIDVector<ACommand> m_Commands;
		std::chrono::time_point<std::chrono::system_clock> m_UpdateElapsedTime;

	public:
		Application(uint16_t, const std::string &);
		uint16_t GetID() const { return m_ID; }
		const std::string &GetName() const { return m_Name; };
		void AddCommand(const std::shared_ptr<ACommand> &);
		bool IsEventAllowed(const Event &);
		void Treat(uint16_t, const Event &);
		void Update();
	};
}