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
		void SendEvent(uint16_t, const std::string &);
		void SendEvent(const std::vector<uint16_t> &, const std::string &);

	public:
		ACommand(uint16_t, const std::string &);
		uint16_t GetID() const { return m_CommandID; }
		const std::string &GetName() const { return m_Name; };
		virtual bool IsValid(const std::string &) const = 0;
		virtual void Treat(uint16_t, const std::string &) = 0;
		virtual void OnInit() {}
		virtual void Update() {}
	};
}