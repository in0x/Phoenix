#pragma once

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;
	class WorldObject;

	// Placeholder.
	class DirectionalLight
	{
	public:
		Vec3 m_direction;
		Vec3 m_color;
	};

	class DeferredRenderer
	{
	public:
		DeferredRenderer(IRIDevice* renderDevice, uint32_t gBufferWidth, uint32_t gBufferHeight);

		void setupGBufferPass(IRIContext* context);

		void fillGBuffer(const WorldObject& entity, IRIContext* context);

		void setupLightPass(IRIContext* context);

		void drawLight(const DirectionalLight& light, IRIContext* context);

		void setViewMatrix(const Matrix4& view);

		void setProjectionMatrix(const Matrix4& projection);

	private:
		Matrix4 m_viewMat;
		Matrix4 m_projMat;

		RenderTargetHandle m_gBuffer;
		
		Texture2DHandle m_kDiffuseDepthTex;
		Texture2DHandle m_kSpecularTex;
		Texture2DHandle m_normalSpecExpTex;
		Texture2DHandle m_depthTex;
		
		// NOTE(Phil): I want to replace these groups of uniforms with constant buffers eventually.

		struct Uniforms
		{
			UniformHandle modelTf;
			UniformHandle viewTf;
			UniformHandle projTf;

			UniformHandle kDiffuse;
			UniformHandle kSpecular;
			UniformHandle specExp;

			UniformHandle lightDirEye;
			UniformHandle lightColor;
		} m_uniforms;

		ProgramHandle m_gBufferProgram;
		ProgramHandle m_directionaLightProgram;

		BlendState m_lightBlendState;
	};
}