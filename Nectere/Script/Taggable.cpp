#include "Script/Taggable.hpp"

#include <sstream>

namespace Nectere
{
	namespace Script
	{
		bool Taggable::Match(const Tag &tag, EMatchParam matchParam) const
		{
			if (m_Tags.find(tag.m_Name) == m_Tags.end())
				return false;
			switch (matchParam)
			{
			case Script::Taggable::EMatchParam::MatchNameOnly:
				return true;
			case Script::Taggable::EMatchParam::MatchContainParam:
			{
				const Tag &tagToCompare = m_Tags.at(tag.m_Name);
				const auto &compareEnd = tagToCompare.m_Params.end();
				for (const std::string &paramToSearch : tag.m_Params)
				{
					if (tagToCompare.m_Params.find(paramToSearch) == compareEnd)
						return false;
				}
				return true;
			}
			case Script::Taggable::EMatchParam::MatchFull:
			{
				const Tag &tagToCompare = m_Tags.at(tag.m_Name);
				if (tagToCompare.m_Params.size() != tag.m_Params.size())
					return false;
				const auto &compareEnd = tagToCompare.m_Params.end();
				for (const std::string &paramToSearch : tag.m_Params)
				{
					if (tagToCompare.m_Params.find(paramToSearch) == compareEnd)
						return false;
				}
				return true;
			}
			default:
				return false;
			}
		}

		bool Taggable::AddTag(const Tag &tag)
		{
			if (m_Tags.find(tag.m_Name) != m_Tags.end())
			{
				Tag &tagToEdit = m_Tags.at(tag.m_Name);
				tagToEdit.m_Params.insert(tag.m_Params.begin(), tag.m_Params.end());
				return true;
			}
			m_Tags[tag.m_Name] = tag;
			++m_NbTag;
			return true;
		}

		bool Taggable::AddTags(const std::vector<std::string> &tagsNames)
		{
			for (const std::string &tagName : tagsNames)
			{
				if (!AddTag({ tagName }))
					return false;
			}
			return true;
		}

		bool Taggable::AddTags(const std::vector<Tag> &tags)
		{
			for (const Tag &tag : tags)
			{
				if (!AddTag(tag))
					return false;
			}
			return true;
		}

		bool Taggable::AddTags(const Taggable *taggable)
		{
			if (taggable)
			{
				for (const auto &pair : taggable->m_Tags)
				{
					const Tag &tag = pair.second;
					if (!AddTag(tag))
						return false;
				}
				return true;
			}
			return false;
		}

		bool Taggable::RemoveTag(const std::string &tagName)
		{
			auto it = m_Tags.find(tagName);
			if (it == m_Tags.end())
				return false;
			m_Tags.erase(it);
			--m_NbTag;
			return true;
		}

		std::string Taggable::TagToString() const
		{
			std::stringstream stream;
			bool first = true;
			stream << '[';
			for (const auto &pair : m_Tags)
			{
				const Tag &tag = pair.second;
				if (!first)
					stream << ", ";
				first = false;
				stream << tag.m_Name;
				std::unordered_set<std::string> params = tag.m_Params;
				if (params.size() != 0)
				{
					bool firstParam = true;
					stream << '(';
					for (const std::string &paramValue : params)
					{
						if (!firstParam)
							stream << ", ";
						firstParam = false;
						stream << paramValue;
					}
					stream << ')';
				}
			}
			stream << ']';
			return stream.str();
		}

		std::vector<Script::Tag> Taggable::GetTagList() const
		{
			std::vector<Tag> tagList;
			for (const auto &pair : m_Tags)
				tagList.emplace_back(pair.second);
			return tagList;
		}
	}
}