#include "Script/Variable.hpp"

#include "Logger.hpp"

static std::string EMPTY = "";

namespace Nectere
{
	namespace Script
	{
		Variable Variable::NULL_VARIABLE(nullptr, false, true);

		Variable::Variable() : Variable(nullptr, false, false) {}
		Variable::Variable(Unit *unit, bool autoDelete) : Variable(unit, autoDelete, false) {}
		Variable::Variable(Unit *unit, bool autoDelete, bool raw) : m_Unit(unit), m_Delete(autoDelete), m_Raw(raw) {}

		Variable::Variable(Unit *unit) : m_Unit(unit), m_Delete(true), m_Raw(false)
		{
			if (!m_Unit)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create new variable: Out of memory");
				m_Delete = false;
			}
		}

		Variable::Variable(const VariableType &type, const CurrentEnvironment &helper) : Variable(new Unit(type, "", helper)) {}
		Variable::Variable(const std::string &name, bool value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, char value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, unsigned char value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, short value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, unsigned short value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, int value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, unsigned int value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, long value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, unsigned long value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, long long value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, unsigned long long value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, float value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, double value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, long double value) : Variable(new Unit(name, value)) {}
		Variable::Variable(const std::string &name, const std::string &value) : Variable(new Unit(name, value)) {}
		Variable::Variable(DeclaredObjectDefinition *definition, const CurrentEnvironment &helper) : Variable(new Unit(definition, "", helper)) {}

		Variable::Variable(Variable &other) : m_Unit(other.m_Unit), m_Delete(other.m_Delete), m_Raw(false)
		{
			if (!other.m_Raw)
			{
				other.m_Unit = nullptr;
				other.m_Delete = false;
			}
			else
				m_Delete = false;
		}

		Variable &Variable::operator=(Variable &other)
		{
			m_Unit = other.m_Unit;
			m_Delete = other.m_Delete;
			if (!other.m_Raw)
			{
				other.m_Unit = nullptr;
				other.m_Delete = false;
			}
			else
				m_Delete = false;
			return *this;
		}

		Variable::Variable(Variable &&other) noexcept : m_Unit(other.m_Unit), m_Delete(other.m_Delete), m_Raw(false)
		{
			if (!other.m_Raw)
			{
				other.m_Unit = nullptr;
				other.m_Delete = false;
			}
			else
				m_Delete = false;
		}

		Variable::Variable(const std::string &name, std::vector<Variable> &elements) : m_Delete(true), m_Raw(false)
		{
			std::vector<Unit *> unitElements;
			for (Variable &element : elements)
			{
				if (element.m_Unit)
				{
					unitElements.emplace_back(element.m_Unit);
					element.m_Unit = nullptr;
				}
			}
			m_Unit = new Unit(name, unitElements);
			if (!m_Unit)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create new variable: Out of memory");
				m_Delete = false;
			}
		}

		Variable::Variable(const std::string &name, std::vector<Variable> &elements, const VariableType &arrayType) : m_Delete(true), m_Raw(false)
		{
			std::vector<Unit *> unitElements;
			for (Variable &element : elements)
			{
				if (element.m_Unit)
				{
					unitElements.emplace_back(element.m_Unit);
					element.m_Unit = nullptr;
				}
			}
			m_Unit = new Unit(name, unitElements, arrayType);
			if (!m_Unit)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot create new variable: Out of memory");
				m_Delete = false;
			}
		}

		Variable &Variable::operator=(Variable &&other) noexcept
		{
			m_Unit = other.m_Unit;
			m_Delete = other.m_Delete;
			if (!other.m_Raw)
			{
				other.m_Unit = nullptr;
				other.m_Delete = false;
			}
			else
				m_Delete = false;
			return *this;
		}

		bool Variable::Have(const std::string &element)
		{
			if (!m_Unit)
				return true;
			return m_Unit->Have(element);
		}

		bool Variable::Have(const std::string &element) const
		{
			if (!m_Unit)
				return true;
			return m_Unit->c_Have(element);
		}

		bool Variable::IsNull() const
		{
			if (!m_Unit)
				return true;
			return m_Unit->IsNull();
		}

		bool Variable::IsConst() const
		{
			if (!m_Unit)
				return true;
			return m_Unit->IsConst();
		}

		void Variable::SetConstness(bool constness)
		{
			if (m_Unit)
				m_Unit->SetConstness(constness);
		}

		const std::string &Variable::GetName() const
		{
			if (!m_Unit)
				return EMPTY;
			return m_Unit->GetName();
		}

		std::string Variable::ToString() const
		{
			if (!m_Unit)
				return "null";
			return m_Unit->ToString();
		}

		std::string Variable::ToRawString() const
		{
			if (!m_Unit)
				return "null";
			return m_Unit->ToRawString();
		}

		Variable::~Variable()
		{
			if (m_Delete && m_Unit != nullptr)
				delete(m_Unit);
		}

		bool Variable::Set(const std::string &value)
		{
			if (m_Raw)
				return false;
			if (m_Unit)
				return m_Unit->Set(value);
			return false;
		}

		bool Variable::Set(const std::string &key, const std::string &value)
		{
			if (m_Raw)
				return false;
			if (m_Unit)
				return m_Unit->Set(key, value);
			return false;
		}

		bool Variable::Set(const Variable &value)
		{
			if (m_Raw || !m_Unit || !value.m_Unit)
				return false;
			return m_Unit->Set(value.m_Unit);
		}

		bool Variable::Set(const std::string &key, const Variable &value)
		{
			if (m_Raw || !m_Unit || !value.m_Unit)
				return false;
			return m_Unit->Set(key, value.m_Unit);
		}

		Variable Variable::Get(const std::string &name)
		{
			if (!m_Unit)
				return Variable::NULL_VARIABLE;
			return Variable(m_Unit->Get(name), false);
		}

		Variable Variable::Get(const std::string &name) const
		{
			if (!m_Unit)
				return Variable::NULL_VARIABLE;
			return Variable(m_Unit->c_Get(name), false);
		}

		Variable Variable::GetMutable() const
		{
			if (!m_Unit)
				return Variable::NULL_VARIABLE;
			return Variable(m_Unit, false);
		}

		Variable Variable::GetCopy() const
		{
			if (!m_Unit)
				return Variable::NULL_VARIABLE;
			Unit *copy = new Unit(m_Unit);
			if (!copy)
			{
				LOG_SCRIPT_RUNTIME(LogType::Error, "Cannot copy variable: Out of memory");
				return Variable::NULL_VARIABLE;
			}
			return Variable(copy, true);
		}

		bool Variable::HaveTags() const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTags();
		}

		bool Variable::HaveTagWithName(const Tag &tag) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTagWithName(tag);
		}

		bool Variable::HaveTagWithName(const std::string &tagName) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTagWithName(tagName);
		}

		bool Variable::HaveTagWithParam(const Tag &tag) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTagWithParam(tag);
		}

		bool Variable::HaveTagWithParam(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTagWithParam(tagName, tagParams);
		}

		bool Variable::HaveTag(const Tag &tag) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTag(tag);
		}

		bool Variable::HaveTag(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const
		{
			if (!m_Unit)
				return false;
			return m_Unit->HaveTag(tagName, tagParams);
		}

		bool Variable::AddTag(const std::string &tagName)
		{
			if (!m_Unit)
				return false;
			return m_Unit->AddTag(tagName);
		}

		bool Variable::AddTag(const Tag &tag)
		{
			if (!m_Unit)
				return false;
			return m_Unit->AddTag(tag);
		}

		bool Variable::AddTags(const std::vector<std::string> &tagsNames)
		{
			if (!m_Unit)
				return false;
			return m_Unit->AddTags(tagsNames);
		}

		bool Variable::AddTags(const std::vector<Tag> &tags)
		{
			if (!m_Unit)
				return false;
			return m_Unit->AddTags(tags);
		}

		bool Variable::AddTags(const Taggable *taggable)
		{
			if (!m_Unit)
				return false;
			return m_Unit->AddTags(taggable);
		}

		bool Variable::RemoveTag(const Tag &tag)
		{
			if (!m_Unit)
				return false;
			return m_Unit->RemoveTag(tag);
		}

		bool Variable::RemoveTag(const std::string &tagName)
		{
			if (!m_Unit)
				return false;
			return m_Unit->RemoveTag(tagName);
		}

		std::string Variable::TagToString() const
		{
			if (!m_Unit)
				return "";
			return m_Unit->TagToString();
		}

		std::vector<Tag> Variable::GetTagList() const
		{
			if (!m_Unit)
				return std::vector<Tag>();
			return m_Unit->GetTagList();
		}

		std::ostream &operator<<(std::ostream &os, const Variable &var)
		{
			os << var.ToString();
			return os;
		}
	}
}