// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/
#pragma once

#include <cassert>

namespace sml
{

// Three dimensional array class with efficient memory use
template <class T>
class Array4D
{
public:
	// Default constructor
	Array4D();

	// Sized constructor
	Array4D(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4);

	// Copy constructor
	Array4D(const Array4D& a);

	// Move constructor
	Array4D(Array4D&& a);

	// Destructor
	virtual ~Array4D();

	// Move assignment operator
	Array4D& operator=(Array4D&& a);

	// Assignment operator
	Array4D& operator=(const Array4D& a);

	// First index operator
	inline T*** operator[](unsigned x) { assert(x < dim1());	return m_pPointers1[x]; }

	// First index operator (read-only) -- syntax, see https://stefansf.de/post/implicit-pointer-to-const-conversion/
	inline T const* const* const* operator[](unsigned x) const { assert(x < dim1()); return m_pPointers1[x]; }

	// Number of entries in the first dimension
	inline unsigned dim1() const { return m_dim1; }

	// Number of entries in the second dimension
	inline unsigned dim2() const { return m_dim2; }

	// Number of entries in the third dimension
	inline unsigned dim3() const { return m_dim3; }

	// Number of entries in the fourth dimensino
	inline unsigned dim4() const { return m_dim4; }

	// Resizes array to specified size (previous contents are lost)
	void resize(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4);

	// Returns the root of the array memory
	T* getElements() const { return m_pElements; }

protected:
	// Copies contents of array
	void copy(const Array4D& m);

	// Data members
	unsigned m_dim1;
	unsigned m_dim2;
	unsigned m_dim3;
	unsigned m_dim4;
	T* m_pElements;			// Pointer to memory block
	T** m_pPointers3;		// Pointers to dim3 of elements
	T*** m_pPointers2;		// Pointers to dim2 of elements
	T**** m_pPointers1;		// Pointers to dim1 of elements
};



template <class T>
Array4D<T>::Array4D()
	: m_dim1(0)
	, m_dim2(0)
	, m_dim3(0)
	, m_dim4(0)
	, m_pElements(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
}



template <class T>
Array4D<T>::Array4D(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4)
	: m_dim1(dim1)
	, m_dim2(dim2)
	, m_dim3(dim3)
	, m_dim4(dim4)
	, m_pElements(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	resize(dim1, dim2, dim3, dim4);
}





template <class T>
Array4D<T>::Array4D(const Array4D<T>& a)
	: m_dim1(a.dim1())
	, m_dim2(a.dim2())
	, m_dim3(a.dim3())
	, m_dim4(a.dim4())
	, m_pElements(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	copy(a);
}



template <class T>
Array4D<T>::Array4D(Array4D<T>&& a)
	: m_dim1(0)
	, m_dim2(0)
	, m_dim3(0)
	, m_dim4(0)
	, m_pElements(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	// Move members
	m_dim1 = a.m_dim1;
	m_dim2 = a.m_dim2;
	m_dim3 = a.m_dim3;
	m_dim4 = a.m_dim4;
	m_pElements = a.m_pElements;
	m_pPointers1 = a.m_pPointers1;
	m_pPointers2 = a.m_pPointers2;
	m_pPointers3 = a.m_pPointers3;

	// Release the pointer to avoid double destruction
	a.m_pElements = nullptr;
	a.m_pPointers1 = nullptr;
	a.m_pPointers2 = nullptr;
	a.m_pPointers3 = nullptr;
	a.m_dim1 = 0;
	a.m_dim2 = 0;
	a.m_dim3 = 0;
	a.m_dim4 = 0;
}



template <class T>
Array4D<T>::~Array4D()
{
	if (m_pElements) {
		delete[] m_pPointers1;
		delete[] m_pPointers2;
		delete[] m_pPointers3;
		delete[] m_pElements;
	}
}



template <class T>
Array4D<T>& Array4D<T>::operator=(Array4D<T>&& a)
{
	if (this != &a) {
		// Destroy any existing data
		delete[] m_pPointers1;
		delete[] m_pPointers2;
		delete[] m_pPointers3;
		delete[] m_pElements;

		// Move members
		m_dim1 = a.m_dim1;
		m_dim2 = a.m_dim2;
		m_dim3 = a.m_dim3;
		m_dim4 = a.m_dim4;
		m_pPointers1 = a.m_pPointers1;
		m_pPointers2 = a.m_pPointers2;
		m_pPointers3 = a.m_pPointers3;
		m_pElements = a.m_pElements;

		// Release the pointer to avoid double destruction
		a.m_pElements = nullptr;
		a.m_pPointers1 = nullptr;
		a.m_pPointers2 = nullptr;
		a.m_pPointers3 = nullptr;
		a.m_dim1 = 0;
		a.m_dim2 = 0;
		a.m_dim3 = 0;
		a.m_dim4 = 0;
	}
	return *this;
}



template <class T>
Array4D<T>& Array4D<T>::operator=(const Array4D<T>& a)
{
	if (&a != this) {
		copy(a);
	}
	return *this;
}



template <class T>
void Array4D<T>::resize(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4)
{
	if (!m_pElements || dim1 != m_dim1 || dim2 != m_dim2 || dim3 != m_dim3 || dim4 != m_dim4) {
		if (m_pElements) {
			delete[] m_pPointers1;
			delete[] m_pPointers2;
			delete[] m_pPointers3;
			delete[] m_pElements;
		}
		m_dim1 = dim1;
		m_dim2 = dim2;
		m_dim3 = dim3;
		m_dim4 = dim4;
		m_pElements = new T[static_cast<size_t>(dim1 * dim2 * dim3 * dim4)]; // Contains all the raw data
		m_pPointers3 = new T* [static_cast<size_t>(dim1 * dim2 * dim3)];
		m_pPointers2 = new T** [static_cast<size_t>(dim1 * dim2)]; // Contains the pointers to the
		m_pPointers1 = new T*** [static_cast<size_t>(dim1)]; // Contains the pointers to the j-pointers
		for (size_t i = 0; i < dim1; i++) {
			for (size_t j = 0; j < dim2; j++) {
				for (size_t k = 0; k < dim3; k++) {
					m_pPointers3[(i * dim2 + j) * dim3 + k] = &m_pElements[(((i * dim2 + j) * dim3 + k) * dim4)];
				}
				m_pPointers2[i * dim2 + j] = &m_pPointers3[(i * dim2 + j) * dim3];
			}
			m_pPointers1[i] = &m_pPointers2[i * dim2];
		}
	}
}



template <class T>
void Array4D<T>::copy(const Array4D<T>& m)
{
	resize(m.dim1(), m.dim2(), m.dim3(), m.dim4());
	memmove(m_pElements, m.m_pElements, dim1() * dim2() * dim3() * dim4() * sizeof(T));
}

} // namespace sml
