
#pragma once

#include <Effekseer/Material/Effekseer.MaterialCompiler.h>
#include <vector>

namespace Effekseer
{

class MaterialCompilerDX9 : public MaterialCompiler, public ReferenceObject
{
private:
public:
	MaterialCompilerDX9() = default;

	virtual ~MaterialCompilerDX9() = default;

	CompiledMaterialBinary* Compile(MaterialFile* materialFile, int32_t maximumUniformCount, int32_t maximumTextureCount);

	CompiledMaterialBinary* Compile(MaterialFile* materialFile) override;

	int AddRef() override
	{
		return ReferenceObject::AddRef();
	}

	int Release() override
	{
		return ReferenceObject::Release();
	}

	int GetRef() override
	{
		return ReferenceObject::GetRef();
	}
};

} // namespace Effekseer
