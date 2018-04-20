#pragma once

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;
	
	struct StaticMesh;
	struct Material;

	class DeferredRenderer
	{
	public:
		DeferredRenderer(IRIDevice* renderDevice, IRIContext* renderContext, uint32_t gBufferWidth, uint32_t gBufferHeight);

		void setViewMatrix(const Matrix4& view);

		void setProjectionMatrix(const Matrix4& projection);

		// Sets up the state needed to draw values into the GBuffer. Needs to be called before e.g. first drawStaticMesh() call.
		void setupGBufferPass();

		// Draws the material values needed for shading from this StaticMesh into the GBuffer.
		void drawStaticMesh(const StaticMesh& mesh, const Matrix4& transform);

		// Sets up the state needed to evaluate the shading equation. Needs to be called after filling the Gbuffer and before submitting
		// the first light.
		void setupLightPass();

		// Evaluates the shading equation for this light using the values written into the GBuffer.
		void drawLight(Vec3 direction, Vec3 color);


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

		IRIDevice* m_device;
		IRIContext* m_context;
	};
}