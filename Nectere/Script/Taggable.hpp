#pragma once

#include <unordered_map>
#include "Script/Tag.hpp"

namespace Nectere
{
	namespace Script
	{
		class Taggable
		{
		private:
			enum class EMatchParam : int
			{
				MatchNameOnly,
				MatchContainParam,
				MatchFull,
				Count,
				Invalid
			};
		private:
			size_t m_NbTag{ 0 };
			std::unordered_map<std::string, Tag> m_Tags;

		private:
			bool Match(const Tag &, EMatchParam) const;

		public:
			inline bool HaveTags() const { return m_NbTag != 0; }
			inline bool HaveTagWithName(const Tag &tag) const { return Match(tag, EMatchParam::MatchNameOnly); }
			inline bool HaveTagWithName(const std::string &tagName) const { return Match(Tag{ tagName }, EMatchParam::MatchNameOnly); }
			inline bool HaveTagWithParam(const Tag &tag) const { return Match(tag, EMatchParam::MatchContainParam); }
			inline bool HaveTagWithParam(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const { return Match(Tag{ tagName, tagParams }, EMatchParam::MatchContainParam); }
			inline bool HaveTag(const Tag &tag) const { return Match(tag, EMatchParam::MatchFull); }
			inline bool HaveTag(const std::string &tagName, const std::unordered_set<std::string> &tagParams) const { return Match(Tag{ tagName, tagParams }, EMatchParam::MatchFull); }
			inline const Tag &GetTag(const Tag &tag) const { return m_Tags.at(tag.m_Name); }
			inline const Tag &GetTag(const std::string &tagName) const { return m_Tags.at(tagName); }
			inline bool AddTag(const std::string &tagName) { return AddTag(Tag{ tagName }); }
			bool AddTag(const Tag &);
			bool AddTags(const std::vector<std::string> &);
			bool AddTags(const std::vector<Tag> &);
			bool AddTags(const Taggable *);
			inline bool RemoveTag(const Tag &tag) { return RemoveTag(tag.m_Name); }
			bool RemoveTag(const std::string &);
			std::string TagToString() const;
			std::vector<Tag> GetTagList() const;
		};
	}
}