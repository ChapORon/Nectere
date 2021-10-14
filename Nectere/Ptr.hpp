#pragma once

namespace Nectere
{
	template <typename t_PtrType>
	class Ptr
	{
	private:
		t_PtrType *m_Ptr;

	public:
		Ptr(t_PtrType *ptr) : m_Ptr(ptr) {}
		Ptr(Ptr<t_PtrType> &other) : m_Ptr(other.m_Ptr) {}
		Ptr(Ptr<t_PtrType> &&other) : m_Ptr(other.m_Ptr) {}
		Ptr(const Ptr<t_PtrType> &other) : m_Ptr(other.m_Ptr) {}

		Ptr<t_PtrType> &operator=(Ptr<t_PtrType> &other) { m_Ptr = other.m_Ptr; return *this; }
		Ptr<t_PtrType> &operator=(Ptr<t_PtrType> &&other) { m_Ptr = other.m_Ptr; return *this; }
		Ptr<t_PtrType> &operator=(const Ptr<t_PtrType> &other) { m_Ptr = other.m_Ptr; return *this; }
		const t_PtrType *operator->() const { return m_Ptr; }
		t_PtrType *operator->() { return m_Ptr; }
		operator bool() { return m_Ptr != nullptr; }
	};
}