#include "SSBO.h"
#include <iostream>

SSBO::SSBO()
{
}

SSBO::~SSBO()
{
    for (auto& entry : m_Cache) {
        glDeleteBuffers(1, &entry.second.first);
    }
}

void SSBO::CreateSSBO(const std::string& name, GLuint bindingPoint, GLsizeiptr size)
{
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_Cache[name] = { ssbo, bindingPoint };
}

void SSBO::UpdateSSBO(const std::string& name, GLsizeiptr offset, GLsizeiptr size, const void* data)
{
    auto it = m_Cache.find(name);
    if (it != m_Cache.end())
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, it->second.first);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
    else
    {
        std::cerr << "SSBO with name " << name << " not found for update." << std::endl;
    }
}

void SSBO::BindSSBO(const std::string& name)
{
    auto it = m_Cache.find(name);
    if (it != m_Cache.end())
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, it->second.second, it->second.first);
    }
    else
    {
        std::cerr << "SSBO with name " << name << " not found for binding." << std::endl;
    }
}

GLuint SSBO::GetSSBO(const std::string& name)
{
    auto it = m_Cache.find(name);
    if (it != m_Cache.end())
    {
        return it->second.first;
    }
    else
    {
        std::cerr << "SSBO with name " << name << " not found." << std::endl;
        return 0;
    }
}
