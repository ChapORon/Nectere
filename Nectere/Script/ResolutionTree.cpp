#include "Script/ResolutionTree.hpp"

#include "Logger.hpp"

namespace Nectere
{
	namespace Script
	{
		ResolutionTree::ResolutionTree(IResolutionTreeOperation *operation) : m_HasResolve(true), m_Operation(operation)
		{
			if (!m_Operation)
				LOG_SCRIPT_COMPILATION(LogType::Error, "Cannot build resolution tree: Out of memory");
		}

		ResolutionTree::ResolutionTree(IResolutionTreeOperation *operation, const std::vector<ResolutionTree *> &childs) : m_HasResolve(true), m_Operation(operation), m_Childs(childs)
		{
			if (!m_Operation)
				LOG_SCRIPT_COMPILATION(LogType::Error, "Cannot build resolution tree: Out of memory");
		}

		Variable ResolutionTree::ResolveChild(Script *script, CallStack &callstack)
		{
			m_HasResolve = true;
			if (!m_Operation)
			{
				m_HasResolve = false;
				return Variable::NULL_VARIABLE;
			}
			bool operationResult = true;
			if (m_Childs.size() == 0)
			{
				Variable ret = m_Operation->Call(script, callstack, operationResult);
				m_HasResolve = operationResult;
				return ret;
			}
			std::vector<Variable> childResolutions;
			for (auto *child : m_Childs)
			{
				childResolutions.emplace_back(child->ResolveChild(script, callstack));
				if (!child->m_HasResolve)
				{
					m_HasResolve = false;
					return Variable::NULL_VARIABLE;
				}
			}
			Variable ret = m_Operation->Call(script, callstack, childResolutions, operationResult);
			m_HasResolve = operationResult;
			return ret;
		}

		bool ResolutionTree::Resolve(Script *script, CallStack &callstack)
		{
			if (!m_Operation)
				return false;
			bool operationResult = true;
			if (m_Childs.size() == 0)
			{
				m_Operation->Call(script, callstack, operationResult);
				return operationResult;
			}
			std::vector<Variable> childResolutions;
			for (auto *child : m_Childs)
			{
				childResolutions.emplace_back(child->ResolveChild(script, callstack));
				if (!child->m_HasResolve)
					return false;
			}
			m_Operation->Call(script, callstack, childResolutions, operationResult);
			return operationResult;
		}

		ResolutionTree::~ResolutionTree()
		{
			if (m_Operation)
				delete(m_Operation);
			for (auto *child : m_Childs)
				delete(child);
		}
	}
}