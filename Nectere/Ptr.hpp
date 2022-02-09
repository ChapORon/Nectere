#pragma once

#include "nectere_export.h"

namespace Nectere
{
	template <typename t_PtrType>
	class NECTERE_EXPORT Ptr
	{
	private:
		t_PtrType *m_Ptr;

	public:
		inline Ptr(): m_Ptr(nullptr) {}
		inline Ptr(t_PtrType *ptr): m_Ptr(ptr) {}
		inline Ptr(Ptr<t_PtrType> &other): m_Ptr(other.m_Ptr) {}
		inline Ptr(Ptr<t_PtrType> &&other): m_Ptr(other.m_Ptr) {}
		inline Ptr(const Ptr<t_PtrType> &other): m_Ptr(other.m_Ptr) {}

		Ptr<t_PtrType> &operator=(Ptr<t_PtrType> &other) { m_Ptr = other.m_Ptr; return *this; }
		Ptr<t_PtrType> &operator=(Ptr<t_PtrType> &&other) { m_Ptr = other.m_Ptr; return *this; }
		Ptr<t_PtrType> &operator=(const Ptr<t_PtrType> &other) { m_Ptr = other.m_Ptr; return *this; }
		inline const t_PtrType *operator->() const { return m_Ptr; }
		inline t_PtrType *operator->() { return m_Ptr; }
		inline operator bool() const { return m_Ptr != nullptr; }
		template <typename t_CastPtrType>
		operator Ptr<t_CastPtrType>() const { return Ptr<t_CastPtrType>(dynamic_cast<t_CastPtrType *>(m_Ptr)); }
	};
}