#ifndef _KIWI_RIGIDBODY_H_
#define _KIWI_RIGIDBODY_H_

#include "Collider.h"

#include "../Core/Component.h"
#include "../Core/Vector3d.h"

namespace Kiwi
{

	class Transform;
	class Entity;

	class Rigidbody:
		public Kiwi::Component,
		public Kiwi::ICollisionEventBroadcaster,
		public Kiwi::ICollisionEventListener
	{
	protected:

		Kiwi::Collider* m_collider;

		Kiwi::Vector3d m_angularVelocity;

		Kiwi::Vector3d m_acceleration;
		Kiwi::Vector3d m_velocity;

		Kiwi::Vector3d m_maxVelocity;

		Kiwi::Vector3d m_appliedForce; //force exerted on this rigidbody
		Kiwi::Vector3d m_exertedForce; //force exerted by the rigidbody
		Kiwi::Vector3d m_normalForce;

		double m_mass;
		double m_frictionKinetic;
		double m_frictionStatic;
		double m_appliedFriction;

		bool m_isKinematic;

	protected:

		virtual void _OnFixedUpdate();
		virtual void _OnShutdown();
		virtual void _OnAttached();

	public:

		Rigidbody();
		~Rigidbody();

		void SetMass( double mass ) { m_mass = mass; }
		void SetMaxVelocity( const Kiwi::Vector3d& maxVelocity ) { m_maxVelocity = maxVelocity; }
		void SetVelocity( const Kiwi::Vector3d& velocity ) { m_velocity = velocity; }
		void SetAngularVelocity( const Kiwi::Vector3d& velocity ) { m_angularVelocity = velocity; }
		void SetAcceleration( const Kiwi::Vector3d& acceleration ) { m_acceleration = acceleration; }
		void SetActive( bool active ) { m_isActive = active; }
		void SetStaticFriction( double coefficientOfFriction ) { m_frictionStatic = coefficientOfFriction; }
		
		void SetKinematic( bool isKinematic ) { m_isKinematic = isKinematic; }

		void AddVelocity( const Kiwi::Vector3d& velocity ) { m_velocity += velocity; }
		void AddAngularVelocity( const Kiwi::Vector3d& velocity ) { m_angularVelocity += velocity; }
		void Accelerate( const Kiwi::Vector3d& acceleration ) { this->ApplyForce( acceleration * m_mass ); }

		void ApplyForce( const Kiwi::Vector3d& force ) { m_appliedForce += force; }

		Kiwi::Collider* AttachComponent( Kiwi::Collider* collider );
		void DestroyCollider();

		const Kiwi::Vector3d& GetVelocity()const { return m_velocity; }
		const Kiwi::Vector3d& GetAngularVelocity()const { return m_angularVelocity; }
		const Kiwi::Vector3d& GetAcceleration()const { return m_acceleration; }
		const Kiwi::Vector3d& GetExertedForce()const { return m_exertedForce; }

		double GetMass()const { return m_mass; }

		bool IsKinematic()const { return m_isKinematic; }

		Kiwi::Collider* GetCollider() { return m_collider; }

		void OnCollisionEnter( const Kiwi::CollisionEvent& collision );
		void OnCollisionExit( const Kiwi::CollisionEvent& collision );
		void OnCollision( const Kiwi::CollisionEvent& collision );

	};
}

#endif