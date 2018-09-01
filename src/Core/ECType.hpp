#pragma once

namespace Phoenix
{
	enum ECType
	{
		CT_Transform,
		CT_StaticMesh,
		CT_PointLight,
		CT_DirectionalLight,
		CT_Max
	};

	static const char* g_ecTypeNames[ECType::CT_Max] = 
	{
		"Transform",
		"StaticMesh",
		"PointLight",
		"DirectionalLight"
	};
}
