#include "Script/Function.hpp"

#include "Script/CurrentEnvironment.hpp"
#include "Script/CallStack.hpp"
#include "Script/ResolutionTree.hpp"
#include "Script/ResolutionTreeUtils.hpp"
#include "Script/Script.hpp"
#include "Script/VariableType.hpp"
#include "StringUtils.hpp"

namespace Nectere
{
	namespace Script
	{

		Function::Function(Script *script) : m_Script(script) {}

		bool Function::AddBody(const std::string &content)
		{
			if (content.size() == 0)
				return true;
			std::vector<std::string> fragments;
			StringUtils::SmartSplit(fragments, content, "}", true);
			std::vector<std::string> instructions;
			for (const auto &fragment : fragments)
				StringUtils::SmartSplit(instructions, fragment, ";", false);
			//#todo Implement body syntax check
			return true;
		}

		bool Function::Compile()
		{
			if (!super::Compile(CurrentEnvironment(m_Script)))
				return false;
			for (const auto &instruction : m_UncompiledBody)
			{
				ResolutionTree *bodyNode = ResolutionTreeUtils::Compile(instruction);
				if (!bodyNode)
				{
					Clear();
					return false;
				}
				m_Body.emplace_back(bodyNode);
			}
			return true;
		}

		bool Function::Call(CallStack &parameters)
		{
			for (auto *instruction : m_Body)
			{
				if (!instruction->Resolve(m_Script, parameters))
					return false;
			}
			return true;
		}

		void Function::Clear()
		{
			for (auto *instruction : m_Body)
				delete(instruction);
		}

		Function::~Function()
		{
			Clear();
		}
	}
}