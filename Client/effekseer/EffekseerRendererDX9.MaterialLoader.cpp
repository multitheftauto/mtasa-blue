#include "EffekseerRendererDX9.MaterialLoader.h"
#include <EffekseerRendererDX9/EffekseerRendererDX9.ModelRenderer.h>
#include <EffekseerRendererDX9/EffekseerRendererDX9.Shader.h>
#include <iostream>

#include "../EffekseerMaterialCompiler/DirectX9/EffekseerMaterialCompilerDX9.h"
#include "Effekseer/Material/Effekseer.CompiledMaterial.h"

#undef min

namespace EffekseerRendererDX9
{

const int32_t ModelRendererInstanceCount = 10;

MaterialLoader::MaterialLoader(const Backend::GraphicsDeviceRef graphicsDevice, ::Effekseer::FileInterfaceRef fileInterface)
	: graphicsDevice_(graphicsDevice)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = Effekseer::MakeRefPtr<Effekseer::DefaultFileInterface>();
	}
}

MaterialLoader ::~MaterialLoader()
{
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
	// code file
	{
		auto binaryPath = std::u16string(path) + u"d";
		auto reader = fileInterface_->OpenRead(binaryPath.c_str());

		if (reader != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Compiled);

			if (material != nullptr)
			{
				return material;
			}
		}
	}

	// code file
	{
		auto reader = fileInterface_->OpenRead(path);

		if (reader != nullptr)
		{
			size_t size = reader->GetLength();
			std::vector<char> data;
			data.resize(size);
			reader->Read(data.data(), size);

			auto material = Load(data.data(), (int32_t)size, ::Effekseer::MaterialFileType::Code);

			return material;
		}
	}

	return nullptr;
}

::Effekseer::MaterialRef MaterialLoader::LoadAcutually(::Effekseer::MaterialFile& materialFile, ::Effekseer::CompiledMaterialBinary* binary)
{
	if (binary == nullptr)
	{
		return nullptr;
	}

	auto graphicsDevice = graphicsDevice_;

	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
	material->IsRefractionRequired = materialFile.GetHasRefraction();

	std::array<Effekseer::MaterialShaderType, 2> shaderTypes;
	std::array<Effekseer::MaterialShaderType, 2> shaderTypesModel;

	shaderTypes[0] = Effekseer::MaterialShaderType::Standard;
	shaderTypes[1] = Effekseer::MaterialShaderType::Refraction;
	shaderTypesModel[0] = Effekseer::MaterialShaderType::Model;
	shaderTypesModel[1] = Effekseer::MaterialShaderType::RefractionModel;
	int32_t shaderTypeCount = 1;

	if (materialFile.GetHasRefraction())
	{
		shaderTypeCount = 2;
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		Shader* shader = nullptr;
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, false, st, ModelRendererInstanceCount);

		if (material->IsSimpleVertex)
		{
			auto vl = EffekseerRenderer::GetMaterialSimpleVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

			shader = Shader::Create(graphicsDevice_,
									graphicsDevice->CreateShaderFromBinary(

										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									vl);
		}
		else
		{
			auto vl = EffekseerRenderer::GetMaterialSpriteVertexLayout(graphicsDevice_, static_cast<int32_t>(materialFile.GetCustomData1Count()), static_cast<int32_t>(materialFile.GetCustomData2Count())).DownCast<Backend::VertexLayout>();

			shader = Shader::Create(graphicsDevice_,
									graphicsDevice->CreateShaderFromBinary(

										(uint8_t*)binary->GetVertexShaderData(shaderTypes[st]),
										binary->GetVertexShaderSize(shaderTypes[st]),
										(uint8_t*)binary->GetPixelShaderData(shaderTypes[st]),
										binary->GetPixelShaderSize(shaderTypes[st])),
									vl);
		}

		if (shader == nullptr)
			return false;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);

		shader->SetPixelConstantBufferSize(pixelUniformSize);

		material->TextureCount = materialFile.GetTextureCount();
		material->UniformCount = materialFile.GetUniformCount();

		if (st == 0)
		{
			material->UserPtr = shader;
		}
		else
		{
			material->RefractionUserPtr = shader;
		}
	}

	for (int32_t st = 0; st < shaderTypeCount; st++)
	{
		auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, true, st, ModelRendererInstanceCount);

		// compile
		std::string log;
		auto vl = EffekseerRenderer::GetMaterialModelVertexLayout(graphicsDevice_).DownCast<Backend::VertexLayout>();

		auto shader = Shader::Create(graphicsDevice_,
									 graphicsDevice->CreateShaderFromBinary(

										 (uint8_t*)binary->GetVertexShaderData(shaderTypesModel[st]),
										 binary->GetVertexShaderSize(shaderTypesModel[st]),
										 (uint8_t*)binary->GetPixelShaderData(shaderTypesModel[st]),
										 binary->GetPixelShaderSize(shaderTypesModel[st])),
									 vl);
		if (shader == nullptr)
			return false;

		auto vertexUniformSize = parameterGenerator.VertexShaderUniformBufferSize;
		auto pixelUniformSize = parameterGenerator.PixelShaderUniformBufferSize;

		shader->SetVertexConstantBufferSize(vertexUniformSize);

		shader->SetPixelConstantBufferSize(pixelUniformSize);

		if (st == 0)
		{
			material->ModelUserPtr = shader;
		}
		else
		{
			material->RefractionModelUserPtr = shader;
		}
	}

	material->CustomData1 = materialFile.GetCustomData1Count();
	material->CustomData2 = materialFile.GetCustomData2Count();
	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
	material->UniformCount = materialFile.GetUniformCount();
	material->ShadingModel = materialFile.GetShadingModel();

	for (int32_t i = 0; i < material->TextureCount; i++)
	{
		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
	}

	return material;
}

::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
{
	if (fileType == Effekseer::MaterialFileType::Compiled)
	{
		auto compiled = Effekseer::CompiledMaterial();
		if (!compiled.Load(static_cast<const uint8_t*>(data), size))
		{
			return nullptr;
		}

		if (!compiled.GetHasValue(::Effekseer::CompiledMaterialPlatformType::DirectX9))
		{
			return nullptr;
		}

		// compiled
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)compiled.GetOriginalData().data(), static_cast<int32_t>(compiled.GetOriginalData().size())))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto binary = compiled.GetBinary(::Effekseer::CompiledMaterialPlatformType::DirectX9);

		return LoadAcutually(materialFile, binary);
	}
	else
	{
		Effekseer::MaterialFile materialFile;
		if (!materialFile.Load((const uint8_t*)data, size))
		{
			std::cout << "Error : Invalid material is loaded." << std::endl;
			return nullptr;
		}

		auto compiler = ::Effekseer::CreateUniqueReference(new Effekseer::MaterialCompilerDX9());
		auto binary = ::Effekseer::CreateUniqueReference(compiler->Compile(&materialFile));

		return LoadAcutually(materialFile, binary.get());
	}
}

void MaterialLoader::Unload(::Effekseer::MaterialRef data)
{
	if (data == nullptr)
		return;
	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(refractionShader);
	ES_SAFE_DELETE(refractionModelShader);

	data->UserPtr = nullptr;
	data->ModelUserPtr = nullptr;
	data->RefractionUserPtr = nullptr;
	data->RefractionModelUserPtr = nullptr;
}

} // namespace EffekseerRendererDX9
