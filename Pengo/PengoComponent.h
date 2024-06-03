#pragma once
#include <memory>
#include "Component.h"
#include "PengoState.h"

class PengoComponent : public dae::Component
{
public:
	void Update(float /*deltaTime*/) override;
	void LateUpdate(float /*deltaTime*/) override {}
	void FixedUpdate(float /*fixedTimeStep*/) override {}
	void Render() const override {}

	void HandleInput(Controlls control);

	void SetState(std::unique_ptr<PengoState> state) 
	{ 
		m_State = std::move(state); 
	}

	void SetCurrentDirection(glm::vec3 direction) { m_CurrentDirection = direction; }
	glm::vec3 GetCurrentDirection() { return m_CurrentDirection; }

	void SetPengoIsKilled() 
	{ 
		m_IsDead = true; 
		SetState(std::make_unique<DyingState>(GetOwner()));
	}
	bool IsPengoKilled() { return m_IsDead; }

	PengoComponent(dae::GameObject* gameObject) : dae::Component(gameObject) { m_State = std::make_unique<Idle>(); }
	virtual ~PengoComponent() 
	{ 
		//m_MovementState = nullptr; delete m_MovementState;
	}
	PengoComponent(const PengoComponent& other) = delete;
	PengoComponent(PengoComponent&& other) = delete;
	PengoComponent& operator=(const PengoComponent& other) = delete;
	PengoComponent& operator=(PengoComponent&& other) = delete;
private:
	std::unique_ptr<PengoState> m_State;

	glm::vec3 m_CurrentDirection;

	bool m_IsDead;
};

