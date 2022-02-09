#pragma once

#include <string>
#include <vector>
#include "Ptr.hpp"

namespace Nectere
{
	class Application;
	class IApplicationManager;
	class ACommand
	{
		friend class Application;
	private:
		uint16_t m_CommandID;
		std::string m_Name;
		Ptr<Application> m_Application;

	protected:
		Ptr<IApplicationManager> m_ApplicationManager;

	private:
		void SetApplication(const Ptr<Application> &, const Ptr<IApplicationManager> &);

	protected:
		NECTERE_EXPORT void SendEvent(uint16_t, const std::string &);
		NECTERE_EXPORT void SendEvent(const std::vector<uint16_t> &, const std::string &);
		NECTERE_EXPORT void SendError(uint16_t, const std::string &);
		NECTERE_EXPORT void SendError(const std::vector<uint16_t> &, const std::string &);
		NECTERE_EXPORT bool IsAuthenticated(uint16_t userId);

	public:
		NECTERE_EXPORT ACommand(uint16_t, const std::string &);
		inline uint16_t GetID() const { return m_CommandID; }
		inline const std::string &GetName() const { return m_Name; };
		NECTERE_EXPORT virtual bool IsValid(const std::string &) const = 0;
		NECTERE_EXPORT virtual void Treat(uint16_t, uint16_t, const std::string &) = 0;
		NECTERE_EXPORT virtual void OnInit() {}
		NECTERE_EXPORT virtual void Update() {}
	};
}