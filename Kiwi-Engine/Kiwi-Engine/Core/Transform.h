#ifndef _KIWI_TRANSFORM_H_
#define _KIWI_TRANSFORM_H_

#include "Component.h"

#include "Vector3.h"
#include "Vector3d.h"
#include "Quaternion.h"
#include "Matrix4.h"

#include "Events\TransformEventBroadcaster.h"

#include <vector>
#include <list>

namespace Kiwi
{

	class Entity;
	class Rigidbody;

	enum AXIS { X_AXIS = 0, Y_AXIS, Z_AXIS };

	class Transform:
		public Kiwi::Component,
		public Kiwi::TransformEventBroadcaster
	{
	protected:

		Kiwi::Rigidbody* m_rigidbody;

		Kiwi::Quaternion m_rotation;

		Kiwi::Vector3d m_position; //local position, relative to the parent, if there is no parent this is the same as the global position
		Kiwi::Vector3d m_globalPosition;

		Kiwi::Vector3d m_scale;

		//stores which axis' are locked
		bool m_lockYaw, m_lockPitch, m_lockRoll;
		//stores the rotation value that each axis should be locked to
		Kiwi::Vector3d m_lockPosition;

		std::list<Kiwi::Transform*> m_childTransforms;

	protected:

		void _TranslateChildren( const Kiwi::Vector3d translation );
		void _UpdateGlobalPosition();
		void _UpdateChildTransforms();

	public:

		Transform();
		~Transform();

		void Update();

		/*translates the object by the given amount in each direction*/
		void Translate( double x, double y, double z );
		/*translates the object by the given amount in each direction*/
		void Translate( const Kiwi::Vector3& translation );
		void Translate( const Kiwi::Vector3d& translation );

		/*sets the position of the object equal to newPosition*/
		void SetPosition( const Kiwi::Vector3& newPosition );
		void SetPosition( const Kiwi::Vector3d& newPosition );
		void SetPosition( const Kiwi::Transform& transform );

		void SetGlobalPosition( double x, double y, double z );
		void SetGlobalPosition( const Kiwi::Vector3d& position );

		void SetHeight(float height) { m_position.y = height; }

		void SetScale( double scale ) { m_scale.Set( scale, scale, scale ); }
		void SetScale(const Kiwi::Vector3& scale) { m_scale = scale; }
		void SetScale( const Kiwi::Vector3d& scale ) { m_scale = scale; }

		void AttachTransform( Kiwi::Transform* transform );
		void DetachTransform( Kiwi::Transform* transform );

		void Rotate(const Kiwi::Vector3d& rotationAxis, double rotationAmount);

		/*rotates the object by the given quaternion*/
		void Rotate(const Kiwi::Quaternion& rotation);

		void RotateAround( const Kiwi::Vector3& point, const Kiwi::Quaternion& rotation );
		void RotateAround( const Kiwi::Vector3d& point, const Kiwi::Quaternion& rotation );

		/*sets the rotation of the object equal to newRotation*/
		void SetRotation( const Kiwi::Quaternion& newRotation );

		/*sets the rotation to that of the given transform's*/
		void SetRotation( const Kiwi::Transform& transform );

		/*rotates the transform so that the local z+ axis passes through the target point*/
		void RotateTowards(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up = Kiwi::Vector3d::up(), double maxRotation = 0.0);

		/*rotates the transform so that the local z+ axis passes through the target transform*/
		void RotateTowards(const Kiwi::Transform& target, const Kiwi::Vector3d& up = Kiwi::Vector3d::up(), double maxRotation = 0.0);

		void RotateAway(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up = Kiwi::Vector3d::up(), double maxRotation = 0.0);
		void RotateAway(const Kiwi::Transform& target, const Kiwi::Vector3d& up = Kiwi::Vector3d::up(), double maxRotation = 0.0);

		/*locks or unlocks the axis to the current rotation, depending on the value of 'lock'*/
		void LockAxis(Kiwi::AXIS axis, bool lock);
		/*locks the axis to the specified rotation*/
		void LockAxisTo( Kiwi::AXIS axis, double radians );

		/*returns the distance between the two transforms*/
		double GetDistance( Kiwi::Transform* target );
		/*returns the distance between this transform and the target vector*/
		double GetDistance(const Kiwi::Vector3d& target);

		/*returns the square of the distance between the two transforms*/
		double GetSquareDistance(Kiwi::Transform* target);
		/*returns the square of the distance between this transform and the target vector*/
		double GetSquareDistance(const Kiwi::Vector3d& target);

		const Kiwi::Vector3d& GetGlobalPosition()const { return m_globalPosition; }
		const Kiwi::Vector3d& GetPosition()const { return m_position; }
		const Kiwi::Quaternion& GetRotation()const { return m_rotation; }
		const Kiwi::Vector3d& GetScale()const { return m_scale; }

		Kiwi::Matrix4 GetWorldMatrix();

		Kiwi::Vector3d GetForward()const;
		Kiwi::Vector3d GetUp()const;
		Kiwi::Vector3d GetRight()const;

	};
};

#endif