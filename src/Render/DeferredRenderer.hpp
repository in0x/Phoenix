#pragma once

#include <Render/RIDefs.hpp>
#include <Render/RIResourceHandles.hpp>

#include <Math/Matrix4.hpp>
#include <Math/Vec3.hpp>

namespace Phoenix
{
	class IRIDevice;
	class IRIContext;
	class LightBuffer;

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

		void setupAmbientLightPass();

		void drawAmbientLight();

		void setupDirectionalLightPass();

		void runLightsPass(const LightBuffer& lightBuffer);

		// Applies gamma correction to the final color values and copies them into the default framebuffer. 
		void copyFinalColorToBackBuffer();

	private:
		Matrix4 m_viewMat;
		Matrix4 m_projMat;

		RenderTargetHandle m_gBuffer;
		Texture2DHandle m_kDiffuseDepthTex;
		Texture2DHandle m_MetallicTex;
		Texture2DHandle m_normalRoughnessTex;
		Texture2DHandle m_depthTex;

		RenderTargetHandle m_backBuffer;
		Texture2DHandle m_kFinalTex;

		struct Uniforms
		{
			UniformHandle modelTf;
			UniformHandle viewTf;
			UniformHandle projTf;
			UniformHandle modelViewTf;
			UniformHandle normalTf;

			UniformHandle diffuseDepthTexSampler;
			UniformHandle metallicTexSampler;
			UniformHandle normalRoughnessSampler;
			UniformHandle depthTexSampler;

			UniformHandle finalTexSampler;

			UniformHandle matDiffuseSampler;
			UniformHandle matRoughnessSampler;
			UniformHandle matMetallicSampler;
			UniformHandle matNormalSampler;
		} m_uniforms;

		ProgramHandle m_gBufferProgram;
		ProgramHandle m_ambientLightProgram;
		ProgramHandle m_directionaLightProgram;
		ProgramHandle m_copyToBackBufferProgram;

		ConstantBufferHandle m_cbLights;

		BlendState m_lightBlendState;

		IRIDevice* m_device;
		IRIContext* m_context;

		void drawStaticMeshWithMaterial(VertexBufferHandle vb, const Material& material, uint32_t numVertices, uint32_t vertexFrom);
	};
}