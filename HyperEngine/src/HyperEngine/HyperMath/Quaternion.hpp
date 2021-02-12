#pragma once

#include <complex>
#include <type_traits>

namespace HyperMath
{
	template <typename T = float>
	class Quaternion
	{
	public:
		static_assert(std::is_same<T, int>() || std::is_same<T, long>() || std::is_same<T, long long>() || std::is_same<T, float>()
			|| std::is_same<T, double>() || std::is_same<T, long double>(), "Invalid scalar type for Quaternion");

	private:
		T m_Scalar;
		Vector3<T> m_Axis;

	public:
		Quaternion(T scalar, T pitch, T yaw, T roll)
			: m_Scalar{ scalar }, m_Axis{ pitch, yaw, roll }
		{
		}

		template<typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion(T1 scalar, T1 pitch, T1 yaw, T1 roll)
			: m_Scalar{ scalar }, m_Axis{ pitch, yaw, roll }
		{
		}

		Quaternion(T scalar, const Vector3<T>& axis)
			: m_Scalar{ scalar }, m_Axis{ axis }
		{
		}

		template<typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion(T1 scalar, const Vector3<T1>& axis)
			: m_Scalar{ scalar }, m_Axis{ axis }
		{
		}

		Quaternion(const std::complex<T>& x, const std::complex<T>& y = std::complex<T>(0, 0))
			: m_Scalar{ x.real() }, m_Axis{ x.imag(), y.real(), y.imag() }
		{
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion(const std::complex<T1>& x, const std::complex<T1>& y = std::complex<T1>(0, 0))
			: m_Scalar{ x.real() }, m_Axis{ x.imag(), y.real(), y.imag() }
		{
		}

		Quaternion(const Quaternion<T>& other)
			: m_Scalar{ other.m_Scalar }, m_Axis{ other.m_Axis }
		{
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion(const Quaternion<T1>& other)
			: m_Scalar{ other.m_Scalar }, m_Axis{ other.m_Axis }
		{
		}

		Quaternion& operator=(const Quaternion<T> other)
		{
			m_Scalar = other.m_Scalar;
			m_Axis = other.m_Axis;
			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion& operator=(const Quaternion<T1> other)
		{
			m_Scalar = other.m_Scalar;
			m_Axis = other.m_Axis;
			return *this;
		}

		inline T GetScalar() const
		{
			return m_Scalar;
		}

		inline const Vector3<T>& GetAxis() const
		{
			return m_Axis;
		}

		inline std::complex<T> GetComplex1() const
		{
			return { m_Scalar, m_Axis.x };
		}

		inline std::complex<T> GetComplex2() const
		{
			return { m_Axis.y, m_Axis.z };
		}

		inline T GetReal() const
		{
			return m_Scalar;
		}

		inline Quaternion GetUnreal() const
		{
			return{ 0, m_Axis };
		}

		T NormSquared() const
		{
			return m_Scalar * m_Scalar + m_Axis.x * m_Axis.x + m_Axis.y * m_Axis.y + m_Axis.z * m_Axis.z;
		}

		T UnrealNormSquared() const
		{
			return m_Axis.x * m_Axis.x + m_Axis.y * m_Axis.y + m_Axis.z * m_Axis.z;
		}

		T Abs() const
		{
			return std::sqrt(NormSquared());
		}

		bool IsZero(T epsilon = 0) const
		{
			return IsScalarZero(m_Scalar, epsilon) && IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsZero(T1 epsilon = 0) const
		{
			return IsScalarZero(m_Scalar, epsilon) && IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		bool IsNonZero(T epsilon = 0) const
		{
			return !IsZero(epsilon);
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsNonZero(T1 epsilon = 0) const
		{
			return !IsZero(epsilon);
		}

		bool IsNan() const
		{
			return std::isnan(m_Scalar) || std::isnan(m_Axis.x) || std::isnan(m_Axis.y) || std::isnan(m_Axis.z);
		}

		bool IsInfinite() const
		{
			return std::isinf(m_Scalar) || std::isinf(m_Axis.x) || std::isinf(m_Axis.y) || std::isinf(m_Axis.z);
		}

		bool IsFinite() const
		{
			return std::isfinite(m_Scalar) && std::isfinite(m_Axis.x) && std::isfinite(m_Axis.y) && std::isfinite(m_Axis.z);
		}

		bool IsUnit(T epsilon = 0) const
		{
			return IsScalarZero(NormSquared() - T(1), epsilon);
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsUnit(T1 epsilon = 0) const
		{
			return IsScalarZero(NormSquared() - T(1), epsilon);
		}

		bool IsReal(T epsilon = 0) const
		{
			return IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsReal(T1 epsilon = 0) const
		{
			return IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		bool IsComplex(T epsilon = 0) const
		{
			return IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsComplex(T1 epsilon = 0) const
		{
			return IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon);
		}

		bool IsUnreal(T epsilon = 0) const
		{
			return IsScalarZero(m_Scalar, epsilon) && !(IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon));
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		bool IsUnreal(T1 epsilon = 0) const
		{
			return IsScalarZero(m_Scalar, epsilon) && !(IsScalarZero(m_Axis.x, epsilon) && IsScalarZero(m_Axis.y, epsilon) && IsScalarZero(m_Axis.z, epsilon));
		}

		Quaternion operator+() const
		{
			return *this;
		}

		Quaternion operator-() const
		{
			return { -m_Scalar, -m_Axis.x, -m_Axis.y, -m_Axis.z };
		}

		Quaternion operator+=(T value)
		{
			m_Scalar += value;
			return *this;
		}

		Quaternion operator-=(T value)
		{
			m_Scalar -= value;
			return *this;
		}

		Quaternion operator*=(T value)
		{
			m_Scalar *= value;
			m_Axis.x *= value;
			m_Axis.y *= value;
			m_Axis.z *= value;
			return *this;
		}

		Quaternion operator/=(T value)
		{
			m_Scalar /= value;
			m_Axis.x /= value;
			m_Axis.y /= value;
			m_Axis.z /= value;
			return *this;
		}

		Quaternion operator+=(const std::complex<T>& value)
		{
			m_Scalar += value.real();
			m_Axis.x += value.imag();
			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator+=(const std::complex<T>& value)
		{
			m_Scalar += value.real();
			m_Axis.x += value.imag();
			return *this;
		}

		Quaternion operator-=(const std::complex<T>& value)
		{
			m_Scalar -= value.real();
			m_Axis.x -= value.imag();
			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator-=(const std::complex<T>& value)
		{
			m_Scalar -= value.real();
			m_Axis.x -= value.imag();
			return *this;
		}

		Quaternion operator*=(const std::complex<T>& value)
		{
			T at = m_Scalar * value.real() - m_Axis.x * value.imag();
			T bt = m_Scalar * value.imag() + m_Axis.x * value.real();
			T ct = m_Axis.y * value.real() + m_Axis.z * value.imag();
			T dt = -m_Axis.y * value.imag() + m_Axis.z * value.real();

			m_Scalar = at;
			m_Axis.x = bt;
			m_Axis.y = ct;
			m_Axis.z = dt;

			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator*=(const std::complex<T>& value)
		{
			T at = m_Scalar * value.real() - m_Axis.x * value.imag();
			T bt = m_Scalar * value.imag() + m_Axis.x * value.real();
			T ct = m_Axis.y * value.real() + m_Axis.z * value.imag();
			T dt = -m_Axis.y * value.imag() + m_Axis.z * value.real();

			m_Scalar = at;
			m_Axis.x = bt;
			m_Axis.y = ct;
			m_Axis.z = dt;

			return *this;
		}

		Quaternion operator/=(const std::complex<T>& value)
		{
			T n2 = y.real() * value.real() + y.imag() * value.imag();

			T at = m_Scalar * value.real() + m_Axis.x * value.imag();
			T bt = -m_Scalar * value.imag() + m_Axis.x * value.real();
			T ct = m_Axis.y * value.real() - m_Axis.z * value.imag();
			T dt = m_Axis.y * value.imag() + m_Axis.z * value.real();

			m_Scalar = at / n2;
			m_Axis.x = bt / n2;
			m_Axis.y = ct / n2;
			m_Axis.z = dt / n2;

			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator/=(const std::complex<T>& value)
		{
			T n2 = y.GetReal() * value.real() + y.imag() * value.imag();

			T at = m_Scalar * value.real() + m_Axis.x * value.imag();
			T bt = -m_Scalar * value.imag() + m_Axis.x * value.real();
			T ct = m_Axis.y * value.real() - m_Axis.z * value.imag();
			T dt = m_Axis.y * value.imag() + m_Axis.z * value.real();

			m_Scalar = at / n2;
			m_Axis.x = bt / n2;
			m_Axis.y = ct / n2;
			m_Axis.z = dt / n2;

			return *this;
		}

		Quaternion operator+=(const Quaternion<T>& value)
		{
			m_Scalar += value.m_Scalar;
			m_Axis.x += value.m_Axis.x;
			m_Axis.y += value.m_Axis.y;
			m_Axis.z += value.m_Axis.z;
			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator+=(const Quaternion<T>& value)
		{
			m_Scalar += value.m_Scalar;
			m_Axis.x += value.m_Axis.x;
			m_Axis.y += value.m_Axis.y;
			m_Axis.z += value.m_Axis.z;
			return *this;
		}

		Quaternion operator-=(const Quaternion<T>& value)
		{
			m_Scalar -= value.m_Scalar;
			m_Axis.x -= value.m_Axis.x;
			m_Axis.y -= value.m_Axis.y;
			m_Axis.z -= value.m_Axis.z;
			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator-=(const Quaternion<T>& value)
		{
			m_Scalar -= value.m_Scalar;
			m_Axis.x -= value.m_Axis.x;
			m_Axis.y -= value.m_Axis.y;
			m_Axis.z -= value.m_Axis.z;
			return *this;
		}

		Quaternion operator*=(const Quaternion<T>& value)
		{
			T at = m_Scalar * value.m_Scalar - m_Axis.x * value.m_Axis.x - m_Axis.y * value.m_Axis.y - m_Axis.z * value.m_Axis.z;
			T bt = m_Scalar * value.m_Axis.x + m_Axis.x * value.m_Scalar + m_Axis.y * value.m_Axis.z - m_Axis.z * value.m_Axis.y;
			T ct = m_Scalar * value.m_Axis.y - m_Axis.x * value.m_Axis.z + m_Axis.y * value.m_Scalar + m_Axis.z * value.m_Axis.x;
			T dt = m_Scalar * value.m_Axis.z + m_Axis.x * value.m_Axis.y - m_Axis.y * value.m_Axis.x + m_Axis.z * value.m_Scalar;

			m_Scalar = at;
			m_Axis.x = bt;
			m_Axis.y = ct;
			m_Axis.z = dt;

			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator*=(const Quaternion<T>& value)
		{
			T at = m_Scalar * value.m_Scalar - m_Axis.x * value.m_Axis.x - m_Axis.y * value.m_Axis.y - m_Axis.z * value.m_Axis.z;
			T bt = m_Scalar * value.m_Axis.x + m_Axis.x * value.m_Scalar + m_Axis.y * value.m_Axis.z - m_Axis.z * value.m_Axis.y;
			T ct = m_Scalar * value.m_Axis.y - m_Axis.x * value.m_Axis.z + m_Axis.y * value.m_Scalar + m_Axis.z * value.m_Axis.x;
			T dt = m_Scalar * value.m_Axis.z + m_Axis.x * value.m_Axis.y - m_Axis.y * value.m_Axis.x + m_Axis.z * value.m_Scalar;

			m_Scalar = at;
			m_Axis.x = bt;
			m_Axis.y = ct;
			m_Axis.z = dt;

			return *this;
		}

		Quaternion operator/=(const Quaternion<T>& value)
		{
			T n2 = value.NormSquared();

			T at = m_Scalar * value.m_Scalar + m_Axis.x * value.m_Axis.x + m_Axis.y * value.m_Axis.y + m_Axis.z * value.m_Axis.z;
			T bt = -m_Scalar * value.m_Axis.x + m_Axis.x * value.m_Scalar - m_Axis.y * value.m_Axis.z + m_Axis.z * value.m_Axis.y;
			T ct = -m_Scalar * value.m_Axis.y + m_Axis.x * value.m_Axis.z + m_Axis.y * value.m_Scalar - m_Axis.z * value.m_Axis.x;
			T dt = -m_Scalar * value.m_Axis.z - m_Axis.x * value.m_Axis.y + m_Axis.y * value.m_Axis.x + m_Axis.z * value.m_Scalar;

			m_Scalar = at / n2;
			m_Axis.x = bt / n2;
			m_Axis.y = ct / n2;
			m_Axis.z = dt / n2;

			return *this;
		}

		template <typename T1, typename std::enable_if<std::is_convertible<T1, T>::value>::type* = nullptr>
		Quaternion operator/=(const Quaternion<T>& value)
		{
			T n2 = value.NormSquared();

			T at = m_Scalar * value.m_Scalar + m_Axis.x * value.m_Axis.x + m_Axis.y * value.m_Axis.y + m_Axis.z * value.m_Axis.z;
			T bt = -m_Scalar * value.m_Axis.x + m_Axis.x * value.m_Scalar - m_Axis.y * value.m_Axis.z + m_Axis.z * value.m_Axis.y;
			T ct = -m_Scalar * value.m_Axis.y + m_Axis.x * value.m_Axis.z + m_Axis.y * value.m_Scalar - m_Axis.z * value.m_Axis.x;
			T dt = -m_Scalar * value.m_Axis.z - m_Axis.x * value.m_Axis.y + m_Axis.y * value.m_Axis.x + m_Axis.z * value.m_Scalar;

			m_Scalar = at / n2;
			m_Axis.x = bt / n2;
			m_Axis.y = ct / n2;
			m_Axis.z = dt / n2;

			return *this;
		}

	private:
		template<typename T1, typename T2>
		bool IsScalarZero(T1 x, T2 epsilon = 0)
		{
			typedef typename std::common_type<T1, T2>::type T;
			T xx = static_cast<T>(x);
			T ee = static_cast<T>(epsilon);
			return std::abs(xx) <= ee;
		}

		template<typename T1, typename T2, typename T3>
		bool IsNearlyEqual(T1 x, T2 y, T3 epsilon = 0)
		{
			typedef typename std::common_type<T1, T2, T3>::type T;
			T xx = static_cast<T>(x);
			T yy = static_cast<T>(y);
			T ee = static_cast<T>(epsilon);
			if (xx == 0)
				return IsScalarZero(yy, ee);
			else if (yy == 0)
				return IsScalarZero(xx, ee);
			else
				return IsScalarZero((xx - yy) / std::min(xx, yy), ee);
		}
	};
}
