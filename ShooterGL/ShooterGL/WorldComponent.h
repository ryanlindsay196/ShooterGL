#pragma once
#include "Component.h"

#define M_PI   3.14159265358979323846264338327950288

class WorldComponent : public Component
{
protected:
	glm::mat4 offsetTransform;

	glm::quat rotationQuat;
	glm::mat4 positionOffset, rotationOffset, scaleOffset;
	glm::vec3 direction;
	float yaw, pitch, roll;
	void SetEulerAngles(glm::vec3 newEulers);
public:
	virtual void Initialize();
	virtual void Update(float gameTime) override;

	bool isActive;
	inline glm::mat4 GetOffsetTransform() { return offsetTransform; }
	void CalculateTransform();

	void SetTransform(glm::mat4 newTransform);
	void Translate(glm::vec3 translateBy);
	void Rotate(glm::vec3 rotateBy);
	void Scale(glm::vec3 scaleBy);
	glm::vec3 Rotation();

	void RotateEulerAngles(glm::vec3 newEulers);
	void RotateQuaternion(glm::vec3 rotationAxis, float angle);
	void SetQuaternion(glm::vec3 rotationAxis, float angle);
	glm::vec3 GetEulerAngles();
};