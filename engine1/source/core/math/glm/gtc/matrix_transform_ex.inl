/// @ref gtc_matrix_transform
/// @file glm/gtc/matrix_transform.inl

#include "../geometric.hpp"
#include "../trigonometric.hpp"
#include "../matrix.hpp"

namespace glm
{
	
	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> ortho
	(
		T left, T right,
		T bottom, T top,
		T zNear, T zFar,
		bool _oglNdc
	)
	{
#		if GLM_COORDINATE_SYSTEM == GLM_LEFT_HANDED
			return orthoLH(left, right, bottom, top, zNear, zFar, _oglNdc);
#		else
			return orthoRH(left, right, bottom, top, zNear, zFar, _oglNdc);
#		endif
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> orthoLH
	(
		T left, T right,
		T bottom, T top,
		T zNear, T zFar,
		bool _oglNdc
	)
	{
		tmat4x4<T, defaultp> Result(1);
		Result[0][0] = static_cast<T>(2) / (right - left);
		Result[1][1] = static_cast<T>(2) / (top - bottom);
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);

		if (!_oglNdc)
		{
			Result[2][2] = static_cast<T>(1) / (zFar - zNear);
			Result[3][2] = - zNear / (zFar - zNear);
		}
		else
		{
			Result[2][2] = static_cast<T>(2) / (zFar - zNear);
			Result[3][2] = - (zFar + zNear) / (zFar - zNear);
		}

		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> orthoRH
	(
		T left, T right,
		T bottom, T top,
		T zNear, T zFar,
		bool _oglNdc
	)
	{
		tmat4x4<T, defaultp> Result(1);
		Result[0][0] = static_cast<T>(2) / (right - left);
		Result[1][1] = static_cast<T>(2) / (top - bottom);
		Result[3][0] = - (right + left) / (right - left);
		Result[3][1] = - (top + bottom) / (top - bottom);

		if (!_oglNdc)
		{
			Result[2][2] = - static_cast<T>(1) / (zFar - zNear);
			Result[3][2] = - zNear / (zFar - zNear);
		}
		else
		{
			Result[2][2] = - static_cast<T>(2) / (zFar - zNear);
			Result[3][2] = - (zFar + zNear) / (zFar - zNear);
		}

		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> frustum
	(
		T left, T right,
		T bottom, T top,
		T nearVal, T farVal,
		bool _oglNdc
	)
	{
#		if GLM_COORDINATE_SYSTEM == GLM_LEFT_HANDED
			return frustumLH(left, right, bottom, top, nearVal, farVal, _oglNdc);
#		else
			return frustumRH(left, right, bottom, top, nearVal, farVal, _oglNdc);
#		endif
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> frustumLH
	(
		T left, T right,
		T bottom, T top,
		T nearVal, T farVal,
		bool _oglNdc
	)
	{
		tmat4x4<T, defaultp> Result(0);
		Result[0][0] = (static_cast<T>(2) * nearVal) / (right - left);
		Result[1][1] = (static_cast<T>(2) * nearVal) / (top - bottom);
		Result[2][0] = (right + left) / (right - left);
		Result[2][1] = (top + bottom) / (top - bottom);
		Result[2][3] = static_cast<T>(1);

		if (!_oglNdc)
		{
			Result[2][2] = farVal / (farVal - nearVal);
			Result[3][2] = -(farVal * nearVal) / (farVal - nearVal);
		}
		else
		{
			Result[2][2] = (farVal + nearVal) / (farVal - nearVal);
			Result[3][2] = - (static_cast<T>(2) * farVal * nearVal) / (farVal - nearVal);
		}


		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> frustumRH
	(
		T left, T right,
		T bottom, T top,
		T nearVal, T farVal,
		bool _oglNdc
	)
	{
		tmat4x4<T, defaultp> Result(0);
		Result[0][0] = (static_cast<T>(2) * nearVal) / (right - left);
		Result[1][1] = (static_cast<T>(2) * nearVal) / (top - bottom);
		Result[2][0] = (right + left) / (right - left);
		Result[2][1] = (top + bottom) / (top - bottom);
		Result[2][3] = static_cast<T>(-1);

		if (!_oglNdc)
		{
			Result[2][2] = farVal / (nearVal - farVal);
			Result[3][2] = -(farVal * nearVal) / (farVal - nearVal);
		}
		else
		{
			Result[2][2] = - (farVal + nearVal) / (farVal - nearVal);
			Result[3][2] = - (static_cast<T>(2) * farVal * nearVal) / (farVal - nearVal);
		}

		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspective(T fovy, T aspect, T zNear, T zFar, bool _oglNdc)
	{
#		if GLM_COORDINATE_SYSTEM == GLM_LEFT_HANDED
			return perspectiveLH(fovy, aspect, zNear, zFar, _oglNdc);
#		else
			return perspectiveRH(fovy, aspect, zNear, zFar, _oglNdc);
#		endif
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveRH(T fovy, T aspect, T zNear, T zFar, bool _oglNdc)
	{
		assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovy = tan(fovy / static_cast<T>(2));

		tmat4x4<T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][3] = - static_cast<T>(1);

		if (!_oglNdc)
		{
			Result[2][2] = zFar / (zNear - zFar);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		}
		else
		{
			Result[2][2] = - (zFar + zNear) / (zFar - zNear);
			Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		}

		return Result;
	}
	
	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveLH(T fovy, T aspect, T zNear, T zFar, bool _oglNdc)
	{
		assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovy = tan(fovy / static_cast<T>(2));
		
		tmat4x4<T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][3] = static_cast<T>(1);

		if (!_oglNdc)
		{
			Result[2][2] = zFar / (zFar - zNear);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		}
		else
		{
			Result[2][2] = (zFar + zNear) / (zFar - zNear);
			Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		}

		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveFov(T fov, T width, T height, T zNear, T zFar, bool _oglNdc)
	{
#		if GLM_COORDINATE_SYSTEM == GLM_LEFT_HANDED
			return perspectiveFovLH(fov, width, height, zNear, zFar, _oglNdc);
#		else
			return perspectiveFovRH(fov, width, height, zNear, zFar, _oglNdc);
#		endif
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveFovRH(T fov, T width, T height, T zNear, T zFar, bool _oglNdc)
	{
		assert(width > static_cast<T>(0));
		assert(height > static_cast<T>(0));
		assert(fov > static_cast<T>(0));
	
		T const rad = fov;
		T const h = glm::cos(static_cast<T>(0.5) * rad) / glm::sin(static_cast<T>(0.5) * rad);
		T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

		tmat4x4<T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = w;
		Result[1][1] = h;
		Result[2][3] = - static_cast<T>(1);

		if (!_oglNdc)
		{
			Result[2][2] = zFar / (zNear - zFar);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		}
		else
		{
			Result[2][2] = - (zFar + zNear) / (zFar - zNear);
			Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		}

		return Result;
	}

	template <typename T>
	GLM_FUNC_QUALIFIER tmat4x4<T, defaultp> perspectiveFovLH(T fov, T width, T height, T zNear, T zFar, bool _oglNdc)
	{
		assert(width > static_cast<T>(0));
		assert(height > static_cast<T>(0));
		assert(fov > static_cast<T>(0));
	
		T const rad = fov;
		T const h = glm::cos(static_cast<T>(0.5) * rad) / glm::sin(static_cast<T>(0.5) * rad);
		T const w = h * height / width; ///todo max(width , Height) / min(width , Height)?

		tmat4x4<T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = w;
		Result[1][1] = h;
		Result[2][3] = static_cast<T>(1);

		if (!_oglNdc)
		{
			Result[2][2] = zFar / (zFar - zNear);
			Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		}
		else
		{
			Result[2][2] = (zFar + zNear) / (zFar - zNear);
			Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
		}

		return Result;
	}

	template <typename T, typename U, precision P>
	GLM_FUNC_QUALIFIER tvec3<T, P> project
	(
		tvec3<T, P> const & obj,
		tmat4x4<T, P> const & model,
		tmat4x4<T, P> const & proj,
		tvec4<U, P> const & viewport,
		bool _oglNdc
	)
	{
		tvec4<T, P> tmp = tvec4<T, P>(obj, static_cast<T>(1));
		tmp = model * tmp;
		tmp = proj * tmp;

		tmp /= tmp.w;
		if (!_oglNdc)
		{
			tmp.x = tmp.x * static_cast<T>(0.5) + static_cast<T>(0.5);
			tmp.y = tmp.y * static_cast<T>(0.5) + static_cast<T>(0.5);
		}
		else
		{
			tmp = tmp * static_cast<T>(0.5) + static_cast<T>(0.5);
		}

		tmp[0] = tmp[0] * T(viewport[2]) + T(viewport[0]);
		tmp[1] = tmp[1] * T(viewport[3]) + T(viewport[1]);

		return tvec3<T, P>(tmp);
	}

	template <typename T, typename U, precision P>
	GLM_FUNC_QUALIFIER tvec3<T, P> unProject
	(
		tvec3<T, P> const & win,
		tmat4x4<T, P> const & model,
		tmat4x4<T, P> const & proj,
		tvec4<U, P> const & viewport,
		bool _oglNdc
	)
	{
		tmat4x4<T, P> Inverse = inverse(proj * model);

		tvec4<T, P> tmp = tvec4<T, P>(win, T(1));
		tmp.x = (tmp.x - T(viewport[0])) / T(viewport[2]);
		tmp.y = (tmp.y - T(viewport[1])) / T(viewport[3]);
		if (!_oglNdc)
		{
			tmp.x = tmp.x * static_cast<T>(2) - static_cast<T>(1);
			tmp.y = tmp.y * static_cast<T>(2) - static_cast<T>(1);
		}
		else
		{
			tmp = tmp * static_cast<T>(2) - static_cast<T>(1);
		}

		tvec4<T, P> obj = Inverse * tmp;
		obj /= obj.w;

		return tvec3<T, P>(obj);
	}

}//namespace glm
