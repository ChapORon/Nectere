#include "Dp/Result.hpp"

namespace Nectere
{
	namespace Dp
	{
		Result::Result(const Format &format) :
			m_Empty(true),
			m_CurrentDepth(format.startDepth),
			m_Indent(static_cast<size_t>(format.startDepth) *format.indentation, ' '),
			m_Format(format) {}

		void Result::Append(char c)
		{
			if (c != '\0')
			{
				m_Empty = false;
				m_Buffer << c;
			}
		}

		void Result::Append(const std::string &value)
		{
			if (!value.empty())
			{
				m_Empty = false;
				m_Buffer << value;
			}
		}

		void Result::Indent()
		{
			Append(m_Indent);
		}

		void Result::LineBreak()
		{
			if (m_Format.breakLine)
			{
				m_Empty = false;
				m_Buffer << '\n';
			}
		}

		void Result::ForceLineBreak()
		{
			m_Empty = false;
			m_Buffer << '\n';
		}

		void Result::IncreaseDepth()
		{
			++m_CurrentDepth;
			m_Indent = std::string(static_cast<size_t>(m_CurrentDepth) * m_Format.indentation, ' ');
		}

		void Result::DecreaseDepth()
		{
			if (m_CurrentDepth > 0)
			{
				--m_CurrentDepth;
				m_Indent = std::string(static_cast<size_t>(m_CurrentDepth) * m_Format.indentation, ' ');
			}
		}

		void Result::Set(const std::string &value)
		{
			m_Buffer.clear();
			m_Empty = value.empty();
			m_Buffer << value;
		}

		unsigned int Result::GetDepth() const
		{
			return m_CurrentDepth;
		}

		bool Result::IsLineBreakAllowed() const
		{
			return m_Format.breakLine;
		}

		bool Result::IsEmpty() const
		{
			return m_Empty;
		}

		std::string Result::Str() const
		{
			return m_Buffer.str();
		}
	}
}