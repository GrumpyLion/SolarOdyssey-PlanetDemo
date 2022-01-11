#include "pch.h"

#include <fstream>
#include "Shader.h"

SPtr<Shader> Shader::Create(const ShaderDesc& desc)
{
    auto shader = MakeShared<Shader>();
    if (!shader->Compile(desc))
        return nullptr;

    return shader;
}

SPtr<Shader> Shader::Create(const std::string& vsLocation, const std::string& fsLocation)
{
    ShaderDesc desc{};
    std::ifstream vsFile(vsLocation);
    if (!vsFile.is_open())
    {
        GE_PRINT("Shader file not found %s", vsLocation.c_str());
        return nullptr;
    }

    desc.myVSCode = std::string((std::istreambuf_iterator<char>(vsFile)),
                                 std::istreambuf_iterator<char>());

    std::ifstream fsFile(fsLocation);
    if (!fsFile.is_open())
    {
        GE_PRINT("Shader file not found %s", fsLocation.c_str());
        return nullptr;
    }

    desc.myFSCode = std::string((std::istreambuf_iterator<char>(fsFile)),
                                 std::istreambuf_iterator<char>());

    desc.myVSLocation = vsLocation;
    desc.myFSLocation = fsLocation;

    auto shader = MakeShared<Shader>();
    if (!shader->Compile(desc))
        return nullptr;

    return shader;
}

bool Shader::Compile(const ShaderDesc& desc)
{
    std::string vsCode = GetShaderCode(desc.myVSCode, desc.myVSLocation);
    std::string fsCode = GetShaderCode(desc.myFSCode, desc.myFSLocation);

    uint vs = glCreateShader(GL_VERTEX_SHADER);
    uint fs = glCreateShader(GL_FRAGMENT_SHADER);
    myProgram = glCreateProgram();
    int status;
    char log[1024];

    if (desc.myDefines.size() > 0)
    {
        std::vector<std::string> vsMergedCode;
        std::vector<std::string> fsMergedCode;
        
        std::string firstLine = vsCode.substr(0, vsCode.find("\n"));
        if (firstLine.find("#version") != std::string::npos)
        {
            vsCode = vsCode.substr(vsCode.find("\n") + 1, vsCode.length() - 1);
            vsMergedCode.push_back(firstLine + "\n");
        }
        
        firstLine = fsCode.substr(0, fsCode.find("\n"));
        if (firstLine.find("#version") != std::string::npos)
        {
            fsCode = fsCode.substr(fsCode.find("\n") + 1, fsCode.length() - 1);
            fsMergedCode.push_back(firstLine + "\n");
        }

        for (uint i = 0; i < desc.myDefines.size(); ++i)
        {
            std::string define = "#define " + desc.myDefines[i] + "\n";
            vsMergedCode.push_back(define);
            fsMergedCode.push_back(define);
        }

        vsMergedCode.push_back(vsCode);
        fsMergedCode.push_back(fsCode);
        
        const char** vsStringsC = new const char* [vsMergedCode.size()];
        const char** fsStringsC = new const char* [fsMergedCode.size()];
        
        for (uint i = 0; i < vsMergedCode.size(); ++i)
            vsStringsC[i] = vsMergedCode[i].c_str();
        for (uint i = 0; i < fsMergedCode.size(); ++i)
            fsStringsC[i] = fsMergedCode[i].c_str();
        
        glShaderSource(vs, static_cast<GLsizei>(vsMergedCode.size()), vsStringsC, NULL);
        glShaderSource(fs, static_cast<GLsizei>(fsMergedCode.size()), fsStringsC, NULL);

        delete[] vsStringsC;
        delete[] fsStringsC;
    }
    else
    {
        const char* vsSourceC = vsCode.c_str();
        const char* fsSourceC = fsCode.c_str();
        glShaderSource(vs, 1, &vsSourceC, NULL);
        glShaderSource(fs, 1, &fsSourceC, NULL);
    }

    glCompileShader(vs);
    glCompileShader(fs);

    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(vs, 1024, NULL, log);
        GE_PRINT("Vertex shader compilation error at: %s!\n%s", desc.myVSLocation.c_str(), std::string(log).c_str());
    }
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        glGetShaderInfoLog(fs, 1024, NULL, log);
        GE_PRINT("Fragment shader compilation error at: %s!\n%s", desc.myFSLocation.c_str(), std::string(log).c_str());
    }

    glAttachShader(myProgram, vs);
    glAttachShader(myProgram, fs);
    glLinkProgram(myProgram);

    glGetProgramiv(myProgram, GL_LINK_STATUS, &status);
    if (!status)
    {
        glGetProgramInfoLog(myProgram, 1024, NULL, log);
        GE_PRINT("Shader program linking error: %s", log);
        GE_PRINT(vsCode.c_str());
        GE_PRINT(fsCode.c_str());
        GE_ASSERT(false);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return status;
}

Shader::~Shader()
{
    glDeleteProgram(myProgram);
}

void Shader::Bind() const
{
    glUseProgram(myProgram);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::BindInt(const char* location, int value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform1i(loc, value);
}

void Shader::BindBool(const char* location, bool value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform1i(loc, (int)value);
}

void Shader::BindFloat(const char* location, float value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform1f(loc, value);
}

void Shader::BindVector2f(const char* location, Vec2 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform2f(loc, value.x, value.y);
}

void Shader::BindVector3f(const char* location, Vec3 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform3f(loc, value.x, value.y, value.z);
}

void Shader::BindVector4f(const char* location, Vec4 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void Shader::BindMatrix2x2(const char* location, Mat2 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniformMatrix2fv(loc, 1, GL_FALSE, (const GLfloat*)&value);
}

void Shader::BindMatrix3x3(const char* location, Mat3 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniformMatrix3fv(loc, 1, GL_FALSE, (const GLfloat*)&value);
}

void Shader::BindMatrix4x4(const char* location, Mat4 value) const
{
    int loc = glGetUniformLocation(myProgram, location);
    if (loc >= 0)
        glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)&value);
}

std::string Shader::GetShaderCode(std::string code, const std::string& filename) const
{
    uint lastPos = 0;
    for (;;)
    {
        uint newPos = (uint)code.find('\n', lastPos + 1);
        std::string line = code.substr(lastPos, newPos - lastPos);
        if (lastPos > code.size() || newPos > code.size())
            break;

        if (line.find("#include ") != -1)
        {
            std::string includeCode = GetIncludeCode(line);
            if (includeCode.empty())
                continue;

            includeCode.append("\n");
            code.erase(lastPos, newPos - lastPos);
            code.insert(lastPos, includeCode);
            continue;
        }
        lastPos = newPos;
    }

    return code;
}

std::string Shader::GetIncludeCode(const std::string& line) const
{
    uint beginning = 0;
    uint ending = 0;
    beginning = (uint)line.find('\"');
    ending = (uint)line.find('\"', beginning + 1);
    std::string filename;
    filename.append(line.substr(beginning + 1, ending - beginning - 1));

    std::fstream file(filename);

    GE_ASSERT(file.is_open());

    std::string shader((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return shader;
}