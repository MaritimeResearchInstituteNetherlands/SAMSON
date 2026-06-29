// Copyright (c) 2026, MARIN, Maritime Research Institute Netherlands.
//
// Licensed under the European Union Public Licence (EUPL), Version 1.2.
// You may not use this work except in compliance with the Licence.
// You may obtain a copy of the Licence at:
//
// https://eupl.eu/1.2/en/

#pragma once

#include <array>
#include <iostream>

namespace sml {

template<typename Type, size_t N>
class Vector 
{
public:
	Vector();

	template<typename... Values>
	Vector(Values... values);

	Vector(const Vector<Type, N>& other) = default;

	Vector(Vector<Type, N>&& other);

	Vector<Type, N>& operator=(const Vector<Type, N>& copy) = default;

	Vector<Type, N>& operator=(Vector<Type, N>&& other);

	double norm() const;

	void normalise();

	inline Type operator[](int i) const { return m_data[i]; }

	inline Type& operator[](int i) { return m_data[i]; }

	inline Vector<Type, N>& operator+=(const Vector<Type, N>& other) { for (size_t i = 0; i < N; i++) { m_data[i] += other[i]; } return *this; }

	inline Vector<Type, N>& operator-=(const Vector<Type, N>& other) { for (size_t i = 0; i < N; i++) { m_data[i] -= other[i]; } return *this; }

	inline Vector<Type, N>& operator/=(const double val) { for (size_t i = 0; i < N; i++) { m_data[i] /= val; } return *this; }

private:
	std::array<Type, N> m_data;
};



template<typename Type, size_t N>
Vector<Type, N>::Vector()
	: m_data()
{
}



template<typename Type, size_t N> template<typename... Values>
Vector<Type, N>::Vector(Values... values)
	: m_data{{ values... }}
{
}



template<typename Type, size_t N>
Vector<Type, N>::Vector(Vector<Type, N>&& other)
{
	std::swap(m_data, other.m_data);
	
}



template<typename Type, size_t N>
Vector<Type, N>& Vector<Type, N>::operator=(Vector<Type, N>&& other)
{
	std::swap(m_data, other.m_data);
	return *this;
}



template<typename Type, size_t N>
double Vector<Type, N>::norm() const
{
	return std::sqrt(*this * *this);
}



template <typename Type, size_t N>
void Vector<Type, N>::normalise()
{
	double scale = 1.0 / norm();
	for (size_t i = 0; i < N; i++) {
		m_data[i] *= scale;
	}
}



template <typename Type, size_t N>
Vector<Type, N> operator+(const Vector<Type, N>& left, const Vector<Type, N>& right)
{
	Vector<Type, N> result;
	for (size_t i = 0; i < N; i++) {
		result[i] = left[i] + right[i];
	}
	return result;
}



template <typename Type, size_t N>
Vector<Type, N> operator-(const Vector<Type, N>& left, const Vector<Type, N>& right)
{
	Vector<Type, N> result;
	for (size_t i = 0; i < N; i++) {
		result[i] = left[i] - right[i];
	}
	return result;
}



template <typename Type, size_t N>
Vector<Type, N> operator-(Vector<Type, N> const& v)
{
	Vector<Type, N> result;
	for (size_t i = 0; i < N; i++) {
		result[i] = -v[i];
	}
	return result;
}



template <typename Type, size_t N>
Vector<Type, N> operator/(const Vector<Type, N>& vector, const double val)
{
	Vector<Type, N> result;
	double scale = 1.0 / val;
	for (size_t i = 0; i < N; i++) {
		result[i] = vector[i] * scale;
	}
	return result;
}



template <typename Type, size_t N>
Vector<Type, N> operator*(const Vector<Type, N>& vector, const double val)
{
	Vector<Type, N> result;
	for (size_t i = 0; i < N; i++) {
		result[i] = vector[i] * val;
	}
	return result;
}



template <typename Type, size_t N>
bool operator==(const Vector<Type, N>& left, const Vector<Type, N>& right)
{
	// Option A
	//for (size_t i = 0;  i < N; i++) {
	//	if ((left[i] - right[i]) != 0) {
	//		return false;
	//	}
	//}
	//return true

	// Option B
	size_t i = 0;
	while (i < N && (left[i] - right[i]) == 0) {
		i++;
	}
	return i == N ? true : false;
}



template <typename Type, size_t N>
std::ostream& operator<<(std::ostream& s, const Vector<Type, N>& vector)
{
	s << "{";
	for (size_t i = 0; i < N; ++i) {
		s << vector[i];
		if (i < N - 1) {
			s << ", ";
		}
	}
	s << "}";
	return s;
}


template <size_t N, typename Type>
double operator*(const Vector<Type, N>& left, const Vector<Type, N>& right)
{
	double dot = 0.0;
	for (size_t i = 0; i < N; i++) {
		dot += left[i] * right[i];
	}
	return dot;
}

using Vector3d = Vector<double, 3>;
using Vector2d = Vector<double, 2>;
} // namespace sml


