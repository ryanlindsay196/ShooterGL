#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Texture.h"
#include "ManagerClasses/TextureManager.h"
//#include "ManagerClasses/ObjectManager.h"
#include "ManagerClasses/LightManager.h"
#include <gtc/type_ptr.hpp>
#include "FileReader.h"

//TODO: potentially remove pragma warning disable : 4996
#pragma warning (disable : 4996)

void Shader::Initialize(std::string& vertexPath, std::string& fragmentPath, std::string& materialPath)
{
	textureManager = TextureManager::GetInstance();
	shaderProgram = glCreateProgram();

	if (materialPath != "")
		LoadMaterial(materialPath);
	else
	{
		SetVertexShader(vertexPath);
		SetFragmentShader(fragmentPath);
	}

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;

	}
	glUseProgram(shaderProgram);

	//GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	glGetProgramiv(shaderProgram, GL_ACTIVE_ATTRIBUTES, &count);
	//printf("Active Attributes: %d\n", count);
	//
	//for (i = 0; i < count; i++)
	//{
	//	glGetActiveAttrib(shaderProgram, (GLuint)i, bufSize, &length, &size, &type, name);
	//
	//	printf("Attribute #%d Type: %u Name: %s\n", i, type, name);
	//}
	//
	//glGetProgramiv(shaderProgram, GL_ACTIVE_UNIFORMS, &count);
	//printf("Active Uniforms: %d\n", count);
	//
	//for (i = 0; i < count; i++)
	//{
	//	glGetActiveUniform(shaderProgram, (GLuint)i, bufSize, &length, &size, &type, name);
	//
	//	printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
	//}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::LoadMaterial(std::string& materialPath)
{
	std::ifstream materialFile(materialPath);

	std::string line;
	std::string currentShader = "";

	while (getline(materialFile, line))
	{
		line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
		if (line.substr(0, 2) == "&&")
		{
			currentShader = (line.erase(0, 2));
			std::string tempTok[2];

			tempTok[0] = line;
			tempTok[0] = strtok((char*)tempTok[0].c_str(), " ");
			tempTok[1] = line;
			tempTok[1] = tempTok[1].substr(tempTok[1].find_first_of(" ") + 1);
			if (tempTok[0] == "VertexShader")
				SetVertexShader(tempTok[1]);
			else if (tempTok[0] == "FragmentShader")
				SetFragmentShader(tempTok[1]);
		}
		else if (line == "<ATTACH AND LINK SHADER>")
		{
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
		}
		else if (line == "textures")
		{
			while (getline(materialFile, line) && line != "}")
			{
				//Remove tabs
				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				//Skip "{"
				//Remove spaces
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				if (line == "{")
					continue;
				else if (line == "}")
					break;

				//Get the texture path
				std::string newTexturePath = line.substr(line.find_first_of(":") + 1);
				std::string newTextureUniform = strtok((char*)line.c_str(), ":");
				AddNewTexture(newTexturePath, newTextureUniform);
			}
		}
		else if (line == "vec3")
		{
			while (getline(materialFile, line) && line != "}")
			{

				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				//Skip "{"
				if (line == "{")
					continue;
				else if (line == "}")
					break;
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");

				glm::vec3 newVector = ParseVector(keyValuePair.second);
				SetShaderUniform_vec3((char*)keyValuePair.first.c_str(), newVector);
			}
		}
		else if (line == "float")
		{
			while (getline(materialFile, line) && line != "}")
			{

				line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());
				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				//Skip "{"
				if (line == "{")
					continue;
				else if (line == "}")
					break;
				std::pair<std::string, std::string> keyValuePair = GenerateKeyValuePair(line, ":");

				SetShaderUniform_vec1((char*)keyValuePair.first.c_str(), strtof(keyValuePair.second.c_str(), nullptr));
			}
		}
	}
	materialFile.close();
}

void Shader::AddNewTexture(std::string& texturePath, std::string& textureUniform)
{
	textures.push_back(textureManager->LoadNewTexture(texturePath));
	textureUniforms.push_back(textureUniform);
}

void Shader::ClearTextures()
{
	textures.clear();
}

void Shader::BindTextures()
{
	//TODO: Get uniform names from material, make hash map using these names and the texture ID
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		textures[i]->UseTexture(i);
		SetShaderUniform_veci1((char*)textureUniforms[i].c_str(), textures[i]->GetTextureID());
	}
}

void Shader::SetVertexShader(std::string& vertexPath)
{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	std::ifstream vShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		std::stringstream vShaderStream;
		vShaderStream << vShaderFile.rdbuf();

		std::string vString = vShaderStream.str();
		vertexShaderSource = (char*)vString.c_str();
		//std::cout << "Vertex shader successfully read" << std::endl;

		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::VERTEX_SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//else
		//	std::cout << "Shader compiled successfully" << std::endl;

		vShaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR:VERTEX_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
}

void Shader::SetFragmentShader(std::string& fragmentPath)
{

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::ifstream fShaderFile;
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fShaderFile.open(fragmentPath);
		std::stringstream fShaderStream;
		fShaderStream << fShaderFile.rdbuf();

		std::string tempFShader = "";
		std::string line;
		while (getline(fShaderStream, line))
		{
			if (line == "#define NR_POINT_LIGHTS X")
			{
 				line = "#define NR_POINT_LIGHTS " + std::to_string(LightManager::GetInstance()->TotalLights());
			}
			tempFShader += line + "\n";
		}

		std::string fString = fShaderStream.str();
		std::string lightsString = "#define NR_POINT_LIGHTS " + std::to_string(LightManager::GetInstance()->TotalLights());
		fString.replace(fString.begin() + fString.find_first_of("#"), fString.begin() + fString.find_first_of("#") + 25, lightsString.c_str());
		//fString.replace(fString.begin() + fString.find_first_of("#define"), 25, "#define NR_POINT_LIGHTS " + std::to_string(m_objectManager->lightManager->TotalLights()));
		//fString.replace(fString.begin(), 5, "#define NR_POINT_LIGHTS " + std::to_string(m_objectManager->lightManager->TotalLights());
		fragmentShaderSource = (char*)tempFShader.c_str();
		//std::cout << fragmentShaderSource;
		//std::cout << "Fragment shader succesfully read" << std::endl;

		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		int success;
		char infoLog[512];
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::FRAGMENT_SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		//else
		//	std::cout << "Shader compiled successfully" << std::endl;

		fShaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR:FRAGMENT_SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
}

std::vector<std::shared_ptr<Texture>> Shader::GetTextures()
{
	return textures;
}

unsigned int Shader::GetShaderProgram()
{
	return shaderProgram;
}

void Shader::UseShader()
{
	glUseProgram(shaderProgram);
}

void Shader::SetShaderUniform_vec1(const char * uniformName, float uniformValue)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1f(uniformLocation, (GLfloat)uniformValue);
}

void Shader::SetShaderUniform_vec2(const char * uniformName, float x, float y)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2f(uniformLocation, (GLfloat)x, (GLfloat)y);
}

void Shader::SetShaderUniform_vec3(const char * uniformName, float x, float y, float z)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3f(uniformLocation, (GLfloat)x, (GLfloat)y, (GLfloat)z);
}

void Shader::SetShaderUniform_vec3(const char * uniformName, glm::vec3 & value)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3fv(uniformLocation, 1, &value[0]);
}

void Shader::SetShaderUniform_vec4(const char * uniformName, float x, float y, float z, float w)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4f(uniformLocation, (GLfloat)x, (GLfloat)y, (GLfloat)z, (GLfloat)w);
}

void Shader::SetShaderUniform_veci1(const char * uniformName, int uniformValue)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform1i(uniformLocation, uniformValue);
}

void Shader::SetShaderUniform_veci2(const char * uniformName, int x, int y)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform2i(uniformLocation, x, y);
}

void Shader::SetShaderUniform_veci3(const char * uniformName, int x, int y, int z)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform3i(uniformLocation, x, y, z);
}

void Shader::SetShaderUniform_veci4(const char * uniformName, int x, int y, int z, int w)
{
	glUseProgram(shaderProgram);
	int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniform4i(uniformLocation, x, y, z, w);
}

void Shader::SetShaderUniform_mat4fv(const char * uniformName, glm::mat4 matrix, GLboolean toTranspose)
{
	glUseProgram(shaderProgram);
	unsigned int uniformLocation = glGetUniformLocation(shaderProgram, uniformName);
	glUniformMatrix4fv(uniformLocation, 1, toTranspose, glm::value_ptr(matrix));
}
