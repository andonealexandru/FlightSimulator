#pragma once
#include <string>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm.hpp>

struct ShaderSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
protected:
	ShaderSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
public:
	ShaderSource Source;
	unsigned int shaderIndex;
	void Set(std::string SourceFilePath);
	void Use();
	void Delete();
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	void SetInt(const std::string& name, int value) const;
	void SetFloat(const std::string& name, const float& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetVec3(const std::string& name, float x, float y, float z) const;
};
