/*=============================================================================*/
// Copyright 2021-2022 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// SteeringBehaviors.h: SteeringBehaviors interface and different implementations
/*=============================================================================*/
#ifndef ELITE_STEERINGBEHAVIORS
#define ELITE_STEERINGBEHAVIORS

//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "../SteeringHelpers.h"
class SteeringAgent;
class Obstacle;

#pragma region **ISTEERINGBEHAVIOR** (BASE)
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	virtual SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) = 0;

	void SetTarget(const TargetData& target) { m_Target = target; }

	template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
	T* As()
	{
		return static_cast<T*>(this);
	}

protected:
	TargetData m_Target;
};
#pragma endregion

/////////////////////////
//Base seek
//****
class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;

	//Seek Behaviour
	SteeringOutput CalculateSteering(float deltaT, SteeringAgent* pAgent) override;
};

/////////////////////////
//Tutorial
//****
class Context : public Seek
{
public:
	Context(const std::vector<Obstacle*>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~Context() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

private:
	const std::vector<Obstacle*> m_Obstacles;
	float m_WanderAngle = Elite::ToRadians(90);
};

/////////////////////////
//Improved
//****
class ContextImproved : public Seek
{
public:
	ContextImproved(const std::vector<Obstacle*>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~ContextImproved() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

private:
	const std::vector<Obstacle*> m_Obstacles;
};

/////////////////////////
//Improved seek
//****
class ContextSeek : public Seek
{
public:
	ContextSeek(const std::vector<std::pair<Elite::Vector2, float>>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~ContextSeek() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

private:
	const std::vector<std::pair<Elite::Vector2, float>> m_Obstacles;
};

/////////////////////////
//Improved flee
//****
class ContextFlee : public Seek
{
public:
	ContextFlee(const std::vector<std::pair<Elite::Vector2, float>>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~ContextFlee() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

	void SetFleeRadius(float radius) { m_FleeRadius = radius; }

private:
	const std::vector<std::pair<Elite::Vector2, float>> m_Obstacles;

	float m_FleeRadius = 25.f;
};

/////////////////////////
//Improved arrive
//****
class ContextArrive : public Seek
{
public:
	ContextArrive(const std::vector<std::pair<Elite::Vector2, float>>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~ContextArrive() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

	void SetSlowRadius(float slowRadius) { m_SlowRadius = slowRadius; }
	void SetTargetRadius(float targetRadius) { m_TargetRadius = targetRadius; }

private:
	const std::vector<std::pair<Elite::Vector2, float>> m_Obstacles;

	float m_SlowRadius = 15.f;
	float m_TargetRadius = 1.f;
};

/////////////////////////
//Improved wander
//****
class ContextWander : public Seek
{
public:
	ContextWander(const std::vector<std::pair<Elite::Vector2, float>>& obstacles)
		: m_Obstacles{ obstacles }
	{}
	virtual ~ContextWander() = default;

	SteeringOutput CalculateSteering(float delaT, SteeringAgent* pAgent) override;

	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	void SetWanderRadius(float radius) { m_Radius = radius; }
	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

private:
	const std::vector<std::pair<Elite::Vector2, float>> m_Obstacles;

	float m_OffsetDistance = 6.f;
	float m_Radius = 4.f;
	float m_MaxAngleChange = Elite::ToRadians(45);
	float m_WanderAngle = 0.f;
};
#endif