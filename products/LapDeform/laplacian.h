#pragma once
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <math.h>
#include <set>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <algorithm>
#include <vector>
#include "common/P3D_Utils.h"
#include "common/NdArray.h"
#include "common/mathVector.hpp"

using namespace std;
using namespace Eigen;

BEGIN_P3D_NS
typedef vector<std::vector<int>> Adj;
typedef Eigen::SparseMatrix<double> SpMat;
typedef Eigen::MatrixXd DMat;
//typedef Eigen::Triplet<double> Triplet;
typedef Eigen::VectorXd Vec;

// Declaration before instantiation
class vec3;


struct State 
{
	bool RSI = true;      // Region Of Interest
	Vec roiDelta;   // delta-xyz, Laplacian coordinates for all the vertices in the ROI
	SpMat augEnergyMatrixTrans;
	SpMat augNormalizeDeltaCoordinatesTrans;
	Eigen::SimplicialCholesky<SpMat>*energyMatrixCholesky = nullptr;
	Eigen::SimplicialCholesky<SpMat>*normalizeDeltaCoordinatesCholesky = nullptr;
	int* roiIndices;
	int nRoi;
	SpMat lapMat;
	std::vector<double> roiDeltaLengths;
	Vec b;
};

class LaplacianDeform{
public:
	typedef Eigen::Triplet<double> Triplet;
	LaplacianDeform() = default;
	void init(pDataBlock vps,pDataBlock tls); 
	//void initData(PNdArrayF vsp,PNdArrayF)

	//void runDemo();
	pDataBlock deformMesh(const std::vector<int>& handelIdx, const std::vector<Vec3f>& newPos,int regionSize = 5);

	PNdArrayF sampleDeform(PNdArrayF srcVps, PNdArrayF tls, const std::vector<int>& newId, PNdArrayF newVps, int regionSize = 5);
	std::vector<int> diffusionVps(PNdArrayF tls, const std::vector<int>& innerIdx,int tSize=3);
	//std::vector<std::vector<int>> getA

private:
	double getLength(double ax, double ay, double az) { return sqrt(ax*ax + ay * ay + az * az); }  // Return length of a vector

	Adj getAdj(int numVertices, const std::vector<int>& cells);  // Get the adjacency information of the mesh topology
	void selectHandles(const int numVertices, const Adj& adj, const std::vector<int>& mainHandle,
		const int unconstrainedRegionSize, std::vector<int>& handles, std::vector<int>& unconstrained, int& boundaryBegin);
	void prepareDeform(int* cells, const int nCells, double* positions, const int nPositions, int* roiIndices, const int nRoi,
		const int unconstrainedBegin, bool RSI);
	vector<LaplacianDeform::Triplet> calcUniformLaplacianCoeffs(int nRoi, std::vector<std::vector<int> > adj,
		std::vector<int>& rowBegins);
	vector<LaplacianDeform::Triplet> calcEnergyMatrixCoeffs(const Vec& roiPositions, const Vec& delta, const int nRoi,
		std::vector<std::vector<int> > adj, const std::vector<int>& rowBegins, const std::vector<Triplet>& laplacianCoeffs);
	void doDeform(double* newHandlePositions, int nHandlePositions, double*);
	
	// Free memories
	void freeDeform();  
	

	State s;  // Define state variables
	std::vector<Vec3d> srcPos;
	std::vector<Vec3d> meshPositions;
	std::vector<int> meshCells;
	Adj mAdj;
};
/*
class vec3 {
public:
	double x, y, z;

	vec3(double x, double y, double z) { this->x = x; this->y = y; this->z = z; }
	vec3(double v) { this->x = v; this->y = v; this->z = v; }
	vec3() { this->x = this->y = this->z = 0; }
	vec3& operator+=(const vec3& b) { (*this) = (*this) + b; return (*this); }
	vec3& operator-=(const vec3& b) { (*this) = (*this) - b; return (*this); }

	friend vec3 operator-(const vec3& a, const vec3& b) { return vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
	friend vec3 operator+(const vec3& a, const vec3& b) { return vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
	friend vec3 operator*(const double s, const vec3& a) { return vec3(s * a.x, s * a.y, s * a.z); }
	friend vec3 operator*(const vec3& a, const double s) { return s * a; }

	static double length(const vec3& a) { return sqrt(vec3::dot(a, a)); }

	// dot product.
	static double dot(const vec3& a, const vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

	static double distance(const vec3& a, const vec3& b) { return length(a - b); }
	static vec3 normalize(const vec3& a) { return (1.0f / vec3::length(a)) * a; }

	// cross product.
	static vec3 cross(const vec3& a, const vec3& b){
		return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}
};
*/
END_P3D_NS



