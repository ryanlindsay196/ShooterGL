#include "Controller.h"
#include "GLFW/glfw3.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
float lastX, lastY, xoffset, yoffset;
bool mouseButtonPressed0, mouseButtonDown0, mouseButtonReleased0;
bool mouseButtonPressed1, mouseButtonDown1, mouseButtonReleased1;
bool firstMouse;

void Controller::Initialize(GLFWwindow* newWindow)
{
	window = newWindow;
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	firstMouse = true;
}

void Controller::AddKeyBinding()
{
}

void Controller::Update(float gameTime)
{
	glm::vec3 moveDirection = glm::vec3(0);
	if (glfwGetKey(window, GLFW_KEY_W))
		moveDirection -= componentParent->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_S))
		moveDirection += componentParent->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_A))
		moveDirection += glm::cross(componentParent->GetDirection(), glm::vec3(0, 1, 0));
	if (glfwGetKey(window, GLFW_KEY_D))
		moveDirection -= glm::cross(componentParent->GetDirection(), glm::vec3(0, 1, 0));

	//Keeps the player from moving up/down
	moveDirection.y = 0;
	if (mouseButtonDown0)
		moveDirection += glm::vec3(0, 0.001, 0);
	if(mouseButtonDown1)
		moveDirection -= glm::vec3(0, 0.001, 0);

	Move(moveDirection, -3.f * gameTime);

	//Rotate the entity based on the mouse offset from the center of the screen
	if (xoffset != 0 || yoffset != 0)
	{
		componentParent->SetEulerAngles(componentParent->GetEulerAngles() + glm::vec3(yoffset, xoffset, 0));
		xoffset = 0;
		yoffset = 0;

		//Controller can't make parent entity look above 89 degrees or below -89 degrees
		float pitch = componentParent->GetEulerAngles().x;
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		componentParent->SetEulerAngles(glm::vec3(pitch, componentParent->GetEulerAngles().y, componentParent->GetEulerAngles().z));
	}

	mouseButtonPressed0 = false;
	mouseButtonPressed1 = false;
}

void Controller::Move(glm::vec3 direction, float moveSpeed)
{
	if (direction != glm::vec3(0))
	{
		//direction.y = 0;
		componentParent->Translate(glm::normalize(direction) * moveSpeed);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mouseButtonDown1 = true;
		mouseButtonPressed1 = true;
		mouseButtonReleased1 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		mouseButtonReleased1 = true;
		mouseButtonDown1 = false;
		mouseButtonPressed1 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouseButtonDown0 = true;
		mouseButtonPressed0 = true;
		mouseButtonReleased0 = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseButtonReleased0 = true;
		mouseButtonDown0 = false;
		mouseButtonPressed0 = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	
	xoffset = -(lastX - xpos);
	yoffset = (lastY - ypos);
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;	
}
