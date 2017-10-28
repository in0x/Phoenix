#pragma once

namespace Phoenix
{
	class RenderInit;
	class RenderInitWGL;

	class RIWGLGlewSupport
	{
	public:
		RIWGLGlewSupport(RenderInit* initValues);
		~RIWGLGlewSupport();

	private:
		void initGlew();
		void initBasic(const RenderInitWGL& initValues);
		void initWithMSAA(const RenderInitWGL& initValues);
		void checkMsaaSupport(const RenderInitWGL& initValues);

		struct Details;
		Details* m_details;
	};
}