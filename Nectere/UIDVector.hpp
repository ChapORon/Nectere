#pragma once

#include <memory>
#include <vector>

namespace Nectere
{
	template <typename t_IDType>
	class UIDVector
	{
	private:
		std::vector<std::shared_ptr<t_IDType>> m_Elements;

	public:
		const std::vector<std::shared_ptr<t_IDType>> &GetElements() const { return m_Elements; }
		bool Add(const std::shared_ptr<t_IDType> &element)
		{
			if (element == nullptr)
				return false;
			uint16_t id = element->GetID();
			if (m_Elements.empty())
			{
				m_Elements.emplace_back(element);
				return true;
			}
			size_t left = 0;
			size_t right = m_Elements.size() - 1;
			uint16_t idOnLeft = m_Elements[left]->GetID();
			uint16_t idOnRight = m_Elements[right]->GetID();
			while ((idOnLeft != idOnRight) && (id > idOnLeft) && (id < idOnRight))
			{
				size_t pos = left + ((right - left) * ((id - idOnLeft) / (idOnRight - idOnLeft)));
				uint16_t idOnPos = m_Elements[pos]->GetID();
				if (idOnPos < id)
				{
					left = pos + 1;
					idOnLeft = m_Elements[left]->GetID();
				}
				else if (idOnPos > id)
				{
					right = pos - 1;
					idOnRight = m_Elements[right]->GetID();
				}
				else
					return false;
			}
			if (id < idOnLeft)
			{
				m_Elements.insert(m_Elements.begin() + left, element);
				return true;
			}
			else if (id > idOnRight)
			{
				m_Elements.insert(m_Elements.begin() + (right + 1), element);
				return true;
			}
			return false;
		}

		bool Find(uint16_t id)
		{
			if (m_Elements.empty())
				return false;
			size_t left = 0;
			size_t right = m_Elements.size() - 1;
			uint16_t idOnLeft = m_Elements[left]->GetID();
			uint16_t idOnRight = m_Elements[right]->GetID();
			while ((idOnLeft != idOnRight) && (id > idOnLeft) && (id < idOnRight))
			{
				size_t pos = left + ((right - left) * ((id - idOnLeft) / (idOnRight - idOnLeft)));
				uint16_t idOnPos = m_Elements[pos]->GetID();
				if (idOnPos < id)
				{
					left = pos + 1;
					idOnLeft = m_Elements[left]->GetID();
				}
				else if (idOnPos > id)
				{
					right = pos - 1;
					idOnRight = m_Elements[right]->GetID();
				}
				else
					return true;
			}
			return (id == idOnLeft || id == idOnRight);
		}

		std::shared_ptr<t_IDType> Get(uint16_t id)
		{
			if (m_Elements.empty())
				return nullptr;
			size_t left = 0;
			size_t right = m_Elements.size() - 1;
			uint16_t idOnLeft = m_Elements[left]->GetID();
			uint16_t idOnRight = m_Elements[right]->GetID();
			while ((idOnLeft != idOnRight) && (id > idOnLeft) && (id < idOnRight))
			{
				size_t pos = left + ((right - left) * ((id - idOnLeft) / (idOnRight - idOnLeft)));
				uint16_t idOnPos = m_Elements[pos]->GetID();
				if (idOnPos < id)
				{
					left = pos + 1;
					idOnLeft = m_Elements[left]->GetID();
				}
				else if (idOnPos > id)
				{
					right = pos - 1;
					idOnRight = m_Elements[right]->GetID();
				}
				else
					return m_Elements[pos];
			}
			if (id == idOnLeft)
				return m_Elements[left];
			else if (id == idOnRight)
				return m_Elements[right];
			return nullptr;
		}

		void Remove(uint16_t id)
		{
			if (m_Elements.empty())
				return;
			size_t left = 0;
			size_t right = m_Elements.size() - 1;
			uint16_t idOnLeft = m_Elements[left]->GetID();
			uint16_t idOnRight = m_Elements[right]->GetID();
			while ((idOnLeft != idOnRight) && (id > idOnLeft) && (id < idOnRight))
			{
				size_t pos = left + ((right - left) * ((id - idOnLeft) / (idOnRight - idOnLeft)));
				uint16_t idOnPos = m_Elements[pos]->GetID();
				if (idOnPos < id)
				{
					left = pos + 1;
					idOnLeft = m_Elements[left]->GetID();
				}
				else if (idOnPos > id)
				{
					right = pos - 1;
					idOnRight = m_Elements[right]->GetID();
				}
				else
				{
					m_Elements.erase(m_Elements.begin() + pos);
					return;
				}
			}
			if (id == idOnLeft)
				m_Elements.erase(m_Elements.begin() + left);
			else if (id == idOnRight)
				m_Elements.erase(m_Elements.begin() + right);
		}

		void Clear()
		{
			m_Elements.clear();
		}
	};
}