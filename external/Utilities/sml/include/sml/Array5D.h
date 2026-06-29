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
class Array5D
{
public:
	// Default constructor
	Array5D();

	// Sized constructor
	Array5D(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4, unsigned dim5);

	// Copy constructor
	Array5D(const Array5D& a);

	// Move constructor
	Array5D(Array5D&& a);

	// Destructor
	virtual ~Array5D();

	// Move assignment operator
	Array5D& operator=(Array5D&& a);

	// Assignment operator
	Array5D& operator=(const Array5D& a);

	// First index operator
	inline T**** operator[](unsigned x) { assert(x < dim1());	return m_pPointers1[x]; }

	// First index operator (read-only) -- syntax, see https://stefansf.de/post/implicit-pointer-to-const-conversion/
	inline T const* const* const* const * operator[](unsigned x) const { assert(x < dim1()); return m_pPointers1[x]; }

	// Number of entries in the first dimension
	inline unsigned dim1() const { return m_dim1; }

	// Number of entries in the second dimension
	inline unsigned dim2() const { return m_dim2; }

	// Number of entries in the third dimension
	inline unsigned dim3() const { return m_dim3; }

	// Number of entries in the fourth dimension
	inline unsigned dim4() const { return m_dim4; }

	// Number of entries in the fifth dimension
	inline unsigned dim5() const { return m_dim5; }

	// Resizes array to specified size (previous contents are lost)
	void resize(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4, unsigned dim5);

	// Returns the root of the array memory
	T* getElements() const { return m_pElements; }

protected:
	// Copies contents of array
	void copy(const Array5D& m);

	// Data members
	unsigned m_dim1;
	unsigned m_dim2;
	unsigned m_dim3;
	unsigned m_dim4;
	unsigned m_dim5;
	T* m_pElements;			// Pointer to memory block
	T** m_pPointers4;		// Pointers to dim4 of elements	
	T*** m_pPointers3;		// Pointers to dim3 of elements
	T**** m_pPointers2;		// Pointers to dim2 of elements
	T***** m_pPointers1;	// Pointers to dim1 of elements
};



template <class T>
Array5D<T>::Array5D()
	: m_dim1(0)
	, m_dim2(0)
	, m_dim3(0)
	, m_dim4(0)
	, m_dim5(0)
	, m_pElements(nullptr)
	, m_pPointers4(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
}



template <class T>
Array5D<T>::Array5D(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4, unsigned dim5)
	: m_dim1(dim1)
	, m_dim2(dim2)
	, m_dim3(dim3)
	, m_dim4(dim4)
	, m_dim5(dim5)
	, m_pElements(nullptr)
	, m_pPointers4(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	resize(dim1, dim2, dim3, dim4, dim5);
}





template <class T>
Array5D<T>::Array5D(const Array5D<T>& a)
	: m_dim1(a.dim1())
	, m_dim2(a.dim2())
	, m_dim3(a.dim3())
	, m_dim4(a.dim4())
	, m_dim5(a.dim5())
	, m_pElements(nullptr)
	, m_pPointers4(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	copy(a);
}



template <class T>
Array5D<T>::Array5D(Array5D<T>&& a)
	: m_dim1(0)
	, m_dim2(0)
	, m_dim3(0)
	, m_dim4(0)
	, m_pElements(nullptr)
	, m_pPointers4(nullptr)
	, m_pPointers3(nullptr)
	, m_pPointers2(nullptr)
	, m_pPointers1(nullptr)
{
	// Move members
	m_dim1 = a.m_dim1;
	m_dim2 = a.m_dim2;
	m_dim3 = a.m_dim3;
	m_dim4 = a.m_dim4;
	m_dim5 = a.m_dim5;
	m_pElements = a.m_pElements;
	m_pPointers1 = a.m_pPointers1;
	m_pPointers2 = a.m_pPointers2;
	m_pPointers3 = a.m_pPointers3;
	m_pPointers4 = a.m_pPointers4;


	// Release the pointer to avoid double destruction
	a.m_pElements = nullptr;
	a.m_pPointers1 = nullptr;
	a.m_pPointers2 = nullptr;
	a.m_pPointers3 = nullptr;
	a.m_pPointers4 = nullptr;
	a.m_dim1 = 0;
	a.m_dim2 = 0;
	a.m_dim3 = 0;
	a.m_dim4 = 0;
	a.m_dim5 = 0;
}



template <class T>
Array5D<T>::~Array5D()
{
	if (m_pElements) {
		delete[] m_pPointers1;
		delete[] m_pPointers2;
		delete[] m_pPointers3;
		delete[] m_pPointers4;
		delete[] m_pElements;
	}
}



template <class T>
Array5D<T>& Array5D<T>::operator=(Array5D<T>&& a)
{
	if (this != &a) {
		// Destroy any existing data
		delete[] m_pPointers1;
		delete[] m_pPointers2;
		delete[] m_pPointers3;
		delete[] m_pPointers4;
		delete[] m_pElements;

		// Move members
		m_dim1 = a.m_dim1;
		m_dim2 = a.m_dim2;
		m_dim3 = a.m_dim3;
		m_dim4 = a.m_dim4;
		m_dim5 = a.m_dim5;
		m_pPointers1 = a.m_pPointers1;
		m_pPointers2 = a.m_pPointers2;
		m_pPointers3 = a.m_pPointers3;
		m_pPointers4 = a.m_pPointers4;
		m_pElements = a.m_pElements;

		// Release the pointer to avoid double destruction
		a.m_pElements = nullptr;
		a.m_pPointers1 = nullptr;
		a.m_pPointers2 = nullptr;
		a.m_pPointers3 = nullptr;
		a.m_pPointers4 = nullptr;
		a.m_dim1 = 0;
		a.m_dim2 = 0;
		a.m_dim3 = 0;
		a.m_dim4 = 0;
		a.m_dim5 = 0;
	}
	return *this;
}



template <class T>
Array5D<T>& Array5D<T>::operator=(const Array5D<T>& a)
{
	if (&a != this) {
		copy(a);
	}
	return *this;
}



template <class T>
void Array5D<T>::resize(unsigned dim1, unsigned dim2, unsigned dim3, unsigned dim4, unsigned dim5)
{
	if (!m_pElements || dim1 != m_dim1 || dim2 != m_dim2 || dim3 != m_dim3 || dim4 != m_dim4 || dim5 != m_dim5) {
		if (m_pElements) {
			delete[] m_pPointers1;
			delete[] m_pPointers2;
			delete[] m_pPointers3;
			delete[] m_pPointers4;
			delete[] m_pElements;
		}
		m_dim1 = dim1;
		m_dim2 = dim2;
		m_dim3 = dim3;
		m_dim4 = dim4;
		m_dim5 = dim5;
		m_pElements = new T[static_cast<size_t>(dim1 * dim2 * dim3 * dim4 * dim5)]; // Contains all the raw data
		m_pPointers4 = new T* [static_cast<size_t>(dim1 * dim2 * dim3 * dim4)];
		m_pPointers3 = new T** [static_cast<size_t>(dim1 * dim2 * dim3)];
		m_pPointers2 = new T*** [static_cast<size_t>(dim1 * dim2)];
		m_pPointers1 = new T**** [static_cast<size_t>(dim1)];
		for (size_t i = 0; i < dim1; i++) {
			for (size_t j = 0; j < dim2; j++) {
				for (size_t k = 0; k < dim3; k++) {
					for (size_t l = 0; l < dim4; l++) {
						m_pPointers4[((i * dim2 + j) * dim3 + k) * dim4 + l] = &m_pElements[(((i * dim2 + j) * dim3 + k) * dim4 + l) * dim5];
					}
					m_pPointers3[(i * dim2 + j) * dim3 + k] = &m_pPointers4[((i * dim2 + j) * dim3 + k) * dim4];
				}
				m_pPointers2[i * dim2 + j] = &m_pPointers3[(i * dim2 + j) * dim3];
			}
			m_pPointers1[i] = &m_pPointers2[i * dim2];
		}
	}
}



template <class T>
void Array5D<T>::copy(const Array5D<T>& m)
{
	resize(m.dim1(), m.dim2(), m.dim3(), m.dim4(), m.dim5());
	memmove(m_pElements, m.m_pElements, dim1() * dim2() * dim3() * dim4() * dim5() * sizeof(T));
}

} // namespace sml
