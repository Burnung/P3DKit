//
//  HairSmoother.h
//  HairSyn
//
//  Created by Liwen Hu on 11/9/13.
//  Copyright (c) 2013 Liwen Hu. All rights reserved.
//

#ifndef __HairSyn__HairSmoother__
#define __HairSyn__HairSmoother__

#include "HairStrands.h"
#include "HairScalp.h"
#include "common/NdArray.h"

class CHairGrid
{
public:
    vector<Vec3f> tangents;
};

class Tensor
{
public:
    Tensor operator+(Tensor &t) {
        Tensor result;
        for (int i=0; i<6; i++) {
            result.cov[i] = this->cov[i]+t.cov[i];
        }
        return result;
    }
    
    Tensor operator*(float &t) {
        Tensor result;
        for (int i=0; i<6; i++) {
            result.cov[i] = this->cov[i]*t;
        }
        return result;
    }
    
    float cov[6];
};

class CVectorGrid
{
public:
    Vec3f vector;
    int isFull;
    Tensor tensor;
};

class CHairSmoother
{
public:
    CHairSmoother();
    CHairSmoother(CHairScalp* _scalp);
    CHairSmoother(CHairStrands* _inputStrands, float _gridSize);
    CHairSmoother(CHairStrands* _inputStrands, CHairScalp* _scalp, float _gridSize);
    CHairSmoother(float _gridSize, Vec3f _minAABB, Vec3f _maxAABB, Vec3i _gridNum);
    CHairSmoother(CHairStrands* _inputStrands, float _gridSize, Vec3f _minAABB, Vec3f _maxAABB, Vec3i _gridNum);
    ~CHairSmoother();
   
//volumes 2 strand hair
	void compNewStrands(const std::string& fPath);
	bool setOrientationField(P3D::PNdArrayF arrayV, P3D::PNdArrayF arrayF, float threshold=0.0);
	bool testNdArray(P3D::PNdArrayF arrF, P3D::PNdArrayI arrI);
	bool init(const std::string rootPositionFile);
	bool loadScalp(const std::string& scalPath);
	bool setGridSize(float grideSize,const std::vector<float>& minAABB, const std::vector<float>& maxAABB);
    bool LoadRootPosition(const char* fileName);



    void ComputeTangentsField(bool flag = false, CHairStrands* coveredStrands = NULL);
    
    void ComputeVectorField();
    
    void FillHole(CHairScalp* scalp);
    
    void GenerateHairStrands(int num = 1);
    
    void GenerateHairStrandsfromRootPositions();
    
    void GeneratePonytailStrandsfromRootPositions();
    
    void GenerateUniformDistributionHairStrands(int num);
    
    void GenerateHairStrandsBlank(int num);
    
    CHairStrands* GetSmoothStrands() { return m_smoothStrands; }
    
    bool DumpRootPosition(const char* fileName, int num = 10000);
    
    
    bool DumpOrientationField(const char* fileName);
    
    bool LoadOrientationField(const char* fileName);

    
    float CheckUncompatableGrid();
    
    Vec3i GetGridId(Vec3f pos);
    
    Vec3f GetGridCenter(Vec3i gridId);
    
    Vec3f GetGridVector(Vec3i gridId);
    
    CVectorGrid GetGrid(Vec3i gridId);
    
    Vec3f InterpolateVector(Vec3f pos);
    
    void SetGridVector(Vec3i gridId, Vec3f vector);
    
    void SetGrid(Vec3i gridId, CVectorGrid grid);
    
    vector<vector<Vec3i> >& GetFilledGrids() { return m_filledGrids; }

protected:
    void FillTangentsField(Vec3f startPos, Vec3f endPos, int flag = -1);
    
    template<class T> T*** My3dMalloc(int xSize, int ySize, int zSize);

	template<typename T>
	void my3dVectorMalloc(std::vector<std::vector<std::vector<T>>>& ret, int xSize, int ySize, int zSize) {
		ret.clear();
		ret.resize(zSize);
		
		for (int i = 0; i < zSize; ++i) {
			ret[i].resize(ySize);
			for (int j = 0; j < ySize; ++j) {
				ret[i][j].resize(xSize);
			}
		}
	}
    
    void ComputeOrientFromTensor(Tensor& tensor, Vec3f& orient);
    
    void ComputeTensorFromOrient(Tensor& tensor, Vec3f& orient);
    
    CHairStrands* m_inputStrands;
	CHairStrands* m_smoothStrands = nullptr;
	CHairScalp* m_scalp = nullptr;
    float m_gridSize;
    Vec3i m_gridNum;
    Vec3f m_minAABB;
    Vec3f m_maxAABB;
	std::vector<std::vector<std::vector<CHairGrid>>>  m_tangentsField;
	std::vector<std::vector<std::vector<CVectorGrid>>> m_vectorField;
//	CHairGrid*** m_tangentsField = nullptr;
//	CVectorGrid*** m_vectorField = nullptr;
    
    vector<Vec3f> rootPostions;
    vector<Vec3f> rootNormals;
    vector<vector<Vec3i> > m_filledGrids;
    
    vector<Vec3f> constrainedBox;
};

#endif /* defined(__HairSyn__HairSmoother__) */
