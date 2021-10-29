#pragma once

#include <vector>
#include "Ptr.hpp"

namespace Nectere
{
	//Use interpolation search to sort at insert and find element faster
	template <typename t_IDType>
	class UIDSet
	{
	private:
		Ptr<t_IDType> m_Null;
		std::vector<t_IDType *> m_Elements;
		std::vector<Ptr<t_IDType>> m_PtrElements;

	private:
		void RemoveAt(size_t pos)
		{
			delete(m_Elements[pos]);
			m_Elements.erase(m_Elements.begin() + pos);
			m_PtrElements.erase(m_PtrElements.begin() + pos);
		}

		size_t Search(uint16_t id, size_t &left, size_t &right) const
		{
			left = 0;
			right = m_Elements.size() - 1;
			uint16_t idOnLeft = m_Elements[left]->GetID();
			uint16_t idOnRight = m_Elements[right]->GetID();
			while ((idOnLeft != idOnRight) && (id > idOnLeft) && (id < idOnRight))
			{
				size_t pos = left + ((right - left) * ((id - idOnLeft) / (idOnRight - idOnLeft)));
				auto comparator = m_Elements[pos]->GetID() <=> id;
				if (comparator < 0)
				{
					left = pos + 1;
					idOnLeft = m_Elements[left]->GetID();
				}
				else if (comparator > 0)
				{
					right = pos - 1;
					idOnRight = m_Elements[right]->GetID();
				}
				else
					return pos;
			}
			if (id == idOnLeft)
				return left;
			else if (id == idOnRight)
				return right;
			return m_Elements.size() + 1;
		}

	public:
		const std::vector<Ptr<t_IDType>> &GetElements() const {return m_PtrElements;}

		bool Add(t_IDType *element)
		{
			if (element == nullptr)
				return false;
			if (m_Elements.empty())
			{
				m_Elements.emplace_back(element);
				m_PtrElements.emplace_back(Ptr(element));
				return true;
			}
			size_t left, right;
			uint16_t id = element->GetID();
			size_t pos = Search(id, left, right);
			if (pos == m_Elements.size() + 1)
			{
				if (id < m_Elements[left]->GetID())
				{
					m_Elements.insert(m_Elements.begin() + left, element);
					m_PtrElements.insert(m_PtrElements.begin() + left, Ptr(element));
					return true;
				}
				else if (id > m_Elements[right]->GetID())
				{
					m_Elements.insert(m_Elements.begin() + right + 1, element);
					m_PtrElements.insert(m_PtrElements.begin() + right + 1, Ptr(element));
					return true;
				}
			}
			return false;
		}

		bool Find(uint16_t id) const
		{
			if (m_Elements.empty())
				return false;
			size_t left, right;
			size_t pos = Search(id, left, right);
			if (pos != (m_Elements.size() + 1))
				return id == m_Elements[pos]->GetID();
			return false;
		}

		Ptr<t_IDType> &Get(uint16_t id)
		{
			if (m_Elements.empty())
				return m_Null;
			size_t left, right;
			size_t pos = Search(id, left, right);
			if (pos != (m_Elements.size() + 1) && id == m_Elements[pos]->GetID())
				return m_PtrElements[pos];
			return m_Null;
		}

		bool Remove(uint16_t id)
		{
			if (m_Elements.empty())
				return false;
			size_t left, right;
			size_t pos = Search(id, left, right);
			if (pos != (m_Elements.size() + 1) && id == m_Elements[pos]->GetID())
			{
				RemoveAt(pos);
				return true;
			}
			return false;
		}

		void Clear()
		{
			m_PtrElements.clear();
			for (t_IDType *element : m_Elements)
				delete(element);
			m_Elements.clear();
		}

		auto begin() { return m_PtrElements.begin(); }
		auto begin() const { return m_PtrElements.begin(); }
		auto end() { return m_PtrElements.end(); }
		auto end() const { return m_PtrElements.end(); }

		~UIDSet() { Clear(); }
	};
}