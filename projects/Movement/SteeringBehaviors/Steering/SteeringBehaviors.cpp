//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"

//General
constexpr size_t RAY_COUNT{ 64 };

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, Elite::Color{ 0, 1, 0, 1 });
	}

	return steering;
}

//ContextTutorial
//****
SteeringOutput Context::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	//General variables
	const float DIR_MULTIPLIER{ std::sqrtf(pAgent->GetLinearVelocity().x * pAgent->GetLinearVelocity().x + pAgent->GetLinearVelocity().y * pAgent->GetLinearVelocity().y) / 2 };

	//Seek
	Elite::Vector2 seekDir{};
	seekDir = m_Target.Position - pAgent->GetPosition();
	seekDir.Normalize();

	Elite::Vector2 wanderDir = {};
	Elite::Vector2 circleCenter{ pAgent->GetPosition().x + 6.f * pAgent->GetDirection().x , pAgent->GetPosition().y + 6.f * pAgent->GetDirection().y };
	m_WanderAngle += Elite::randomFloat(-Elite::ToRadians(45), Elite::ToRadians(45));
	Elite::Vector2 goPoint{ circleCenter.x + 4.f * cos(m_WanderAngle), circleCenter.y + 4.f * sin(m_WanderAngle) };
	wanderDir = goPoint - pAgent->GetPosition();
	wanderDir.Normalize();

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dir(RAY_COUNT);
	std::vector<float> pos(RAY_COUNT);
	std::vector<float> neg(RAY_COUNT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * 2 * float(E_PI) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dir[idx] = dirRay * DIR_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ wanderDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dir[idx]) };
		pos[idx] = dotPosDir > 0.f ? dotPosDir : 0.f;

		if (neg[idx] < 1.f)
		{
			//Negative rays
			for (auto obst : m_Obstacles)
			{
				//If ray overlapping with obstacle
				if ((obst->GetCenter() - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dir[idx] * DIR_MULTIPLIER, obst->GetCenter())).MagnitudeSquared() < obst->GetRadius() * obst->GetRadius())
				{
					neg[idx] = 1.f;
					auto invIdx = idx < RAY_COUNT / 2 ? idx + RAY_COUNT / 2 : idx - RAY_COUNT / 2;
					pos[invIdx] += DIR_MULTIPLIER;

					auto next{ idx }, prev{ idx };
					for (size_t amount{}; amount < RAY_COUNT / 16; ++amount)
					{
						next = next == RAY_COUNT - 1 ? 0 : next + 1;
						prev = prev == 0 ? RAY_COUNT - 1 : prev - 1;
						neg[next] = 1.f;
						neg[prev] = 1.f;
					}
				}
				else
				{
					neg[idx] = 0.f;
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		if (neg[idx] > 0.f)
		{
			pos[idx] = 0.f;
		}

		steering.LinearVelocity += dir[idx] * pos[idx];

		if (pAgent->CanRenderBehavior())
		{
			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dir[idx] * pos[idx], (dir[idx] * pos[idx]).Magnitude(), Elite::Color{ 1.f, 1.f, 1.f, 1.f }, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}

//ContextImproved
//****
SteeringOutput ContextImproved::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Own implementation
	SteeringOutput steering = {};

	//General variables
	constexpr float RAY_LENGTH_DEFAULT{ 2.f };
	constexpr float RAY_LENGTH_MULTIPLIER{ 2.f };

	//Start direction
	Elite::Vector2 seekDir = {};
	seekDir = m_Target.Position - pAgent->GetPosition();
	seekDir.Normalize();

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dirRays(RAY_COUNT);
	std::vector<float> weightRays(RAY_COUNT, RAY_LENGTH_DEFAULT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * Elite::ToRadians(360) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dirRays[idx] = dirRay * RAY_LENGTH_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ seekDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dirRays[idx]) };
		weightRays[idx] += dotPosDir;
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		//Negative rays
		for (auto obst : m_Obstacles)
		{
			//If ray overlapping with obstacle
			if ((obst->GetCenter() - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dirRays[idx] * (RAY_LENGTH_DEFAULT + RAY_LENGTH_MULTIPLIER), obst->GetCenter())).MagnitudeSquared() < obst->GetRadius() * obst->GetRadius())
			{
				const float distAgentToObst{ Elite::Distance(obst->GetCenter() - Elite::Vector2{ obst->GetRadius(), obst->GetRadius() }, pAgent->GetPosition() + dirRays[idx]) };

				weightRays[idx] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER);

				for (size_t idx2{}; idx2 < RAY_COUNT; ++idx2)
				{
					int idxDif = idx > idx2 ? idx - idx2 : idx2 - idx;

					if (idxDif < RAY_COUNT / 4)
					{
						weightRays[idx2] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER) * idxDif / (RAY_COUNT / 8);
					}
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		steering.LinearVelocity += dirRays[idx] * weightRays[idx];

		if (pAgent->CanRenderBehavior())
		{
			Elite::Color rayColor{ 1.f, 0.5f, 0.f, 1.f };
			if (weightRays[idx] > RAY_LENGTH_DEFAULT)
			{
				rayColor.g *= weightRays[idx] - RAY_LENGTH_DEFAULT;
				rayColor.r /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}
			else
			{
				rayColor.g /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}

			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dirRays[idx] * weightRays[idx], (dirRays[idx] * weightRays[idx]).Magnitude(), rayColor, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}

//ContextSeek
//****
SteeringOutput ContextSeek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Own implementation
	SteeringOutput steering = {};

	//General variables
	constexpr float RAY_LENGTH_DEFAULT{ 2.f };
	constexpr float RAY_LENGTH_MULTIPLIER{ 2.f };

	//Seek direction
	Elite::Vector2 seekDir = {};
	seekDir = m_Target.Position - pAgent->GetPosition();
	seekDir.Normalize();

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dirRays(RAY_COUNT);
	std::vector<float> weightRays(RAY_COUNT, RAY_LENGTH_DEFAULT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * Elite::ToRadians(360) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dirRays[idx] = dirRay * RAY_LENGTH_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ seekDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dirRays[idx]) };
		weightRays[idx] += dotPosDir;
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		//Negative rays
		for (const auto& obst : m_Obstacles)
		{
			//If ray overlapping with obstacle
			if ((obst.first - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dirRays[idx] * (RAY_LENGTH_DEFAULT + RAY_LENGTH_MULTIPLIER), obst.first)).MagnitudeSquared() < obst.second * obst.second)
			{
				const float distAgentToObst{ Elite::Distance(obst.first - Elite::Vector2{ obst.second, obst.second }, pAgent->GetPosition() + dirRays[idx]) };

				weightRays[idx] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER);

				for (size_t idx2{}; idx2 < RAY_COUNT; ++idx2)
				{
					int idxDif = idx > idx2 ? idx - idx2 : idx2 - idx;

					if (idxDif < RAY_COUNT / 4)
					{
						weightRays[idx2] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER) * idxDif / (RAY_COUNT / 8);
					}
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		steering.LinearVelocity += dirRays[idx] * weightRays[idx];

		if (pAgent->CanRenderBehavior())
		{
			Elite::Color rayColor{ 1.f, 0.5f, 0.f, 1.f };
			if (weightRays[idx] > RAY_LENGTH_DEFAULT)
			{
				rayColor.g *= weightRays[idx] - RAY_LENGTH_DEFAULT;
				rayColor.r /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}
			else
			{
				rayColor.g /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}

			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dirRays[idx] * weightRays[idx], (dirRays[idx] * weightRays[idx]).Magnitude(), rayColor, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}

//ContextFlee
//****
SteeringOutput ContextFlee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Own implementation
	SteeringOutput steering = {};

	//General variables
	constexpr float RAY_LENGTH_DEFAULT{ 2.f };
	constexpr float RAY_LENGTH_MULTIPLIER{ 2.f };

	//Flee direction
	Elite::Vector2 fleeDir = {};
	float distaneToTarget = Distance(pAgent->GetPosition(), m_Target.Position);
	if (distaneToTarget > m_FleeRadius) return SteeringOutput(Elite::ZeroVector2, 0.f, false);
	fleeDir = pAgent->GetPosition() - m_Target.Position;
	fleeDir.Normalize();

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dirRays(RAY_COUNT);
	std::vector<float> weightRays(RAY_COUNT, RAY_LENGTH_DEFAULT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * Elite::ToRadians(360) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dirRays[idx] = dirRay * RAY_LENGTH_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ fleeDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dirRays[idx]) };
		weightRays[idx] += dotPosDir;
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		//Negative rays
		for (const auto& obst : m_Obstacles)
		{
			//If ray overlapping with obstacle
			if ((obst.first - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dirRays[idx] * (RAY_LENGTH_DEFAULT + RAY_LENGTH_MULTIPLIER), obst.first)).MagnitudeSquared() < obst.second * obst.second)
			{
				const float distAgentToObst{ Elite::Distance(obst.first - Elite::Vector2{ obst.second, obst.second }, pAgent->GetPosition() + dirRays[idx]) };

				weightRays[idx] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER);

				for (size_t idx2{}; idx2 < RAY_COUNT; ++idx2)
				{
					int idxDif = idx > idx2 ? idx - idx2 : idx2 - idx;

					if (idxDif < RAY_COUNT / 4)
					{
						weightRays[idx2] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER) * idxDif / (RAY_COUNT / 8);
					}
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		steering.LinearVelocity += dirRays[idx] * weightRays[idx];

		if (pAgent->CanRenderBehavior())
		{
			Elite::Color rayColor{ 1.f, 0.5f, 0.f, 1.f };
			if (weightRays[idx] > RAY_LENGTH_DEFAULT)
			{
				rayColor.g *= weightRays[idx] - RAY_LENGTH_DEFAULT;
				rayColor.r /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}
			else
			{
				rayColor.g /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}

			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dirRays[idx] * weightRays[idx], (dirRays[idx] * weightRays[idx]).Magnitude(), rayColor, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}

//ContextArrive
//****
SteeringOutput ContextArrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Own implementation
	SteeringOutput steering = {};

	//General variables
	constexpr float RAY_LENGTH_DEFAULT{ 2.f };
	constexpr float RAY_LENGTH_MULTIPLIER{ 2.f };

	//Flee direction
	Elite::Vector2 arriveDir = {};
	Elite::Vector2 toTarget = m_Target.Position - pAgent->GetPosition();
	const float distance = toTarget.Magnitude();

	if (distance < m_TargetRadius)
	{
		pAgent->SetLinearVelocity(Elite::ZeroVector2);
	}
	else
	{
		arriveDir = m_Target.Position - pAgent->GetPosition();
		arriveDir.Normalize();
	}

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dirRays(RAY_COUNT);
	std::vector<float> weightRays(RAY_COUNT, RAY_LENGTH_DEFAULT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * Elite::ToRadians(360) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dirRays[idx] = dirRay * RAY_LENGTH_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ arriveDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dirRays[idx]) };
		weightRays[idx] += dotPosDir;
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		//Negative rays
		for (const auto& obst : m_Obstacles)
		{
			//If ray overlapping with obstacle
			if ((obst.first - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dirRays[idx] * (RAY_LENGTH_DEFAULT + RAY_LENGTH_MULTIPLIER), obst.first)).MagnitudeSquared() < obst.second * obst.second)
			{
				const float distAgentToObst{ Elite::Distance(obst.first - Elite::Vector2{ obst.second, obst.second }, pAgent->GetPosition() + dirRays[idx]) };

				weightRays[idx] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER);

				for (size_t idx2{}; idx2 < RAY_COUNT; ++idx2)
				{
					int idxDif = idx > idx2 ? idx - idx2 : idx2 - idx;

					if (idxDif < RAY_COUNT / 4)
					{
						weightRays[idx2] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER) * idxDif / (RAY_COUNT / 8);
					}
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		steering.LinearVelocity += dirRays[idx] * weightRays[idx];

		if (pAgent->CanRenderBehavior())
		{
			Elite::Color rayColor{ 1.f, 0.5f, 0.f, 1.f };
			if (weightRays[idx] > RAY_LENGTH_DEFAULT)
			{
				rayColor.g *= weightRays[idx] - RAY_LENGTH_DEFAULT;
				rayColor.r /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}
			else
			{
				rayColor.g /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}

			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dirRays[idx] * weightRays[idx], (dirRays[idx] * weightRays[idx]).Magnitude(), rayColor, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	if (distance < m_SlowRadius)
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * distance / m_SlowRadius;
	}
	else
	{
		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
	}

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}

//ContextWander
//****
SteeringOutput ContextWander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	//Own implementation
	SteeringOutput steering = {};

	//General variables
	constexpr float RAY_LENGTH_DEFAULT{ 2.f };
	constexpr float RAY_LENGTH_MULTIPLIER{ 2.f };

	//Seek direction
	Elite::Vector2 wanderDir = {};
	Elite::Vector2 circleCenter{ pAgent->GetPosition().x + m_OffsetDistance * pAgent->GetDirection().x , pAgent->GetPosition().y + m_OffsetDistance * pAgent->GetDirection().y };
	m_WanderAngle += Elite::randomFloat(-m_MaxAngleChange, m_MaxAngleChange);
	Elite::Vector2 goPoint{ circleCenter.x + m_Radius * cos(m_WanderAngle), circleCenter.y + m_Radius * sin(m_WanderAngle) };

	wanderDir = goPoint - pAgent->GetPosition();
	wanderDir.Normalize();

	//Prepare vectors of rays	
	std::vector<Elite::Vector2> dirRays(RAY_COUNT);
	std::vector<float> weightRays(RAY_COUNT, RAY_LENGTH_DEFAULT);

	//Make rays
	Elite::Vector2 finalDir{};

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{
		//Direction rays
		const float rayAngle{ idx * Elite::ToRadians(360) / RAY_COUNT };

		Elite::Vector2 dirRay{}, direction{ 1.f, 0.f };
		dirRay.x = direction.x * cos(pAgent->GetOrientation() + rayAngle) - direction.y * sin(pAgent->GetOrientation() + rayAngle);
		dirRay.y = direction.x * sin(pAgent->GetOrientation() + rayAngle) - direction.y * cos(pAgent->GetOrientation() + rayAngle);

		dirRay.Normalize();

		dirRays[idx] = dirRay * RAY_LENGTH_MULTIPLIER;

		//Positive rays
		Elite::Vector2 positiveRay{ wanderDir };

		const float dotPosDir{ Elite::Dot(positiveRay, dirRays[idx]) };
		weightRays[idx] += dotPosDir;
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		//Negative rays
		for (const auto& obst : m_Obstacles)
		{
			//If ray overlapping with obstacle
			if ((obst.first - Elite::ProjectOnLineSegment(pAgent->GetPosition(), pAgent->GetPosition() + dirRays[idx] * (RAY_LENGTH_DEFAULT + RAY_LENGTH_MULTIPLIER), obst.first)).MagnitudeSquared() < obst.second * obst.second)
			{
				const float distAgentToObst{ Elite::Distance(obst.first - Elite::Vector2{ obst.second, obst.second }, pAgent->GetPosition() + dirRays[idx]) };

				weightRays[idx] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER);

				for (size_t idx2{}; idx2 < RAY_COUNT; ++idx2)
				{
					int idxDif = idx > idx2 ? idx - idx2 : idx2 - idx;

					if (idxDif < RAY_COUNT / 4)
					{
						weightRays[idx2] *= distAgentToObst / (RAY_LENGTH_DEFAULT * RAY_LENGTH_MULTIPLIER) * idxDif / (RAY_COUNT / 8);
					}
				}
			}
		}
	}

	for (size_t idx{}; idx < RAY_COUNT; ++idx)
	{

		steering.LinearVelocity += dirRays[idx] * weightRays[idx];

		if (pAgent->CanRenderBehavior())
		{
			Elite::Color rayColor{ 1.f, 0.5f, 0.f, 1.f };
			if (weightRays[idx] > RAY_LENGTH_DEFAULT)
			{
				rayColor.g *= weightRays[idx] - RAY_LENGTH_DEFAULT;
				rayColor.r /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}
			else
			{
				rayColor.g /= weightRays[idx] - RAY_LENGTH_DEFAULT;
			}

			DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), dirRays[idx] * weightRays[idx], (dirRays[idx] * weightRays[idx]).Magnitude(), rayColor, 0.2f);
		}
	}

	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->CanRenderBehavior())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, steering.LinearVelocity.Magnitude(), Elite::Color{ 0.f, 0.f, 1.f, 1.f }, 0.1f);
	}

	return steering;
}