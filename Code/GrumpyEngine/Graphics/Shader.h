#pragma once

struct ShaderDesc
{
    std::string myVSCode;
    std::string myFSCode;
    std::string myVSLocation;
    std::string myFSLocation;
    std::vector<std::string> myDefines;
};

struct Shader
{
    static SPtr<Shader> Create(const ShaderDesc& desc);
    static SPtr<Shader> Create(const std::string& vsLocation, const std::string& fsLocation);

    bool Compile(const ShaderDesc& desc);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void BindInt(const char* location, int value) const;
    void BindBool(const char* location, bool value) const;
    void BindFloat(const char* location, float value) const;
    void BindVector2f(const char* location, Vec2 value) const;
    void BindVector3f(const char* location, Vec3 value) const;
    void BindVector4f(const char* location, Vec4 value) const;
    void BindMatrix2x2(const char* location, Mat2 value) const;
    void BindMatrix3x3(const char* location, Mat3 value) const;
    void BindMatrix4x4(const char* location, Mat4 value) const;

    std::string GetShaderCode(std::string code, const std::string& filename) const;
    std::string GetIncludeCode(const std::string& line) const;

    GLuint myProgram = 0;
};