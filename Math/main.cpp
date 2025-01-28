

#include <iostream>
#include <vector>

#include "MyAssert.h"
#include "GameMath.h"


int main()
{

	{
		Vector3 v{ 1.0f, 1.0f, 1.0f };
		Vector3 v2;
		Vector3::Normalize(&v2, v);

		float sqSum = Vector3::SquareSum(v2);
	}
	
	{
		std::vector<Vector2> currentDirs;
		currentDirs.push_back({ 1.0f, 0.0f });

		int res;


		std::vector<Vector2> targets;
		std::vector<std::vector<int>> ans(currentDirs.size());

		targets.push_back({ 1.0f, 0.f }); // xAxisPos
		ans[0].push_back(0);
		targets.push_back({ -1.0f, 0.f }); // xAxisNeg
		ans[0].push_back(1);
		targets.push_back({ 0.f, 1.0f }); // yAxisPos
		ans[0].push_back(1);
		targets.push_back({ 0.f, -1.0f }); // yAxisNeg
		ans[0].push_back(-1);

		targets.push_back({ 1.0f, 1.0f }); // q1
		ans[0].push_back(1);
		targets.push_back({ -1.0f, 1.0f }); // q2
		ans[0].push_back(1);
		targets.push_back({ -1.0f, -1.0f }); // q3
		ans[0].push_back(-1);
		targets.push_back({ 1.0f, -1.0f }); // q4
		ans[0].push_back(-1);

		for (int j = 0; j < currentDirs.size(); ++j) {
			Vector2 v = currentDirs[j];
			for (int i = 0; i < targets.size(); ++i) {
				res = Vector2::DeterminRotationDirection(v, targets[i]);
				ASSERT(res == ans[j][i]);
			}
		}
	}

	Vector2 headDir{ 0.561699, -0.827389 };
	Vector2 moveDir{ -8.000000, 11.000000 };
	Vector2::DeterminRotationDirection(headDir, moveDir);


	headDir = { 0.600051, -0.800011 };
	moveDir = { -8.000000, 11.000000 };
	Vector2::DeterminRotationDirection(headDir, moveDir);


	return 0;
}