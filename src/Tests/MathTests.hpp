#pragma once

namespace Phoenix { namespace Tests
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

	void runMathTests();
	void legacyTests();
	void vec3Tests();
	void matrix4Tests();
	void planeTests();
}
}