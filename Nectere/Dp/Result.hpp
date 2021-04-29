#pragma once

#include <sstream>
#include <string>
#include "Dp/Format.hpp"

namespace Nectere
{
	namespace Dp
	{
		class Result
		{
		private:
			bool m_Empty;
			unsigned int m_CurrentDepth;
			std::string m_Indent;
			std::stringstream m_Buffer;
			Format m_Format;

		public:
			explicit Result(const Format &);
			void Append(char);
			void Append(const std::string &);
			void Indent();
			void LineBreak();
			void ForceLineBreak();
			void IncreaseDepth();
			void DecreaseDepth();
			void Set(const std::string &);
			unsigned int GetDepth() const;
			bool IsLineBreakAllowed() const;
			bool IsEmpty() const;
			std::string Str() const;
		};
	}
}