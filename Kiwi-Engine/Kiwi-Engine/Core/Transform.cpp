#include "Transform.h"
#include "Math.h"
#include "Entity.h"

#include "../Physics/Rigidbody.h"

namespace Kiwi
{

	Transform::Transform()
	{

		m_scale = Kiwi::Vector3d( 1.0, 1.0, 1.0 );
		m_lockYaw = m_lockPitch = m_lockRoll = false;
		m_rigidbody = 0;

	}

	Transform::~Transform()
	{
	}

	void Transform::_TranslateChildren( const Kiwi::Vector3d translation )
	{

		//rotate child entities around this transform
		/*auto itr = m_childTransforms.begin();
		for( ; itr != m_childTransforms.end(); itr++ )
		{
			(*itr)->Translate( translation );
		}*/

	}

	void Transform::_UpdateGlobalPosition()
	{

		if( m_entity != 0 && m_entity->GetParent() != 0 && m_entity->GetParent()->GetType() == m_entity->GetType() )
		{
			Kiwi::Transform* parentTransform = m_entity->GetParent()->FindComponent<Kiwi::Transform>();
			if( parentTransform != 0 )
			{
				m_globalPosition = m_position + parentTransform->GetGlobalPosition();

			} else
			{
				m_globalPosition = m_position;
			}

		} else
		{
			m_globalPosition = m_position;
		}

	}

	void Transform::_UpdateChildTransforms()
	{

		if( m_entity != 0 && m_entity->HasChild() )
		{
			Kiwi::EntityMap children = m_entity->GetChildren();

			for( auto itr = children.begin(); itr != children.end(); itr++ )
			{
				Kiwi::Transform* trans = itr->second->FindComponent<Kiwi::Transform>();
				if( trans != 0 )
				{
					trans->Update();
				}
			}
		}

	}

	void Transform::Update()
	{

		this->_UpdateGlobalPosition();

	}

	void Transform::Translate( double x, double y, double z )
	{

		Kiwi::Vector3 translation( x, y, z );
		Kiwi::TransformEvent transEvent( this, Kiwi::TransformEvent::TRANSFORM_TRANSLATION, translation, m_position );

		m_position += translation;
		this->_UpdateGlobalPosition();
		this->_UpdateChildTransforms();

		//this->_TranslateChildren( translation );

		this->BroadcastTransformEvent( transEvent );

	}

	void Transform::Translate( const Kiwi::Vector3& translation )
	{

		Kiwi::TransformEvent transEvent( this, Kiwi::TransformEvent::TRANSFORM_TRANSLATION, translation, m_position );

		m_position += translation;
		this->_UpdateGlobalPosition();
		this->_UpdateChildTransforms();
		//this->_TranslateChildren( translation );

		this->BroadcastTransformEvent( transEvent );

	}

	void Transform::Translate( const Kiwi::Vector3d& translation )
	{

		Kiwi::TransformEvent transEvent( this, Kiwi::TransformEvent::TRANSFORM_TRANSLATION, translation, m_position );

		m_position += translation;
		this->_UpdateGlobalPosition();
		this->_UpdateChildTransforms();
		//this->_TranslateChildren( translation );

		this->BroadcastTransformEvent( transEvent );

	}

	void Transform::SetPosition( const Kiwi::Vector3& newPosition )
	{

		this->SetPosition( Kiwi::Vector3d( newPosition.x, newPosition.y, newPosition.z ) );

	}

	void Transform::SetPosition( const Kiwi::Vector3d& newPosition )
	{

		Kiwi::Vector3d translation = newPosition - m_position;

		Kiwi::TransformEvent transEvent( this, Kiwi::TransformEvent::TRANSFORM_TRANSLATION, translation, m_position );

		m_position = newPosition;
		this->_UpdateGlobalPosition();
		this->_UpdateChildTransforms();
		//this->_TranslateChildren( translation );

		this->BroadcastTransformEvent( transEvent );

	}

	void Transform::SetPosition( const Kiwi::Transform& transform )
	{

		Kiwi::Vector3d translation = transform.GetPosition() - m_position;

		Kiwi::TransformEvent transEvent( this, Kiwi::TransformEvent::TRANSFORM_TRANSLATION, translation, m_position );

		m_position = transform.GetPosition();
		this->_UpdateGlobalPosition();
		this->_UpdateChildTransforms();
		//this->_TranslateChildren( translation );

		this->BroadcastTransformEvent( transEvent );

	}

	void Transform::SetGlobalPosition( double x, double y, double z )
	{

		this->SetGlobalPosition( Kiwi::Vector3d( x, y, z ) );

	}

	void Transform::SetGlobalPosition( const Kiwi::Vector3d& position )
	{

		m_globalPosition = position;

		if( m_entity != 0 && m_entity->GetParent() != 0 )
		{
			Kiwi::Transform* parentTransform = m_entity->GetParent()->FindComponent<Kiwi::Transform>();
			if( parentTransform != 0 )
			{
				m_position = m_globalPosition - parentTransform->GetGlobalPosition();

			} else
			{
				m_position = m_globalPosition;
			}

		} else
		{
			m_position = m_globalPosition;
		}

		this->_UpdateChildTransforms();

	}

	void Transform::AttachTransform( Kiwi::Transform* transform )
	{

		if( transform )
		{
			auto itr = m_childTransforms.begin();
			for( ; itr != m_childTransforms.end(); itr++ )
			{
				if( *itr == transform )
				{
					return;
				}
			}

			m_childTransforms.push_back( transform );

			//add the parent entity as a transform listener so it gets notified whenever this new transform updates
			//transform->AddTransformListener( m_entity );
		}

	}

	void Transform::DetachTransform( Kiwi::Transform* transform )
	{

		if( transform )
		{
			auto itr = m_childTransforms.begin();
			for( ; itr != m_childTransforms.end(); itr++ )
			{
				if( *itr == transform )
				{
					itr = m_childTransforms.erase( itr );
					return;
				}
			}
		}

	}

	void Transform::Rotate(const Kiwi::Vector3d& rotationAxis, double rotationAmount)
	{

		Kiwi::Quaternion rotQuat(rotationAxis, rotationAmount);

		this->Rotate(rotQuat);

	}

	void Transform::Rotate(const Kiwi::Quaternion& rotation)
	{

		Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, 0.0, this, rotation, m_rotation );

		//multiply the current rotation with the new rotation to get the final rotation
		m_rotation = rotation.Cross(m_rotation).Normalized();

		this->BroadcastTransformEvent( rotEvent );

		//if an axis is locked, we need to rotate that axis back to its fixed rotation value
		if(m_lockRoll)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.z != m_lockPosition.z)
			{
				Kiwi::Quaternion zRot(this->GetForward(),-(eulerAngles.z - m_lockPosition.z)); 

				Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.z - m_lockPosition.z), this, zRot, m_rotation );

				m_rotation = zRot.Cross(m_rotation).Normalized();

				this->BroadcastTransformEvent( rotEvent );
			}

		}

		if(m_lockPitch)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.x != m_lockPosition.x)
			{
				Kiwi::Quaternion xRot(this->GetRight(),-(eulerAngles.x - m_lockPosition.x)); 

				Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.x - m_lockPosition.x), this, xRot, m_rotation );

				m_rotation = xRot.Cross(m_rotation).Normalized();

				this->BroadcastTransformEvent( rotEvent );
			}

		}

		if(m_lockYaw)
		{

			Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

			if(eulerAngles.y != m_lockPosition.y)
			{
				Kiwi::Quaternion yRot(this->GetUp(),-(eulerAngles.y - m_lockPosition.y));

				Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, -(eulerAngles.y - m_lockPosition.y), this, yRot, m_rotation );

				m_rotation = yRot.Cross(m_rotation).Normalized();

				this->BroadcastTransformEvent( rotEvent );
			}

		}

		//rotate child entities around this transform as well
		auto itr = m_childTransforms.begin();
		for( ; itr != m_childTransforms.end(); itr++ )
		{
			if( *itr == 0 || *itr == this )
			{
				continue;

			} else
			{
				(*itr)->RotateAround( m_position, rotation );
			}
		}

	}

	void Transform::RotateAround( const Kiwi::Vector3& point, const Kiwi::Quaternion& rotation )
	{

		//put the point into a quaternion to do the rotation
		Quaternion vq( 0.0f, point.x, point.y, point.z );

		Quaternion qr = rotation.Cross( vq );
		qr = qr.Cross( rotation.Conjugate() );

		Kiwi::Vector3d newPos = m_position;
		newPos += Kiwi::Vector3d( qr.x, qr.y, qr.z );
		this->SetPosition( newPos );

	}

	void Transform::RotateAround( const Kiwi::Vector3d& point, const Kiwi::Quaternion& rotation )
	{

		//put the point into a quaternion to do the rotation
		Quaternion vq( 0.0f, point.x, point.y, point.z );

		Quaternion qr = rotation.Cross( vq );
		qr = qr.Cross( rotation.Conjugate() );

		Kiwi::Vector3d newPos = m_position;
		newPos += Kiwi::Vector3d( qr.x, qr.y, qr.z );
		this->SetPosition( newPos );

	}

	void Transform::SetRotation( const Kiwi::Quaternion& newRotation )
	{

		//calculate the difference between the new rotation and the old
		/*
		diff * q1 = q2  --->  diff = q2 * inverse(q1)
		where:  inverse(q1) = conjugate(q1) / abs(q1)
		and:  conjugate( quaternion(re, i, j, k) ) = quaternion(re, -i, -j, -k)
		*/
		Kiwi::Quaternion diff = newRotation.Cross( m_rotation.Inverse() ).Normalized();

		Kiwi::TransformEvent rotEvent( this, Kiwi::TransformEvent::TRANSFORM_ROTATION, 0.0, this, diff, m_rotation );

		m_rotation = newRotation;

		this->BroadcastTransformEvent( rotEvent );

		//rotate child entities around this transform by the difference
		auto itr = m_childTransforms.begin();
		for( ; itr != m_childTransforms.end(); itr++ )
		{
			if( *itr == 0 || *itr == this )
			{
				continue;

			} else
			{
				(*itr)->Rotate( diff );
			}
		}

	}

	void Transform::SetRotation( const Kiwi::Transform& transform )
	{

		this->SetRotation( transform.GetRotation() );

	}

	void Transform::RotateTowards(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up, double maxRotation)
	{

		if(m_position == target) return;

		Kiwi::Vector3d forwardDifference = (target - m_position).Normalized();
		Kiwi::Vector3d forward = Kiwi::Vector3d::forward();

		Kiwi::Vector3d rotationAxis = forward.Cross( forwardDifference ).Normalized();

		double dot = forward.Dot(forwardDifference);
		if( std::abs(dot + 1.0) < 0.000001 )
		{
			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, KIWI_PI);
			return;

		}else if( std::abs(dot - 1.0) < 0.000001 )
		{
			//already facing each other, do nothing
			return;
		}

		double rotationAngle = std::acos( dot );

		Kiwi::Quaternion rotation( rotationAxis, rotationAngle );
		m_rotation = rotation;

		double zAngle = Kiwi::Vector3d::right().Dot( this->GetRight() );
		if( zAngle != 0.0 )
		{
			this->Rotate( this->GetForward(), -zAngle );
		}

		/*Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}*/

		//calculate the vector between the current position and the target
		//Kiwi::Vector3d vectorBetween = (target - m_position).Normalized();

		//Kiwi::Vector3d forward = this->GetForward();
		//Kiwi::Vector3d up = this->GetUp();

		//double dot = forward.Dot(vectorBetween);

		//if( dot < -0.99998 )
		//{
		//	//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
		//	this->Rotate(up, KIWI_PI);
		//	return;

		//}else if( dot > 0.999998 )
		//{
		//	//already facing each other, do nothing
		//	return;
		//}

		//Kiwi::Vector3d rotationAxis = forward.Cross(vectorBetween);

		//double angle = std::acos( dot );
		////angle = ToDegrees(angle);

		//if( maxRotation > 0.000001 && angle > maxRotation )
		//{
		//	angle = maxRotation;
		//}

		//Kiwi::Quaternion rotation( rotationAxis, angle );

		//this->Rotate(rotationAxis, angle);	

		////now the quaternion is facing the target, but it may be rotated around the z-axis
		////need to rotate around the z-axis to the desired up direction
		//Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		//if(eulerAngles.z != 0.0)
		//{
		//	this->Rotate(this->GetForward(),-eulerAngles.z); 
		//}

	}

	void Transform::RotateTowards(const Transform& target, const Kiwi::Vector3d& up, double maxRotation)
	{

		this->RotateTowards(target.GetPosition(), up, maxRotation);

	}

	void Transform::RotateAway(const Kiwi::Vector3d& target, const Kiwi::Vector3d& up, double maxRotation)
	{


		if(m_position == target) return;

		//calculate the vector between the current position and the target
		Kiwi::Vector3 vectorBetween(target.x - m_position.x, target.y - m_position.y, target.z - m_position.z);
		vectorBetween = vectorBetween.Normalized();

		Kiwi::Vector3d forward = this->GetForward().Scaled(-1.0);

		double dot = forward.Dot(vectorBetween);

		if( dot < -0.99998 )
		{

			//currently pointing in exactly opposite directions so rotate 180 degrees about the up axis
			this->Rotate(up, KIWI_PI);
			return;

		}else if( dot > 0.99998 )
		{
			//already facing each other, do nothing
			return;

		}

		Kiwi::Vector3d rotationAxis = forward.Cross(vectorBetween);

		double angle = std::acos( forward.Dot(vectorBetween) );

		if(maxRotation != 0.0 && angle > maxRotation)
		{
			angle = maxRotation;
		}

		this->Rotate(rotationAxis, angle);	

		//now the quaternion is facing the target, but it may be rotated around the z-axis
		//need to rotate around the z-axis to the desired up direction
		Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();

		if(eulerAngles.z != 0.0)
		{
			this->Rotate(this->GetForward(),-eulerAngles.z); 
		}

	}

	void Transform::RotateAway(const Transform& target, const Kiwi::Vector3d& up, double maxRotation)
	{

		this->RotateAway(target.GetPosition(), up, maxRotation);

	}

	/*locks or unlocks the axis to the current rotation, depending on the value of 'lock'*/
	void Transform::LockAxis(Kiwi::AXIS axis, bool lock)
	{

		switch(axis)
		{
		case Kiwi::AXIS::X_AXIS:
			{
				m_lockPitch = lock;
				break;
			}
		case Kiwi::AXIS::Y_AXIS:
			{
				m_lockYaw = lock;
				break;
			}
		case Kiwi::AXIS::Z_AXIS:
			{
				m_lockRoll = lock;
				break;
			}
		default:break;
		}

	}

	/*locks the axis to the specified rotation*/
	void Transform::LockAxisTo(Kiwi::AXIS axis, double radians)
	{

		switch(axis)
		{
		case Kiwi::AXIS::X_AXIS:
			{
				m_lockPitch = true;
				m_lockPosition.x = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.x != m_lockPosition.x)
				{
					Kiwi::Quaternion xRot( this->GetRight(), -(eulerAngles.x - m_lockPosition.x) );
					m_rotation = xRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Y_AXIS:
			{
				m_lockYaw = true;
				m_lockPosition.y = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.y != m_lockPosition.y)
				{
					Kiwi::Quaternion yRot( this->GetUp(), -(eulerAngles.y - m_lockPosition.y) );
					m_rotation = yRot.Cross(m_rotation);
				}

				break;
			}
		case Kiwi::AXIS::Z_AXIS:
			{
				m_lockRoll = true;
				m_lockPosition.z = radians;

				Kiwi::Vector3 eulerAngles = m_rotation.GetEulerAngles();
				if(eulerAngles.z != m_lockPosition.z)
				{
					Kiwi::Quaternion zRot( this->GetForward(), -(eulerAngles.z - m_lockPosition.z) );
					m_rotation = zRot.Cross(m_rotation);
				}

				break;
			}
		default:break;
		}

	}

	/*returns the distance between the two transforms*/
	double Transform::GetDistance(Kiwi::Transform* targetTransform)
	{

		return std::sqrtf( this->GetSquareDistance(targetTransform) );

	}

	/*returns the distance between this transform and the target vector*/
	double Transform::GetDistance( const Kiwi::Vector3d& target )
	{

		return std::sqrtf( this->GetSquareDistance( target ) );

	}

	/*returns the square of the distance between the two transforms*/
	double Transform::GetSquareDistance( Kiwi::Transform* targetTransform )
	{

		if( targetTransform == 0 )
		{
			return 0.0f;
		}

		return ( std::powf( (targetTransform->GetGlobalPosition().x - m_globalPosition.x), 2) +
				 std::powf( (targetTransform->GetGlobalPosition().y - m_globalPosition.y), 2) +
				 std::powf( (targetTransform->GetGlobalPosition().z - m_globalPosition.z), 2) );

	}

	/*returns the square of the distance between this transform and the target vector*/
	double Transform::GetSquareDistance( const Kiwi::Vector3d& target )
	{

		return ( std::pow( (target.x - m_globalPosition.x), 2 ) +
				 std::pow( (target.y - m_globalPosition.y), 2 ) +
				 std::pow( (target.z - m_globalPosition.z), 2 ) );

	}

	Kiwi::Matrix4 Transform::GetWorldMatrix()
	{

		Kiwi::Matrix4 translate = Kiwi::Matrix4::Translation(m_globalPosition);
		Kiwi::Matrix4 scale = Kiwi::Matrix4::Scaling(m_scale);
		Kiwi::Matrix4 rotate = m_rotation.ToRotationMatrix();
		
		/* using row-major matrices so to get proper scaling when rotating need to use scale*rotate*translate
		if using column major matrices, need to use rotate*scale*translate */
		Kiwi::Matrix4 world = (scale * rotate) * translate;
		return world;

	}

	Kiwi::Vector3d Transform::GetForward()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::forward()).Normalized(); 
	}

	Kiwi::Vector3d Transform::GetUp()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::up()).Normalized();
	}

	Kiwi::Vector3d Transform::GetRight()const 
	{ 
		return m_rotation.RotatePoint(Kiwi::Vector3d::right()).Normalized();
	}

};