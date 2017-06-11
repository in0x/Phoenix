#pragma once
#include "Math/PhiMath.hpp"
#include <map>
#include <memory>
#include "ObjStructs.hpp"

namespace Phoenix
{
	class ObjIndexer
	{
		struct PackedVertexData
		{
			Vec3 vertex;
			Vec3 normal;
			Vec2 uv;

			bool operator<(const PackedVertexData rhv) const
			{
				return memcmp((void*)this, (void*)&rhv, sizeof(PackedVertexData)) > 0;
			};
		};

		std::map<PackedVertexData, unsigned int> packed;
		std::unique_ptr<Mesh> pConvertedMesh;

	public:
		std::unique_ptr<Mesh> convertForOpenGL(ObjData* loaded);

	private:
		void(ObjIndexer::*toPacked)(PackedVertexData*, const Face&, const ObjData*);
		void(ObjIndexer::*addData)(const PackedVertexData&);

		bool doesDataExist(const PackedVertexData& data, unsigned int& outIndex);

		void toPackedV(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVT(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);
		void toPackedVTN(PackedVertexData* dataArr, const Face& face, const ObjData* loaded);

		void addV(const PackedVertexData& data);
		void addVN(const PackedVertexData& data);
		void addVT(const PackedVertexData& data);
		void addVTN(const PackedVertexData& data);

		void addDataAndIndex(const PackedVertexData& data);
	};
}
