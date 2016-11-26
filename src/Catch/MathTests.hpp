#pragma once

namespace Phoenix::Tests::MathTests
{
	/*template<typename Result>
	void Assert(Result actual, Result expected)
	{
		if (actual == expected)
		{
			return;
		}
		else
		{
			Fail(actual, expected);
		}
	}

	template<typename Result>
	void Assert(Result actual, Result expected, std::function<bool(Result, Result)> evalFunc)
	{
		auto success = evalFunc(actual, expected);

		if (success)
		{
			return;
		}
		else
		{
			Fail(actual, expected);
		}
	}

	template<typename Result>
	void Fail(Result actual, Result expected)
	{
		Problem: No common to string for all types
		std::cout << "Assertion failed: Expected: " << expected << " Got: " << actual;
		assert(false);
	}*/

	void RunMathTests();
	void LegacyTests();
	void Vec3Tests();
	void Matrix4Tests();
	void PlaneTests();
}