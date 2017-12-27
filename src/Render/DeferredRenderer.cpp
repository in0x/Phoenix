#include "DeferredRenderer.hpp"

#include <Core/Shader.hpp>
#include <Core/WorldObject.hpp>
#include <Core/PerspectiveCamera.hpp>

#include <Render/RIDevice.hpp>
#include <Render/RIContext.hpp>

namespace Phoenix
{
	DeferredRenderer::DeferredRenderer(IRIDevice* renderDevice, uint32_t gBufferWidth, uint32_t gBufferHeight)
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

	void DeferredRenderer::setupGBufferPass(IRIContext* context)
	{
		context->bindRenderTarget(m_gBuffer);
		context->clearRenderTargetColor(m_gBuffer, RGBA{ 0.f, 0.f, 0.f, 0.f });
		context->clearRenderTargetDepth(m_gBuffer);

		context->setDepthTest(EDepth::Enable);
		context->setBlendState(BlendState(EBlend::Disable));
		context->bindShaderProgram(m_gBufferProgram);
		context->bindUniform(m_uniforms.viewTf, &m_viewMat);
		context->bindUniform(m_uniforms.projTf, &m_projMat);
	}

	
	// We'll use the context directly for now, then commands when we refactor.
	void DeferredRenderer::fillGBuffer(const WorldObject& entity, IRIContext* context)
	{
		context->bindUniform(m_uniforms.modelTf, &entity.m_transform.toMat4());

		context->bindUniform(m_uniforms.kDiffuse, &entity.m_material.m_diffuse);
		context->bindUniform(m_uniforms.kSpecular, &entity.m_material.m_specular);
		context->bindUniform(m_uniforms.specExp, &entity.m_material.m_specularExp);

		context->drawIndexed(entity.m_mesh.m_vertexbuffer, entity.m_mesh.m_indexbuffer, EPrimitive::Triangles);
	}

	void DeferredRenderer::setupLightPass(IRIContext* context)
	{
		context->bindDefaultRenderTarget();
		context->setBlendState(m_lightBlendState);
		context->clearColor();
		context->clearDepth();

		context->setDepthTest(EDepth::Disable);
	}

	// Again do this for each light with context directly now, later we 
	// can produce commands and sort them so we only set this state once.
	void DeferredRenderer::drawLight(const DirectionalLight& light, IRIContext* context)
	{
		context->bindShaderProgram(m_directionaLightProgram);

		context->bindUniform(m_uniforms.projTf, &m_projMat);

		context->bindTexture(m_normalSpecExpTex);
		context->bindTexture(m_kDiffuseDepthTex);
		context->bindTexture(m_kSpecularTex);

		Vec3 lightDirEye = m_viewMat * light.m_direction;

		context->bindUniform(m_uniforms.lightDirEye, &lightDirEye);
		context->bindUniform(m_uniforms.lightColor, &light.m_color);

		context->drawLinear(EPrimitive::Triangles, 3, 0);
	}
}