#pragma once

#include <string>
#include <vector>

namespace Nectere
{
	class Application;
	class ACommand
	{
		friend class Application;
	private:
		uint16_t m_CommandID;
		Application *m_Application;

	private:
		void SetApplication(Application *application) { m_Application = application; }

	protected:
		void SendEvent(uint16_t, const std::string &);
		void SendEvent(const std::vector<uint16_t> &, const std::string &);

	public:
		ACommand(uint16_t);
		uint16_t GetID() const { return m_CommandID; }
		virtual bool IsValid(const std::string &) const = 0;
		virtual void Treat(uint16_t, const std::string &) = 0;
		virtual void OnInit() {}
		virtual void Update() {}
	};
}