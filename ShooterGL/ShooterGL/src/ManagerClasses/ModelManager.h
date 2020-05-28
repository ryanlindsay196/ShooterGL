#pragma once

#include <vector>
#include "Renderables/Model.h"

class TextureManager;
class ObjectManager;

class ModelManager
{
private:
	std::vector<Model*> models;

	TextureManager* m_textureManager;
	ObjectManager* m_objectManager;

public:
	ModelManager();
	~ModelManager();

	void Initialize(ObjectManager * objectManager);

	Model* LoadModel(glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	Model* LoadModel(char* modelPath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);
	Model* LoadModel(char* modelPath, char* texturePath, glm::vec3 positionOffset, glm::vec3 rotationAxis, float rotationAngle, glm::vec3 scaleOffset);

	void UpdateModels(float gameTime);

	void RenderModels();

	Model* GetModel(int index);
};