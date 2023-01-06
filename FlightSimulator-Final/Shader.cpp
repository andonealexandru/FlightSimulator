#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

ShaderSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream fin(filepath);

    enum class ShaderType
    {
        NONE = -1,
        vertex = 0,
        fragment = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(fin, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::vertex;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::fragment;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str() };
}
unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader";
        std::cout << message << '\n';
        glDeleteShader(id);
        return 0;
    }
    return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void Shader::Set(std::string SourceFilePath)
{
    ShaderSource source = ParseShader(SourceFilePath);
    shaderIndex = CreateShader(source.VertexSource, source.FragmentSource);
}

void Shader::Use()
{
    glUseProgram(shaderIndex);
}

void Shader::Delete()
{
    glDeleteProgram(shaderIndex);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderIndex, name.c_str()), value);
}
void Shader::SetFloat(const std::string& name, const float& value) const
{
    glUniform1f(glGetUniformLocation(shaderIndex, name.c_str()), value);
}
void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(shaderIndex, name.c_str()), 1, &value[0]);
}
void Shader::SetVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(shaderIndex, name.c_str()), x, y, z);
}
void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderIndex, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}