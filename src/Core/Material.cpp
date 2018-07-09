#include "Material.hpp"

#include <assert.h>

#include <Core/Texture.hpp>
#include <Core/Serialize.hpp>
#include <Core/SerialUtil.hpp>
#include <Core/AssetRegistry.hpp>

namespace Phoenix
{
	TextureCreationHints* getDiffuseHints()
	{
		static TextureCreationHints hints;
		hints.colorSpace = ETextrueColorSpace::SRGB;
		hints.minFilter = ETextureFilter::Linear;
		hints.magFilter = ETextureFilter::Linear;
		hints.wrapU = ETextureWrap::Repeat;
		hints.wrapV = ETextureWrap::Repeat;
		hints.wrapW = ETextureWrap::Repeat;
		hints.bGenMipMaps = true;
		hints.mipFilter = ETextureFilter::Linear;
		return &hints;
	}

	TextureCreationHints* getNonDiffuseHints()
	{
		static TextureCreationHints hints;
		hints.colorSpace = ETextrueColorSpace::Linear;
		hints.wrapU = ETextureWrap::Repeat;
		hints.wrapV = ETextureWrap::Repeat;
		hints.wrapW = ETextureWrap::Repeat;
		hints.bGenMipMaps = true;
		hints.mipFilter = ETextureFilter::Linear;
		return &hints;
	}

	TextureCreationHints* getRoughnessHints()
	{
		return getNonDiffuseHints();
	}

	TextureCreationHints* getMetallicHints()
	{
		return getNonDiffuseHints();
	}

	TextureCreationHints* getNormalsHints()
	{
		return getNonDiffuseHints();
	}

	void addDefaultMaterial(AssetRegistry* assets)
	{
		Material* material = assets->getMaterial(g_defaultMaterialPath);

		if (material)
		{
			return;
		}

		Texture2D* defaultTexWhite = assets->getTexture(g_defaultWhiteTexName);
		Texture2D* defaultTexBlack = assets->getTexture(g_defaultBlackTexName);

		const char* materialName = "defaultMaterial";
		
		material = assets->allocMaterial(materialName);

		material->m_diffuseTex = defaultTexWhite;
		material->m_roughnessTex = defaultTexWhite;
		material->m_metallicTex = defaultTexBlack;
		material->m_normalTex = defaultTexBlack;

		material->m_name = materialName;
	}

	struct MaterialResources
	{
		MaterialResources() = default;

		MaterialResources(const Material& material)
			: m_name(material.m_name)
			, m_diffuseTexPath(material.m_diffuseTex->m_name)
			, m_roughnessTexPath(material.m_roughnessTex->m_name)
			, m_metallicTexPath(material.m_metallicTex->m_name)
			, m_normalTexPath(material.m_normalTex->m_name)
		{}

		std::string m_name;
		std::string m_diffuseTexPath;
		std::string m_roughnessTexPath;
		std::string m_metallicTexPath;
		std::string m_normalTexPath;
	};

	void serialize(Archive& ar, MaterialResources& exp)
	{
		serialize(ar, exp.m_name);
		serialize(ar, exp.m_diffuseTexPath);
		serialize(ar, exp.m_roughnessTexPath);
		serialize(ar, exp.m_metallicTexPath);
		serialize(ar, exp.m_normalTexPath);
	}

	static const char* g_assetFileExt = ".mat";

	void saveMaterial(Material& material, AssetRegistry* assets)
	{
		WriteArchive ar;
		createWriteArchive(sizeof(Material), &ar);

		MaterialResources exp(material);
		serialize(ar, exp);

		std::string writePath = assets->getAssetsPath() + material.m_name;
		writePath += g_assetFileExt;

		EArchiveError err = writeArchiveToDisk(writePath.c_str(), ar);
		assert(err == EArchiveError::NoError);
		destroyArchive(ar);
	}

	Material* loadMaterial(const char* path, IRIDevice* renderDevice, IRIContext* renderContext, AssetRegistry* assets)
	{
		Material* mat = assets->getMaterial(path);

		if (mat)
		{
			return mat;
		}

		std::string readPath = path;
		readPath = assets->getAssetsPath() + readPath + g_assetFileExt;

		ReadArchive ar;
		EArchiveError err = createReadArchive(readPath.c_str(), &ar);

		assert(err == EArchiveError::NoError);
		if (err != EArchiveError::NoError)
		{
			return mat;
		}

		MaterialResources exp;
		serialize(ar, exp);

		mat = assets->allocMaterial(path);
		mat->m_name = exp.m_name;

		mat->m_diffuseTex =	  loadTexture(exp.m_diffuseTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_roughnessTex = loadTexture(exp.m_roughnessTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_metallicTex =  loadTexture(exp.m_metallicTexPath.c_str(), renderDevice, renderContext, assets);
		mat->m_normalTex =	  loadTexture(exp.m_normalTexPath.c_str(), renderDevice, renderContext, assets);

		return mat;
	}
}