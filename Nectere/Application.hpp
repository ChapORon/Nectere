#pragma once

#include <chrono>
#include "AApplicationHandler.hpp"
#include "ACommand.hpp"
#include "Dp/Node.hpp"
#include "Event.hpp"
#include "Ptr.hpp"
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
	class ApplicationManager;
	class Application
	{
		friend class ACommand;
		friend class AApplicationHandler;
		friend class ApplicationManager;
	private:
		uint16_t m_ID;
		std::atomic_bool m_IsReloading;
		std::string m_Name;
		UIDSet<ACommand> m_Commands;
		Ptr<ApplicationManager> m_ApplicationManager;
		AApplicationHandler *m_Handler;
		std::chrono::time_point<std::chrono::system_clock> m_UpdateElapsedTime;

	private:
		void SetName(const std::string &name) { m_Name = name; };
		ACommand *InternalAddCommand(ACommand *);
		void SendEvent(uint16_t, bool, uint16_t, const std::string &);
		void SendEvent(const std::vector<uint16_t> &, bool, uint16_t, const std::string &);
		void SendEvent(uint16_t, uint16_t, const std::string &);
		void SendEvent(const std::vector<uint16_t> &, uint16_t, const std::string &);
		void SendError(uint16_t, uint16_t, const std::string &);
		void SendError(const std::vector<uint16_t> &, uint16_t, const std::string &);
		//TODO: Add GetCommand<t_CommandType>

	public:
		NECTERE_EXPORT Application(uint16_t, const std::string &, const Ptr<ApplicationManager> &);
		inline uint16_t GetID() const { return m_ID; }
		inline const std::string &GetName() const { return m_Name; };
		inline void SetHandler(AApplicationHandler *handler) { m_Handler = handler; };
		template <typename t_CommandType, typename ...t_Arg>
		inline Ptr<t_CommandType> AddCommand(t_Arg&&... args) { return Ptr(dynamic_cast<t_CommandType *>(InternalAddCommand(new t_CommandType(std::forward<t_Arg>(args)...)))); }
		NECTERE_EXPORT bool IsEventAllowed(const Event &, bool &);
		NECTERE_EXPORT void Treat(uint16_t, const Event &);
		NECTERE_EXPORT void Update();
		NECTERE_EXPORT void BeforeReloading(Dp::Node &);
		NECTERE_EXPORT void AfterReloading(const Dp::Node &);
		NECTERE_EXPORT bool IsAuthenticated(uint16_t userId);
		inline const std::vector<Ptr<ACommand>> &GetCommands() const { return m_Commands.GetElements(); }
	};
}