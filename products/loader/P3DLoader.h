#include "common/RenderCommon.h"
#include "common/mathVector.hpp"
#include "geo/comp.h"
#include "geo/P3DSkinMesh.h"
#include "common/P3D_Utils.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"


//#include ""
BEGIN_P3D_NS
class P3DLoader :public SingletonBase<P3DLoader>,P3DObject {
	P3D_SINGLETON_DECLEAR(P3DLoader)
public:
	const unsigned int importFlags = 
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		//aiProcess_PreTransformVertices |
		//aiProcess_OptimizeMeshes |
	//	aiProcess_OptimizeGraph|
		//aiProcess_Debone |
		aiProcess_ValidateDataStructure;

public:
	std::vector<pComp> loadComps(const std::string& fPath, bool calcTangent=false);
	pSkinMesh loadFbxMesh(const std::string& fPath,bool slitVertex = false);
	

	pCPuImage loadCpuImg(const std::string& fPath);
private:
	pComp convertAiMesh2Comp(const aiMesh* pMesh);
	template<typename T>
	void swapRedBlue(PNdArrayT<T>& arrData) {
		P3D_ENSURE(arrData.shape()[2] >= 3,"array channel must >= 3")
		for (int r = 0; r < arrData.shape()[0]; ++r) {
			for (int c = 0; c < arrData.shape()[1]; ++c)
				std::swap(arrData[{r, c, 0}], arrData[{r, c, 2}]);
		}
	}

private:
	std::unordered_map<std::string, Mat4f> fbxTmpl2gMat;

};






END_P3D_NS