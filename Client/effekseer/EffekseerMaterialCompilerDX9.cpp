
#include "StdInc.h"

#include "EffekseerMaterialCompilerDX9.h"
//#include "HLSLGenerator/ShaderGenerator.h"
#include <effekseer/effekseer-submodule/Dev/Cpp/EffekseerMaterialCompiler/HLSLGenerator/ShaderGenerator.h>

#include <d3d11.h>
#include <d3d9.h>
#include <d3dcompiler.h>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")

namespace Effekseer
{
    namespace DX9
    {

        static ID3DBlob* CompileVertexShader(const char* vertexShaderText, const char* vertexShaderFileName, const std::vector<D3D_SHADER_MACRO>& macro,
                                             std::string& log)
        {
            ID3DBlob* shader = nullptr;
            ID3DBlob* error = nullptr;

            UINT flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
            flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

            HRESULT hr;

            hr = D3DCompile(vertexShaderText, strlen(vertexShaderText), vertexShaderFileName, macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : nullptr,
                            nullptr, "main", "vs_3_0", flag, 0, &shader, &error);

            if (FAILED(hr))
            {
                if (hr == E_OUTOFMEMORY)
                {
                    log += "Out of memory\n";
                }
                else
                {
                    log += "Unknown error\n";
                }

                if (error != nullptr)
                {
                    log += (const char*)error->GetBufferPointer();
                    error->Release();
                }

                ES_SAFE_RELEASE(shader);
                return nullptr;
            }

            return shader;
        }

        static ID3DBlob* CompilePixelShader(const char* vertexShaderText, const char* vertexShaderFileName, const std::vector<D3D_SHADER_MACRO>& macro,
                                            std::string& log)
        {
            ID3DBlob* shader = nullptr;
            ID3DBlob* error = nullptr;
            UINT      flag = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR;
#if !_DEBUG
            flag = flag | D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

            HRESULT hr;

            hr = D3DCompile(vertexShaderText, strlen(vertexShaderText), vertexShaderFileName, macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : nullptr,
                            nullptr, "main", "ps_3_0", flag, 0, &shader, &error);

            if (FAILED(hr))
            {
                if (hr == E_OUTOFMEMORY)
                {
                    log += "Out of memory\n";
                }
                else
                {
                    log += "Unknown error\n";
                }

                if (error != nullptr)
                {
                    log += (const char*)error->GetBufferPointer();
                    error->Release();
                }

                ES_SAFE_RELEASE(shader);
                return nullptr;
            }

            return shader;
        }

    }            // namespace DX9

}            // namespace Effekseer

namespace Effekseer
{

    const int32_t DX9_ModelRendererInstanceCount = 10;

    class CompiledMaterialBinaryDX9 : public CompiledMaterialBinary, public ReferenceObject
    {
    private:
        std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> vertexShaders_;

        std::array<std::vector<uint8_t>, static_cast<int32_t>(MaterialShaderType::Max)> pixelShaders_;

    public:
        CompiledMaterialBinaryDX9() {}

        virtual ~CompiledMaterialBinaryDX9() {}

        void SetVertexShaderData(MaterialShaderType type, const std::vector<uint8_t>& data) { vertexShaders_.at(static_cast<int>(type)) = data; }

        void SetPixelShaderData(MaterialShaderType type, const std::vector<uint8_t>& data) { pixelShaders_.at(static_cast<int>(type)) = data; }

        const uint8_t* GetVertexShaderData(MaterialShaderType type) const override { return vertexShaders_.at(static_cast<int>(type)).data(); }

        int32_t GetVertexShaderSize(MaterialShaderType type) const override { return static_cast<int32_t>(vertexShaders_.at(static_cast<int>(type)).size()); }

        const uint8_t* GetPixelShaderData(MaterialShaderType type) const override { return pixelShaders_.at(static_cast<int>(type)).data(); }

        int32_t GetPixelShaderSize(MaterialShaderType type) const override { return static_cast<int32_t>(pixelShaders_.at(static_cast<int>(type)).size()); }

        int AddRef() override { return ReferenceObject::AddRef(); }

        int Release() override { return ReferenceObject::Release(); }

        int GetRef() override { return ReferenceObject::GetRef(); }
    };

    CompiledMaterialBinary* MaterialCompilerDX9::Compile(MaterialFile* materialFile, int32_t maximumUniformCount, int32_t maximumTextureCount)
    {
        auto binary = new CompiledMaterialBinaryDX9();

        auto convertToVectorVS = [](const std::string& str) -> std::vector<uint8_t>
        {
            std::vector<uint8_t> ret;

            std::string                   log;
            std::vector<D3D_SHADER_MACRO> macros;

            auto blob = DX9::CompileVertexShader(str.c_str(), "VS", macros, log);

            if (blob != nullptr)
            {
                ret.resize(blob->GetBufferSize());
                memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
                blob->Release();
            }
            else
            {
                std::cout << "VertexShader Compile Error" << std::endl;
                std::cout << log << std::endl;
                std::cout << str << std::endl;
            }

            return ret;
        };

        auto convertToVectorPS = [](const std::string& str) -> std::vector<uint8_t>
        {
            std::vector<uint8_t> ret;

            std::string                   log;
            std::vector<D3D_SHADER_MACRO> macros;

            auto blob = DX9::CompilePixelShader(str.c_str(), "PS", macros, log);

            if (blob != nullptr)
            {
                ret.resize(blob->GetBufferSize());
                memcpy(ret.data(), blob->GetBufferPointer(), ret.size());
                blob->Release();
            }
            else
            {
                std::cout << "PixelShader Compile Error" << std::endl;
                std::cout << log << std::endl;
                std::cout << str << std::endl;
            }

            return ret;
        };

        auto saveBinary = [&materialFile, &binary, &convertToVectorVS, &convertToVectorPS, &maximumUniformCount, &maximumTextureCount](MaterialShaderType type)
        {
            auto generator = DirectX::ShaderGenerator(DirectX::ShaderGeneratorTarget::DirectX9);

            auto shader = generator.GenerateShader(materialFile, type, maximumUniformCount, maximumTextureCount, 0, DX9_ModelRendererInstanceCount);

            binary->SetVertexShaderData(type, convertToVectorVS(shader.CodeVS));
            binary->SetPixelShaderData(type, convertToVectorPS(shader.CodePS));
        };

        if (materialFile->GetHasRefraction())
        {
            saveBinary(MaterialShaderType::Refraction);
            saveBinary(MaterialShaderType::RefractionModel);
        }

        saveBinary(MaterialShaderType::Standard);
        saveBinary(MaterialShaderType::Model);

        return binary;
    }

    CompiledMaterialBinary* MaterialCompilerDX9::Compile(MaterialFile* materialFile)
    {
        return Compile(materialFile, Effekseer::UserUniformSlotMax, Effekseer::UserTextureSlotMax);
    }

}            // namespace Effekseer

