/*****************************************************************************************
Project      : 3D Game Engine
Author       : Sam Tian
Contributors :
******************************************************************************************/
#pragma once

#include "VectorOperator.h"
#include "Utility.h"
#include <algorithm>
#include <cassert>

namespace sml
{
  /***** BASE VECTOR TEMPLATE *****/

  template <typename T, size_t N, VecType VT, EvalType ET>
  struct Vector
  {
    T data[N];

    //  variadic constructor - this is really versatile
    template <typename... V> constexpr Vector(V... vs) : data{ vs... } {}

    //  constexpr array constructor
    template <size_t... Is> constexpr Vector(const T * a, std::index_sequence<Is...>) : data{ a[Is]... } {}
    constexpr Vector(const T * pv) : Vector<T, N, VT>(pv, std::make_index_sequence<N>{}){}

    //  value constructor - all members will be initialized to a single value
    explicit Vector(T v = 0) { for (auto&& i : data) { i = v; } }

    //  construct from differently sized vector - this delegates to array constructor
    template <size_t S> Vector(const Vector<T, S>& vec) : Vector(vec.data, std::make_index_sequence<min(N, S)>{} ) {}

    //  construct from vector-value pair
    Vector(const Vector<T, N - 1> vec, T v)
    {
      std::copy(vec.data, vec.data + N - 1, data);
      data[N - 1] = v;
    }

    //  accessors
    constexpr T operator[](size_t n) const { assert(N > n); return data[n]; }

    constexpr T x() const { static_assert(N > 0, "Error: cannot access x()"); return data[0]; }
    constexpr T y() const { static_assert(N > 1, "Error: cannot access y()"); return data[1]; }
    constexpr T z() const { static_assert(N > 2, "Error: cannot access z()"); return data[2]; }
    constexpr T r() const { static_assert(N > 0, "Error: cannot access r()"); return data[0]; }
    constexpr T g() const { static_assert(N > 1, "Error: cannot access g()"); return data[1]; }
    constexpr T b() const { static_assert(N > 2, "Error: cannot access b()"); return data[2]; }
    constexpr T a() const { static_assert(N > 3, "Error: cannot access a()"); return data[3]; }

    //  mutators
    T& operator[](size_t n) { assert(N > n); return data[n]; }

    T& x() { static_assert(N > 0, "Error: cannot access x()"); return data[0]; }
    T& y() { static_assert(N > 1, "Error: cannot access y()"); return data[1]; }
    T& z() { static_assert(N > 2, "Error: cannot access z()"); return data[2]; }
    T& r() { static_assert(N > 0, "Error: cannot access r()"); return data[0]; }
    T& g() { static_assert(N > 1, "Error: cannot access g()"); return data[1]; }
    T& b() { static_assert(N > 2, "Error: cannot access b()"); return data[2]; }
    T& a() { static_assert(N > 3, "Error: cannot access a()"); return data[3]; }

    // mutable operators
    Vector& operator+=(T rhs)
    {
			/*
      for (auto&& i : data)
        i += rhs;
      return *this;
			*/

			VectorOperator<T, N, VT, ET>::Add(*this, rhs);
			return *this;
    }

    template <size_t S>
    Vector& operator+=(const Vector<T, S>& rhs)
    {
      for (size_t i = 0; i < min(N, S); ++i)   
        data[i] += rhs[i];
      return *this;
    }

    Vector& operator-=(T rhs)
    {
      for (auto&& i : data)
        i -= rhs;
      return *this;
    }

    template <size_t S>
    Vector& operator-=(const Vector<T, S>& rhs)
    {
      for (size_t i = 0; i < min(N, S); ++i)
        data[i] -= rhs[i];
      return *this;
    }

    Vector& operator*=(T rhs)
    {
      for (auto&& i : data)
        i *= rhs;
      return *this;
    }

    Vector& operator/=(T rhs)
    {
      if (s != 0)
      {
        for (auto&& i : res.data)
          i /= s;
      }
      return *this;
    }

    //  mutable vector operations
    Vector& cross(const Vector& rhs)
    {
      static_assert(N > 2, "Error: cannot take cross product of vector with less than 3 dimensions");
      Vector<T, 3> res(y() * rhs.z() - z() * rhs.y(), z() * rhs.x() - x() * rhs.z(), x() * rhs.y() - y() * rhs.x());
      x() = res.x();
      y() = res.y();
      z() = res.z();
      return *this;
    }

    Vector& normalize()
    {
      return (*this) /= distance();
    }

    //  utility
    T distance() const
    {
      return (T)q_sqrt((*this) * (*this));
    }
  };

  /***** OPERATORS *****/

  //  operator +
  template <template<typename, size_t> typename V, typename T, size_t N1, size_t N2>
  Vector<T, N1> operator+(const V<T, N1>& lhs, const V<T, N2>& rhs)
  {
      return V<T, N1>(lhs) += rhs;
  }

  template <typename T, size_t N>
  Vector<T, N> operator+(const Vector<T, N>& lhs, T rhs)
  {
    return Vector<T, N>(lhs) += rhs;
  }

  template <typename T, size_t N>
  Vector<T, N> operator+(T lhs, const Vector<T, N>& rhs)
  {
    return Vector<T, N>(rhs) += lhs;
  }

  //  operator -
  template <typename T, size_t N1, size_t N2>
  Vector<T, N1> operator-(const Vector<T, N1>& lhs, const Vector<T, N2>& rhs)
  {
    return Vector<T, N1>(lhs) -= rhs;
  }

  template <typename T, size_t N>
  Vector<T, N> operator-(const Vector<T, N>& lhs, T rhs)
  {
    return Vector<T, N>(lhs) -= rhs;
  }

  //  operator *
  template <typename T, size_t N>
  Vector<T, N> operator*(const Vector<T, N>& lhs, T rhs)
  {
    return Vector<T, N>(lhs) *= rhs;
  }

  template <typename T, size_t N>
  Vector<T, N> operator*(T lhs, const Vector<T, N>& rhs)
  {
    return Vector<T, N>(rhs) *= lhs;
  }

  //  operator * (dot product)
  template <typename T, size_t N>
  T operator*(const Vector<T, N>& lhs, const Vector<T, N>& rhs)
  {
    T res = 0.0;
    for (size_t i = 0; i < N; ++i)
      res += lhs[i] * rhs[i];
    return res;
  }

  //  operator /
  template <typename T, size_t N>
  Vector<T, N> operator/(const Vector<T, N>& lhs, T rhs)
  {
    return Vector<T, N>(lhs) /= rhs;
  }

  //  vector operations
  template <typename T, size_t N>
  Vector<T, N> cross(const Vector<T, N>& lhs, const Vector<T, N>& rhs)
  {
    return Vector<T, N>(lhs).cross(rhs);
  }

  template <typename T, size_t N>
  Vector<T, N> normalize(const Vector<T, N>& vec)
  {
    return Vector<T, N>(vec).normalize();
  }
};