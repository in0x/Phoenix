#pragma once

#include <string>

namespace Phoenix
{
	static const char* g_defaultMaterialPath = "defaultMaterial";

	struct Texture2D;
	class AssetRegistry;
	struct TextureCreationHints;

	TextureCreationHints* getDiffuseHints();
	TextureCreationHints* getNonDiffuseHints();
	TextureCreationHints* getRoughnessHints();
	TextureCreationHints* getMetallicHints();
	TextureCreationHints* getNormalsHints();

	struct Material
	{
		std::string m_name;

		Texture2D* m_diffuseTex;
		Texture2D* m_roughnessTex;
		Texture2D* m_metallicTex;
		Texture2D* m_normalTex;
	};

	void addDefaultMaterial(AssetRegistry* assets);

	Material* loadMaterial(const char* path, class IRIDevice* renderDevice, class IRIContext* renderContext, AssetRegistry* assets);

	void saveMaterial(Material& material, const char* path);
}