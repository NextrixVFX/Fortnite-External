#pragma once
#include <imgui/imgui.h>

namespace primitives
{
	inline const int screen_width = GetSystemMetrics(0);
	inline const int screen_height = GetSystemMetrics(1);

	inline const int screen_width_center = screen_width / 2;
	inline const int screen_height_center = screen_height / 2;

	class fvector {
	public:
		double x{ };
		double y{ };
		double z{ };

		constexpr fvector() noexcept = default;
		constexpr fvector(double _x, double _y, double _z) noexcept : x(_x), y(_y), z(_z) {}
		~fvector() noexcept = default;

		[[nodiscard]] inline double dot(const fvector& v) const noexcept {
			return x * v.x + y * v.y + z * v.z;
		}

		[[nodiscard]] inline double distance(const fvector& v) const noexcept {
			return std::sqrt(std::pow(v.x - x, 2.0) + std::pow(v.y - y, 2.0) + std::pow(v.z - z, 2.0));
		}

		[[nodiscard]] inline double vector_scalar(const fvector& v) const noexcept {
			return x * v.x + y * v.y + z * v.z;
		}

		[[nodiscard]] inline double length() const noexcept {
			return std::sqrt(x * x + y * y + z * z);
		}

		[[nodiscard]] fvector operator + (const fvector& v) const noexcept {
			return fvector(x + v.x, y + v.y, z + v.z);
		}

		[[nodiscard]] fvector operator - (const fvector& v) const noexcept {
			return fvector(x - v.x, y - v.y, z - v.z);
		}

		[[nodiscard]] fvector operator * (double number) const noexcept {
			return fvector(x * number, y * number, z * number);
		}

		[[nodiscard]] fvector operator / (double number) const noexcept {
			return fvector(x / number, y / number, z / number);
		}

		fvector& operator *= (double number) noexcept {
			x *= number;
			y *= number;
			z *= number;
			return *this;
		}

		inline void add_scale(const fvector& v, float scale) noexcept {
			x += v.x * scale;
			y += v.y * scale;
			z += v.z * scale;
		}

		[[nodiscard]] inline bool is_zero() const noexcept {
			return x == 0.0 && y == 0.0 && z == 0.0;
		}

		[[nodiscard]] fvector normalize() const noexcept {
			const double len = length();
			if (len == 0.0)
				return fvector(0.0, 0.0, 0.0);
			return fvector(x / len, y / len, z / len);
		}
	};

	class fvector2d {
	public:
		double x{ };
		double y{ };

		constexpr fvector2d() noexcept = default;
		constexpr fvector2d(double _x, double _y) noexcept : x(_x), y(_y) {}

		[[nodiscard]] fvector2d operator + (const fvector2d& v) const noexcept {
			return fvector2d(x + v.x, y + v.y);
		}

		[[nodiscard]] fvector2d operator - (const fvector2d& v) const noexcept {
			return fvector2d(x - v.x, y - v.y);
		}

		[[nodiscard]] fvector2d operator * (double scalar) const noexcept {
			return fvector2d(x * scalar, y * scalar);
		}

		[[nodiscard]] inline bool is_zero() const noexcept {
			return x == 0.0 && y == 0.0;
		}
	};

	class frotator {
	public:
		double pitch{ };
		double yaw{ };
		double roll{ };

		constexpr frotator() noexcept = default;
		constexpr frotator(double _pitch, double _yaw, double _roll) noexcept : pitch(_pitch), yaw(_yaw), roll(_roll) {}

		[[nodiscard]] frotator operator + (const frotator& other) const noexcept {
			return frotator(pitch + other.pitch, yaw + other.yaw, roll + other.roll);
		}

		[[nodiscard]] frotator operator - (const frotator& other) const noexcept {
			return frotator(pitch - other.pitch, yaw - other.yaw, roll - other.roll);
		}

		[[nodiscard]] frotator operator * (double offset) const noexcept {
			return frotator(pitch * offset, yaw * offset, roll * offset);
		}

		[[nodiscard]] frotator operator / (double offset) const noexcept {
			return frotator(pitch / offset, yaw / offset, roll / offset);
		}

		frotator& operator = (const frotator& other) noexcept = default;
		frotator& operator = (double val) noexcept { pitch = val; yaw = val; roll = val; return *this; }

		frotator& operator *= (double val) noexcept { pitch *= val; yaw *= val; roll *= val; return *this; }
		frotator& operator /= (double val) noexcept { pitch /= val; yaw /= val; roll /= val; return *this; }

		frotator& operator += (const frotator& other) noexcept { pitch += other.pitch; yaw += other.yaw; roll += other.roll; return *this; }
		frotator& operator -= (const frotator& other) noexcept { pitch -= other.pitch; yaw -= other.yaw; roll -= other.roll; return *this; }
		frotator& operator /= (const frotator& other) noexcept { pitch /= other.pitch; yaw /= other.yaw; roll /= other.roll; return *this; }

		[[nodiscard]] explicit operator bool() const noexcept {
			return pitch != 0.0 || yaw != 0.0 || roll != 0.0;
		}

		friend bool operator == (const frotator& a, const frotator& b) noexcept {
			return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll;
		}

		friend bool operator != (const frotator& a, const frotator& b) noexcept {
			return !(a == b);
		}

		[[nodiscard]] frotator get() const noexcept {
			return frotator(pitch, yaw, roll);
		}

		inline void set(double _pitch, double _yaw, double _roll) noexcept {
			pitch = _pitch;
			yaw = _yaw;
			roll = _roll;
		}

		[[nodiscard]] frotator normalize() const noexcept {
			frotator result = get();

			if (std::isfinite(result.pitch) && std::isfinite(result.yaw) && std::isfinite(result.roll)) {
				result.pitch = std::clamp(result.pitch, -89.0, 89.0);
				result.yaw = std::clamp(result.yaw, -180.0, 180.0);
				result.roll = 0.0;
			}

			return result;
		}

		[[nodiscard]] double length() const noexcept {
			return std::sqrt(pitch * pitch + yaw * yaw + roll * roll);
		}

		[[nodiscard]] double dot(const frotator& v) const noexcept {
			return pitch * v.pitch + yaw * v.yaw + roll * v.roll;
		}

		[[nodiscard]] double distance(const frotator& v) const noexcept {
			return std::sqrt(std::pow(v.pitch - pitch, 2.0) + std::pow(v.yaw - yaw, 2.0) + std::pow(v.roll - roll, 2.0));
		}
	};

	struct fplane : fvector {
		double w{ 0.0 };
	};

}
