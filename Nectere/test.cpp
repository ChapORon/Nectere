#include <chrono>
#include <iostream>
#include <thread>
#include "AUser.hpp"
#include "ACommand.hpp"
#include "ApplicationManager.hpp"
#include "UserManager.hpp"

using namespace Nectere;

class MyCommand : public ACommand
{
public:
	MyCommand(): ACommand(12) {}

	bool IsValid(const std::string &) const override { return false; }

	void Treat(uint16_t userID, const std::string &msg) override
	{
		std::cout << "Receiving from " << userID << ": \"" << msg << '"' << std::endl;
		if (msg == "Bonjour")
			SendEvent(userID, "Salutation");
		else if (msg == "Comment vas-tu?")
			SendEvent(userID, "Bien, et toi?");
		else if (msg == "Ça vas très bien, merci")
			SendEvent(userID, "C'est super alors");
		else
			SendEvent(userID, "I'm on repeat mode");
	}
};

class MyUser : public AUser
{
private:
	uint16_t m_ApplicationID;
	uint16_t m_NbCall{ 0 };


public:
	MyUser(uint16_t applicationID): m_ApplicationID(applicationID) {}

	void Update() override
	{
		switch (m_NbCall)
		{
		case 0:
		{
			Send({ m_ApplicationID, 12, "Bonjour" });
			break;
		}
		case 1:
		{
			Send({ m_ApplicationID, 12, "Comment vas-tu?" });
			break;
		}
		case 2:
		{
			Send({ m_ApplicationID, 12, "Ça vas très bien, merci" });
			break;
		}
		case 3:
		{
			Send({ m_ApplicationID, 12, "C'est mon 4ème message" });
			break;
		}
		case 4:
		{
			Send({ m_ApplicationID, 12, "Wow déjà 5" });
			break;
		}
		case 5:
		{
			Send({ m_ApplicationID, 12, "Et de 6" });
			break;
		}
		case 6:
		{
			Send({ m_ApplicationID, 12, "Oh un 7ème" });
			break;
		}
		case 7:
		{
			Send({ m_ApplicationID, 12, "8 messages mais où allons nous" });
			break;
		}
		case 8:
		{
			Send({ m_ApplicationID, 12, "It's over 9" });
			break;
		}
		case 9:
		{
			Send({ m_ApplicationID, 12, "10" });
			break;
		}
		}
		++m_NbCall;
	}

	void Receive(const Event &event) override
	{
		std::cout << "{Application: " << event.m_ApplicationID << ", Event Code: " << event.m_EventCode << ", Data: \"" << event.m_Data << "\"}" << std::endl;
	}
};

int main(int argc, char **argv)
{
	uint16_t applicationID;
	if (UserManager *userManager = new UserManager())
	{
		if (auto applicationManager = userManager->GetApplicationManager())
		{
			if (auto application = applicationManager->CreateNewApplication("Test"))
			{
				applicationID = application->GetID();
				if (application->AddCommand<MyCommand>())
				{
					if (auto myUser = userManager->AddUser<MyUser>(applicationID))
					{
						for (int i = 0; i != 10; ++i)
						{
							userManager->Update();
							std::this_thread::sleep_for(std::chrono::seconds(1));
						}
					}
				}
			}
		}
		delete(userManager);
	}
}