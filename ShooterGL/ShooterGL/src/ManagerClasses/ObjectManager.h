#pragma once
#include <string>

class EntityManager;
class ModelManager;
class TextureManager;
class CameraManager;
class ControllerManager;
class LightManager;
class ShaderManager;
class PhysicsManager;
class NetworkManager;

//TODO: include all managers
struct GLFWwindow;

class ObjectManager
{
public:
	EntityManager* entityManager;
	ModelManager* modelManager;
	TextureManager* textureManager;
	CameraManager* cameraManager;
	ControllerManager* controllerManager;
	LightManager* lightManager;
	ShaderManager* shaderManager;
	PhysicsManager* physicsManager;
	NetworkManager* networkManager;

	void Initialize(GLFWwindow* window);
	void Update(float gameTime);
	void Render();
	void LoadScene(std::string scenePath, GLFWwindow* window);
private:
	float fixedUpdateTimer, fixedUpdateMaxTime;
};