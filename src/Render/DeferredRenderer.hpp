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

		void setupAmbientLightPass();

		void drawAmbientLight();

		// Sets up the state needed to evaluate the shading equation. Needs to be called after filling the Gbuffer and before submitting
		// the first light.
		void setupDirectionalLightPass();

		// Evaluates the shading equation for these lights using the values written into the GBuffer.
		void runDirectionalLightPass(Vec3* directions, Vec3* colors, size_t numLights);

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
			UniformHandle lightDirEye;
			UniformHandle lightColor;

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

		struct DirectionalLightUniforms
		{
			UniformHandle directions;
			UniformHandle colors;
			UniformHandle numLights;
		} m_dirLights;

		ProgramHandle m_gBufferProgram;
		ProgramHandle m_ambientLightProgram;
		ProgramHandle m_directionaLightProgram;
		ProgramHandle m_copyToBackBufferProgram;

		BlendState m_lightBlendState;

		IRIDevice* m_device;
		IRIContext* m_context;

		void drawStaticMeshWithMaterial(VertexBufferHandle vb, const Material& material, uint32_t numVertices, uint32_t vertexFrom);
	};
}