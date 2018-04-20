#include "DeferredRenderer.hpp"

#include <Core/Shader.hpp>
#include <Core/Mesh.hpp>
#include <Core/Logger.hpp>

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

namespace Phoenix
{
	DeferredRenderer::DeferredRenderer(IRIDevice* renderDevice, IRIContext* renderContext, uint32_t gBufferWidth, uint32_t gBufferHeight)
		: m_device(renderDevice)
		, m_context(renderContext)
	{
		TextureDesc desc;
		desc.pixelFormat = EPixelFormat::RGBA16F;
		desc.width = gBufferWidth;
		desc.height = gBufferHeight;

		m_normalSpecExpTex = renderDevice->createTexture2D(desc, "normalRGBSpecExpA_tex");

		desc.pixelFormat = EPixelFormat::RGBA16F;
		m_kDiffuseDepthTex = renderDevice->createTexture2D(desc, "kDiffuseRGBDepthA_tex");
		
		desc.pixelFormat = EPixelFormat::RGB16F;
		m_kSpecularTex = renderDevice->createTexture2D(desc, "kSpecularRGB_tex");

		desc.pixelFormat = EPixelFormat::Depth32F;
		m_depthTex = renderDevice->createTexture2D(desc, "depth_tex");

		RenderTargetDesc gBufferDesc;
		gBufferDesc.colorAttachs[RenderTargetDesc::Color0] = m_normalSpecExpTex;
		gBufferDesc.colorAttachs[RenderTargetDesc::Color1] = m_kDiffuseDepthTex;
		gBufferDesc.colorAttachs[RenderTargetDesc::Color2] = m_kSpecularTex;
		gBufferDesc.depthAttach = m_depthTex;

		m_gBuffer = renderDevice->createRenderTarget(gBufferDesc);

		desc.pixelFormat = EPixelFormat::RGBA16F;
		m_kFinalTex = renderDevice->createTexture2D(desc, "kfinalRGBA_tex");

		RenderTargetDesc backBufferDesc;
		backBufferDesc.colorAttachs[RenderTargetDesc::Color0] = m_kFinalTex;

		m_backBuffer = renderDevice->createRenderTarget(backBufferDesc);

		m_uniforms.modelTf = renderDevice->createUniform("modelTf", EUniformType::Mat4);
		m_uniforms.viewTf = renderDevice->createUniform("viewTf", EUniformType::Mat4);
		m_uniforms.projTf = renderDevice->createUniform("projectionTf", EUniformType::Mat4);

		m_uniforms.kDiffuse = renderDevice->createUniform("kDiffuse", EUniformType::Vec3);
		m_uniforms.kSpecular = renderDevice->createUniform("kSpecular", EUniformType::Vec3);
		m_uniforms.specExp = renderDevice->createUniform("specularExp", EUniformType::Float);

		m_uniforms.lightDirEye = renderDevice->createUniform("lightDirectionEye", EUniformType::Vec3);
		m_uniforms.lightColor = renderDevice->createUniform("lightColor", EUniformType::Vec3);

		m_gBufferProgram = loadShaderProgram(renderDevice, "Shaders/deferred/buildGBuffer.vert", "Shaders/deferred/buildGBuffer.frag");
		m_directionaLightProgram = loadShaderProgram(renderDevice, "Shaders/deferred/drawDirectionalLight.vert", "Shaders/deferred/drawDirectionalLight.frag");
		m_copyToBackBufferProgram = loadShaderProgram(renderDevice, "Shaders/deferred/copyToBackBuffer.vert", "Shaders/deferred/copyToBackBuffer.frag");

		m_lightBlendState = BlendState(EBlendOp::Add, EBlendFactor::One, EBlendFactor::One);
	}

	void DeferredRenderer::setViewMatrix(const Matrix4& view)
	{
		m_viewMat = view;
	}

	void DeferredRenderer::setProjectionMatrix(const Matrix4& projection)
	{
		m_projMat = projection;
	}

	void DeferredRenderer::setupGBufferPass()
	{
		m_context->bindRenderTarget(m_gBuffer);
		m_context->clearRenderTargetColor(m_gBuffer, RGBA{ 0.f, 0.f, 0.f, 0.f });
		m_context->clearRenderTargetDepth(m_gBuffer);

		m_context->setDepthTest(EDepth::Enable);
		m_context->setDepthWrite(EDepth::Enable);
		m_context->setBlendState(BlendState(EBlend::Disable));
		m_context->bindShaderProgram(m_gBufferProgram);
		m_context->bindUniform(m_uniforms.viewTf, &m_viewMat);
		m_context->bindUniform(m_uniforms.projTf, &m_projMat);
	}

	void DeferredRenderer::drawStaticMesh(const StaticMesh& mesh, const Matrix4& transform)
	{
		if (mesh.m_numMaterials == 0)
		{
			Logger::warning("Trying to draw static mesh with no material, this is currently not handled. Skipping drawing.");
			return;
		}

		m_context->bindUniform(m_uniforms.modelTf, &transform);

		if (mesh.m_indexbuffer.isValid() && mesh.m_numIndices > 0)
		{
			const Material& material = mesh.m_materials[0];

			m_context->bindTexture(material.m_diffuseTex);
			m_context->drawIndexed(mesh.m_vertexbuffer, mesh.m_indexbuffer, EPrimitive::Triangles);
		}
		else
		{
			size_t materialIdx = 0;

			for (; materialIdx < mesh.m_numMaterials - 1; ++materialIdx)
			{
				const Material& material = mesh.m_materials[materialIdx];
				size_t currVertexIdx = mesh.m_vertexFrom[materialIdx];

				m_context->bindTexture(material.m_diffuseTex);
				m_context->drawLinear(mesh.m_vertexbuffer, EPrimitive::Triangles, mesh.m_vertexFrom[materialIdx + 1] - currVertexIdx, currVertexIdx);
			}

			const Material& material = mesh.m_materials[materialIdx];
			size_t currVertexIdx = mesh.m_vertexFrom[materialIdx];

			m_context->bindTexture(material.m_diffuseTex);
			m_context->drawLinear(mesh.m_vertexbuffer, EPrimitive::Triangles, mesh.m_numVertices - currVertexIdx, currVertexIdx);
		}

		m_context->unbindTextures();
	}

	void DeferredRenderer::setupLightPass()
	{
		m_context->bindRenderTarget(m_backBuffer);
		m_context->clearRenderTargetColor(m_backBuffer, RGBA{ 0.f, 0.f, 0.f, 0.f });
		
		m_context->setBlendState(m_lightBlendState);	
		m_context->setDepthTest(EDepth::Disable);
		
		m_context->bindShaderProgram(m_directionaLightProgram);

		//m_context->bindUniform(m_uniforms.projTf, &m_projMat);

		m_context->bindTexture(m_normalSpecExpTex);
		m_context->bindTexture(m_kDiffuseDepthTex);
	}

	void DeferredRenderer::drawLight(Vec3 direction, Vec3 color)
	{
		Vec3 lightDirEye = m_viewMat * direction;

		m_context->bindUniform(m_uniforms.lightDirEye, &lightDirEye);
		m_context->bindUniform(m_uniforms.lightColor, &color);
		m_context->drawLinear(EPrimitive::TriangleStrips, 4, 0);
	}

	void DeferredRenderer::copyFinalColorToBackBuffer()
	{
		m_context->unbindTextures();

		m_context->bindDefaultRenderTarget();
		m_context->clearColor();
		m_context->clearDepth();

		m_context->setDepthTest(EDepth::Disable);

		m_context->bindShaderProgram(m_copyToBackBufferProgram);
		m_context->bindTexture(m_kFinalTex);

		m_context->drawLinear(EPrimitive::TriangleStrips, 4, 0);

		m_context->unbindTextures();
	}
}