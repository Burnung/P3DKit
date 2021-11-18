#include "laplacian.h"

BEGIN_P3D_NS
void  LaplacianDeform::init(pDataBlock vps, pDataBlock tls) {
	int nVert = vps->getSize() / sizeof(float32) / 3;
	srcPos.resize(nVert);
	auto vData = vps->rawData<float32>();
	for (int i = 0; i < nVert; ++i) {
		srcPos[i] = Vec3d(vData[i*3],vData[i*3+1],vData[i*3+2]);
	}

	int nTri = tls->getSize() / sizeof(uint32) / 3;
	auto tData = tls->rawData<uint32>();
	meshCells.resize(nTri * 3);
	doFor(i,nTri*3) {
		meshCells[i] = tData[i];
	}
	mAdj = getAdj(srcPos.size(), meshCells);
}

std::vector<int> LaplacianDeform::diffusionVps(PNdArrayF tls, const std::vector<int>& innerIdx, int tSize) {
	int nTri = tls.shape()[0];
	std::vector<int> tCells;
	tCells.resize(nTri * 3);
	int nVerts = 0;
	doFor(i, nTri) {
		doFor(j, 3) {
			tCells[i * 3 + j] = int(tls[{int(i), int(j)}]);
			nVerts = max(tCells[i * 3 + j], nVerts);
		}
	}
	auto tAdj = getAdj(nVerts+1, tCells);
	std::vector<int> retHandls;
	std::vector<int> softSelect;
	int tb;
	selectHandles(nVerts + 1, tAdj, innerIdx, tSize - 1, retHandls, softSelect, tb);
	for (auto tId : softSelect)
		retHandls.push_back(tId);
	return retHandls;

}
PNdArrayF LaplacianDeform::sampleDeform(PNdArrayF srcVps, PNdArrayF tls, const std::vector<int>& newId, PNdArrayF newVps, int regionSize) {
	auto vpsData = srcVps.getDataBlock()->clone();
	auto tlData = std::make_shared<DataBlock>( tls.shape()[0]*3*sizeof(uint32), DType::Uint32);
	auto pData = tlData->rawData<uint32>();
	for (int i = 0; i < tls.shape()[0]; ++i) {
		for (int j = 0; j < 3; ++j)
			pData[i * 3 + j] = (uint32)(tls[{i, j}]);
	}
	this->init(vpsData, tlData);

	std::vector<Vec3f> newVpsData;
	newVpsData.resize(newVps.shape()[0]);
	auto tData = newVps.getDataBlock()->rawData<Vec3f>();
	for (int i = 0; i < newVpsData.size(); ++i) {
		newVpsData[i] = tData[i];
	}
	auto retVps = deformMesh(newId, newVpsData, regionSize);
	
	PNdArrayF retVpsA({ srcVps.shape()[0],srcVps.shape()[1] });
	retVpsA.getDataBlock()->copyFromData(retVps);
	return retVpsA;

}

pDataBlock LaplacianDeform::deformMesh(const std::vector<int>& handelIdx, const std::vector<Vec3f>& newPos,int regionSize){
	cout << "deformMesh:\n";
	s = State();
	//std::vector<vec3> meshPositions;  // vector of vectors whose length equal to 3
	//std::vector<int> meshCells;   // equal to 'corner'
	meshPositions.assign(srcPos.begin(), srcPos.end());

	std::vector<int> handles;
	std::vector<int> unconstrained;
	int boundaryBegin;

	// boundaryBegin: the size of 'vector<int> handles' (except the outmost ring of the ROI)
//void laplacian::selectHandles(const int numVertices, const Adj& adj, const int mainHandle, const int handleRegionSize,
	//const int unconstrainedRegionSize, std::vector<int>& handles, std::vector<int>& unconstrained, int& boundaryBegin)
	selectHandles((int)meshPositions.size(), mAdj, handelIdx, regionSize, handles, unconstrained, boundaryBegin);
	std::cout << "old handle size is " << handelIdx.size() << "new handle size is " << handles.size() << std::endl;

	std::vector<int> combined;
	{
		// put all handles and unconstrained in one array, and send to solver. 
		for (int i = 0; i < handles.size(); ++i) {
			combined.push_back(handles[i]);
		}

		// unconstrainedBegin: size of the complete handles, including the outmost ring
		int unconstrainedBegin = (int)handles.size();

		for (int i = 0; i < unconstrained.size(); ++i) {
			combined.push_back(unconstrained[i]);
		}

		// meshCells.data(): pass a pointer to internally stored memories
		prepareDeform(meshCells.data(), (int)meshCells.size(), (double *)meshPositions.data(),
			(int)meshPositions.size() * 3, combined.data(), (int)combined.size(), unconstrainedBegin, false);

		/*prepareDeform(meshCells.data(), (int)meshCells.size(), (double *)meshPositions.data(),
		(int)meshPositions.size(), combined.data(), (int)combined.size(), unconstrainedBegin, true);*/
	}

	double* arr = new double[(handles.size()) * 3];   // coordinates of the vertices in the handles
	// move the handles. but the boundary vertices keep their old position.
	// operation to move the handle points
	// arr: handles + unconstrained + outermost ring
	{
		int j = 0;
		for (int i = 0; i < (boundaryBegin); ++i)
		{
			arr[j * 3 + 0] = newPos[i].x;
			arr[j * 3 + 1] = newPos[i].y; // 0.4  all handles' y coordinates increase by 0.2
			arr[j * 3 + 2] = newPos[i].z;
			++j;
		}
		// positions of vertices in the outer ring of the ROI remains unchanged
		for (int i = boundaryBegin; i < (handles.size()); ++i)
		{
			arr[j * 3 + 0] = meshPositions[handles[i]].x;
			arr[j * 3 + 1] = meshPositions[handles[i]].y;
			arr[j * 3 + 2] = meshPositions[handles[i]].z;
			++j;
		}
	}
	
	// After the doDeform operation, the 'meshPosition' contains the newly updated position of vertices.
	doDeform(arr, (int)handles.size(), (double*)meshPositions.data());

	delete[] arr;

	// Update mesh vertex positions. Save the updated version to a new mesh.
	pDataBlock retData = std::make_shared<DataBlock>(meshPositions.size() * 3 * sizeof(float32), DType::Float32);
	auto dataPtr = retData->rawData<float32>();
	for (int i = 0; i < meshPositions.size(); ++i) {
		dataPtr[i * 3] = meshPositions[i].x;
		dataPtr[i * 3+1] = meshPositions[i].y;
		dataPtr[i * 3+2] = meshPositions[i].z;
	}
	return retData;

}

Adj LaplacianDeform::getAdj(int numVertices, const std::vector<int>& cells)
{
	cout << "getAdj:\n";
	std::vector<std::vector<int>> adj;  // declare an adjacent list
	for (int i = 0; i < numVertices; ++i) {
		adj.push_back(std::vector<int>());  // initialization
	}
	for (int i = 0; i < cells.size(); i += 3) {
		for (int j = 0; j < 3; ++j) {
			int a = cells[i + (j + 0)];
			int b = cells[i + ((j + 1) % 3)];
			adj[a].push_back(b);
		}
	}
	return adj;   // adj includes all the adjacent vertices (serial number) for all vertices, excluding itself
}


void LaplacianDeform::selectHandles(const int numVertices, const Adj& adj, const std::vector<int>& tHandels,
	const int unconstrainedRegionSize,std::vector<int>& handles, std::vector<int>& unconstrained, int& boundaryBegin)
{
	cout << "selectHandles:\n";
	std::vector<int> currentRing;

	std::vector<bool> visited;   // size of visited equal to number of vertices, all initialized to false
	for (int i = 0; i < numVertices; ++i) {
		visited.push_back(false);
	}

	std::vector<bool> unconstrainedSet;
	std::vector<bool> handlesSet;

	for (int i = 0; i < numVertices; ++i) {
		unconstrainedSet.push_back(false);
		handlesSet.push_back(false);
	}
	for (auto ti : tHandels) {
		handlesSet[ti] = true;
		visited[ti] = true;
		handles.push_back(ti);
	}
	for (auto ti : handles) {
		const auto& tadj = adj[ti];
		for (auto tj : tadj) {
			if (visited[tj]) 
				continue;
			currentRing.push_back(tj);
		}
	}
//	currentRing = tHandels;
	for (int k = 0; k < unconstrainedRegionSize; ++k)
	{
		std::vector<int> nextRing;
		for (int i = 0; i < currentRing.size(); ++i)
		{
			int e = currentRing[i];
			if (visited[e])
				continue;
			unconstrained.push_back(e);
			visited[e] = true;
			unconstrainedSet[e] = true;
			const std::vector<int>& adjs = adj[e];
			for (int j = 0; j < adjs.size(); ++j)
			{
				nextRing.push_back(adjs[j]);
			}
		}
		currentRing = nextRing;
	}

	boundaryBegin = (int)tHandels.size();
	for (int i = 0; i < currentRing.size(); ++i)
	{
		int e = currentRing[i];
		if (visited[e])
			continue;
		handles.push_back(e);
		visited[e] = true;
	}
}

// roiIndices combine index
void LaplacianDeform::prepareDeform(int* cells, const int nCells, double* positions, const int nPositions, int* roiIndices,
	const int nRoi, const int unconstrainedBegin, bool RSI)
{
	cout << "prepareDeform:\n";
	// free memory from previous call of prepareDeform()
	if (s.energyMatrixCholesky != nullptr) {
		delete s.energyMatrixCholesky;
		s.energyMatrixCholesky = nullptr;
	}
	if (s.normalizeDeltaCoordinatesCholesky != nullptr) {
		delete s.normalizeDeltaCoordinatesCholesky;
		s.normalizeDeltaCoordinatesCholesky = nullptr;
	}

	// vector<int> combined;
	std::vector<std::vector<int> > adj;
	std::vector<int> roiMap(nPositions, -1);  // number of vertices * 3 ? * 1

	{
		// nRoi: number of vertices in the ROI
		for (int i = 0; i < nRoi; ++i) {
			roiMap[roiIndices[i]] = i;  // roiIndices = combined（存储的是ROI的顶点serial number）
		}
		adj.resize(nRoi);  // nRoi: number of vertices in the ROI
		for (int i = 0; i < adj.size(); ++i) {
			adj[i] = std::vector<int>();
		}
		// nCells = corner
		for (int i = 0; i < nCells; i += 3) {
			int c[3] = { cells[i + 0], cells[i + 1] , cells[i + 2] };
			for (int j = 0; j < 3; ++j) {
				int a = roiMap[c[j]];  // c[j]在ROI中出现的序列号
				int b = roiMap[c[(j + 1) % 3]];  // [c[(j + 1) % 3]在ROI中出现的序列号
				// a and b are both in the ROI
				if (a != -1 && b != -1) {
					adj[a].push_back(b);
				}
			}
		}
	}

	// put all the positions of the vertices in ROI in a single vector.
	// xyz are all included
	Vec roiPositions(nRoi * 3);
	{
		int c = 0;
		for (int i = 0; i < nRoi; ++i) {
			for (int d = 0; d < 3; ++d) {
				// xyz coordinates are stored in roiPositions[] according to the sequence of vertices in 'combined'
				roiPositions[c++] = positions[3 * roiIndices[i] + d];
				// search in roiIndices = combined: stores serial numbers for vertices
				// sequence conforms with the vertices in the ROI
				// positions[]: get access to the xyz coordinates of the vertices
			}
		}
	}

	std::vector<int> rowBegins;
	vector<LaplacianDeform::Triplet> laplacianCoeffs;
	//std::vector<Eigen::Triplet<double>> laplacianCoeffs;

	/*cotangent laplacian doesnt yield any good results, for some reason so we don't use it. instead, use uniform.*/

	// Return the vector<Triplet> used to construct Laplacian operator (xyz)
	// size of laplacianCoeffs is (3 * nRoi) * (3 * nRoi)
	laplacianCoeffs = calcUniformLaplacianCoeffs(nRoi, adj, rowBegins);

	// Construct sparse matrix from Triplet
	s.lapMat = SpMat(nRoi * 3, nRoi * 3);
	s.lapMat.setFromTriplets(laplacianCoeffs.begin(), laplacianCoeffs.end());

	// by simply multiplying by the laplacian matrix, we can compute the laplacian coordinates(the delta coordinates)
	// of the vertices in ROI.
	s.roiDelta = s.lapMat * roiPositions;  // delta-xyz 

	// we save away the original lengths of the delta coordinates.
	// we need these when normalizing the results of our solver.
	{
		s.roiDeltaLengths = std::vector<double>(s.roiDelta.size() / 3, 0.0f);
		for (int i = 0; i < s.roiDelta.size() / 3; ++i) {
			s.roiDeltaLengths[i] = getLength(s.roiDelta[3 * i + 0], s.roiDelta[3 * i + 1], s.roiDelta[3 * i + 2]);
		}
	}

	std::vector<LaplacianDeform::Triplet> energyMatrixCoeffs;

	// num rows in augmented matrix. 
	int M = (nRoi + unconstrainedBegin) * 3;
	// unconstrainedBegin = handles.size(): unconstrained starts
	// unconstrainedBegin: size of the complete handles, including the outmost ring
	// nRoi = combined.size(): number of vertices in the ROI

	// num columns in augmented matrix.
	int N = nRoi * 3;

	if (RSI) {  // rotation and scale invariant
				// this matrix represents the first term of the energy (5).
				// adj diagonals are 0
		energyMatrixCoeffs = calcEnergyMatrixCoeffs(roiPositions, s.roiDelta, nRoi, adj, rowBegins, laplacianCoeffs);

		// add a diagonal matrix below the laplacianCoeffs matrix
		for (int i = 0; i < unconstrainedBegin; ++i) {
			laplacianCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 0, 3 * i + 0, 1));
			laplacianCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 1, 3 * i + 1, 1));
			laplacianCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 2, 3 * i + 2, 1));
		}

		SpMat augMat(M, N);
		augMat.setFromTriplets(laplacianCoeffs.begin(), laplacianCoeffs.end());
		s.augNormalizeDeltaCoordinatesTrans = augMat.transpose();

		s.normalizeDeltaCoordinatesCholesky =
			new Eigen::SimplicialCholesky<SpMat>(s.augNormalizeDeltaCoordinatesTrans * augMat);
	}
	else {
		// if not rotation-scale-invariant, we simply use the regular laplacian matrix. 
		// This is the first term of the energy (4)
		energyMatrixCoeffs = laplacianCoeffs;
	}

	// in order to add the second term of the energy (4) or (5), we now augment the matrix.
	{
		// we augment the matrix by adding constraints for the handles.
		// these constraints ensure that if the handles are dragged, the handles will strictly follow in the specified direction.
		// the handle vertices are not free, unlike the unconstrained vertices.
		// This corresponds to the second term in equation (5)
		for (int i = 0; i < unconstrainedBegin; ++i) {
			energyMatrixCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 0, 3 * i + 0, 1));
			energyMatrixCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 1, 3 * i + 1, 1));
			energyMatrixCoeffs.push_back(LaplacianDeform::Triplet(i * 3 + N + 2, 3 * i + 2, 1));
		}

		SpMat augMat(M, N);
		augMat.setFromTriplets(energyMatrixCoeffs.begin(), energyMatrixCoeffs.end());
		s.augEnergyMatrixTrans = augMat.transpose();

		// for solving later, we need the cholesky decomposition of (transpose(augMat) * augMat)
		// this is a slow step! probably the slowest part of the entire algorithm.
		s.energyMatrixCholesky = new Eigen::SimplicialCholesky<SpMat>(s.augEnergyMatrixTrans * augMat);
	}

	s.b = Vec(M);
	s.roiIndices = roiIndices;
	s.nRoi = nRoi;
	s.RSI = RSI;

}


vector<LaplacianDeform::Triplet> LaplacianDeform::calcUniformLaplacianCoeffs(int nRoi, std::vector<std::vector<int> > adj,
	std::vector<int>& rowBegins)
{
	cout << "calcUniformLaplacianCoeffs:\n";
	std::vector<LaplacianDeform::Triplet> result;
	std::map<int, double> row;

	for (int i = 0; i < (nRoi * 3); ++i)
	{
		rowBegins.push_back((int)result.size());  // initially 0
		row.clear();

		row[(i % 3) + int(i / 3) * 3] = 1;  // corresponds to vi's coefficients, all are 1
		double w = -1.0 / adj[int(i / 3)].size();   // corresponds to -1/di in the 1-ring neighborhood of current vertex
		for (int j = 0; j < adj[int(i / 3)].size(); ++j) {
			row[(i % 3) + 3 * adj[int(i / 3)][j]] = w;
		}
		for (const auto& p : row) {
			result.push_back(LaplacianDeform::Triplet(i, p.first, p.second));  // items at the same position added up together
		}
	}
	rowBegins.push_back((int)result.size());
	return result;
}


vector<LaplacianDeform::Triplet> LaplacianDeform::calcEnergyMatrixCoeffs(const Vec& roiPositions, const Vec& delta, const int nRoi,
	std::vector<std::vector<int> > adj, const std::vector<int>& rowBegins, const std::vector<LaplacianDeform::Triplet>& laplacianCoeffs)
{
	cout << "calcEnergyMatrixCoeffs:\n";
	std::vector<DMat> Ts;  // vector of MatrixXd
	Ts.resize(nRoi);  // number of vertices in the ROI
	// Each 3 rows correspond to a transformation matrix Ti

	for (int i = 0; i < nRoi; ++i)
	{
		// set of {i} and the neigbbours of i.
		// adj contains serial numbers for all the neighbors of a given vertex
		std::vector<int> iAndNeighbours;
		iAndNeighbours.push_back(i);
		for (int j = 0; j < adj[i].size(); ++j) {
			iAndNeighbours.push_back(adj[i][j]);
		}

		// initialization
		DMat At(7, iAndNeighbours.size() * 3);
		for (int row = 0; row < 7; ++row) {
			for (int col = 0; col < iAndNeighbours.size() * 3; ++col) {
				At(row, col) = 0.0f;
			}
		}

		for (int j = 0; j < iAndNeighbours.size(); ++j)
		{
			int k = iAndNeighbours[j];

			double vk[3];
			vk[0] = roiPositions[3 * k + 0];  // coordinates xyz for a given point vk
			vk[1] = roiPositions[3 * k + 1];
			vk[2] = roiPositions[3 * k + 2];

			const int x = 0;
			const int y = 1;
			const int z = 2;

			// At is the transpose of Ai in the paper
			At(0, j * 3 + 0) = +vk[x];
			At(1, j * 3 + 0) = 0;
			At(2, j * 3 + 0) = +vk[z];
			At(3, j * 3 + 0) = -vk[y];
			At(4, j * 3 + 0) = +1;
			At(5, j * 3 + 0) = 0;
			At(6, j * 3 + 0) = 0;

			At(0, j * 3 + 1) = +vk[y];
			At(1, j * 3 + 1) = -vk[z];
			At(2, j * 3 + 1) = 0;
			At(3, j * 3 + 1) = +vk[x];
			At(4, j * 3 + 1) = 0;
			At(5, j * 3 + 1) = +1;
			At(6, j * 3 + 1) = 0;

			At(0, j * 3 + 2) = +vk[z];
			At(1, j * 3 + 2) = +vk[y];
			At(2, j * 3 + 2) = -vk[x];
			At(3, j * 3 + 2) = 0;
			At(4, j * 3 + 2) = 0;
			At(5, j * 3 + 2) = 0;
			At(6, j * 3 + 2) = 1;
		}

		DMat invprod = (At * At.transpose()).inverse();
		DMat pseudoinv = invprod * At;
		Ts[i] = pseudoinv;
		// Ts[i] now contains (A^T A ) A^T (see equation 12 from paper.) 
		// for every point in nRoi
	}

	std::vector<LaplacianDeform::Triplet> result;
	std::map<int, double> row;  // correspond to a certain row

	for (int i = 0; i < (nRoi * 3); ++i)   // nRoi: number of elements in the Region Of Interest
	{
		row.clear();

		// add uniform weights to matrix (equation 2 from paper)
		for (int ientry = rowBegins[i]; ientry < rowBegins[i + 1]; ++ientry)
		{
			// rowBegins acts as a connection betweeen laplacianCoeffs and energyCoeffs
			LaplacianDeform::Triplet t = laplacianCoeffs[ientry];
			row[t.col()] = t.value();  // uniform weights: 1, -1/di, -1/di,......
									   // other terms are added on the basis of Laplacian Coefficients
		}

		// get delta coordinates for the vertex.  delta-xyz
		double dx = delta[int(i / 3) * 3 + 0];  // delta: s.roiDelta
		double dy = delta[int(i / 3) * 3 + 1];
		double dz = delta[int(i / 3) * 3 + 2];

		std::vector<int> iAndNeighbours;
		iAndNeighbours.push_back(int(i / 3));  // push_back i from 0~nRoi-1 in each iteration
		for (int j = 0; j < adj[int(i / 3)].size(); ++j) {
			iAndNeighbours.push_back(adj[int(i / 3)][j]);  // 包含所有顶点及其邻接顶点的序列号
		}

		DMat T = Ts[int(i / 3)];  // obtain the (A^T A ) A^T for the current vertex in the ROI
								  // T: 大小7*n

		Vec s = T.row(0);  // each row of Ts[i]
		Vec h1 = T.row(1);
		Vec h2 = T.row(2);
		Vec h3 = T.row(3);
		Vec tx = T.row(4);
		Vec ty = T.row(5);
		Vec tz = T.row(6);

		if ((i % 3) == 0) { // x case.
			for (int j = 0; j < T.row(0).size(); ++j) {
				int p = j % 3;
				int q = (int)floor((double)j / (double)3);
				int r = iAndNeighbours[q];  // serial number of the points in the ROI

				row[p + 3 * r] -= dx * (+s[j]);
				row[p + 3 * r] -= dy * (-h3[j]);
				row[p + 3 * r] -= dz * (+h2[j]);
			}
		}
		else if ((i % 3) == 1) { // y case.
			for (int j = 0; j < T.row(0).size(); ++j) {
				int p = j % 3;
				int q = (int)floor((double)j / (double)3);
				int r = iAndNeighbours[q];

				row[p + 3 * r] -= dx * (+h3[j]);
				row[p + 3 * r] -= dy * (+s[j]);
				row[p + 3 * r] -= dz * (-h1[j]);
			}
		}
		else if ((i % 3) == 2) { // z case.
			for (int j = 0; j < T.row(0).size(); ++j) {
				int p = j % 3;
				int q = (int)floor((double)j / (double)3);
				int r = iAndNeighbours[q];

				row[p + 3 * r] -= dx * (-h2[j]);
				row[p + 3 * r] -= dy * (+h1[j]);
				row[p + 3 * r] -= dz * (+s[j]);

			}
		}
		for (const auto& p : row) {
			result.push_back(LaplacianDeform::Triplet(i, p.first, p.second));  //i: row number, p.first: column number, p.second: value
		}
	}

	return result;

}


void LaplacianDeform::doDeform(double* newHandlePositions, int nHandlePositions, double* outPositions)
{
	cout << "doDeform:\n";
	{
		int count = 0;
		for (int i = 0; i < s.roiDelta.size(); ++i) {  // delta-包含点的xyz坐标
			if (s.RSI) {
				// following from our derivations, we must set all these to zero. 
				s.b[count++] = 0.0f; 
			}
			else {
				s.b[count++] = s.roiDelta[i];
			}
		}
		for (int j = 0; j < nHandlePositions; ++j) {   
			// newHandlePositions = 15 + 1
			// assign new handle positions to b[]
			s.b[count++] = newHandlePositions[j * 3 + 0];
			s.b[count++] = newHandlePositions[j * 3 + 1];
			s.b[count++] = newHandlePositions[j * 3 + 2];
		}
	}

	Vec minimizerSolution;
	{
		// Now we solve 
		// Ax = b
		// where A is the energy matrix, and the value of b depends on whether we are optimizing (4) or (5)
		// by solving, we obtain the deformed surface coordinates that minimizes either (4) or (5).
		// s.augEnergyMatrixTrans: transpose of the energy matrix A
		Vec y = s.augEnergyMatrixTrans * s.b;  
		minimizerSolution = s.energyMatrixCholesky->solve(y);   // AT*A*X=A*b
	}

	if (s.RSI) {
		// if minimizing (5), a local scaling is introduced by the solver.
		// so we need to normalize the delta coordinates of the deformed vertices back to their
		// original lengths.
		// otherwise, the mesh will increase in size when manipulating the mesh, which is not desirable.

		// the normalization step is pretty simple:
		// we find the delta coordinates of our solution.			
		// then we normalize these delta coordinates, so that their lengths match the lengths of the original, 
		// undeformed delta coordinates.			
		// then we simply do a minimization to find the coordinates that are as close as possible to the 
		// normalized delta coordinates	
		// and the solution of this minimization is our final solution.

		// solutionDelta: delta coordinates of the solutions
		Vec solutionDelta = s.lapMat * minimizerSolution;

		int count = 0;
		for (int i = 0; i < s.roiDeltaLengths.size(); ++i) {

			double len = getLength(solutionDelta[3 * i + 0], solutionDelta[3 * i + 1], solutionDelta[3 * i + 2]);
			double originalLength = s.roiDeltaLengths[i];
			double scale = originalLength / len;

			for (int d = 0; d < 3; ++d) {
				s.b[count++] = scale * solutionDelta[3 * i + d];
			}
		}

		Vec y = s.augNormalizeDeltaCoordinatesTrans * s.b;
		Vec normalizedSolution = s.normalizeDeltaCoordinatesCholesky->solve(y);

		for (int i = 0; i < s.nRoi; ++i) {
			for(int d = 0;d < 3;++d)
				outPositions[3 * s.roiIndices[i] + d] = normalizedSolution[3 * i + d];
			//newPos.push_back({ normalizedSolution[3 * i],normalizedSolution[3 * i + 1],normalizedSolution[i * 3 + 2] });
		}
	}
	else {
		for (int i = 0; i < s.nRoi; ++i) {
			for(int d = 0;d < 3;++d)
				outPositions[3 * s.roiIndices[i] + d] = minimizerSolution[3 * i + d];
		}
	}

}

// Free engaged memories during the calculation.
void LaplacianDeform::freeDeform()
{
	cout << "freeDeform:\n";
	if (s.energyMatrixCholesky != nullptr)
	{
		delete s.energyMatrixCholesky;
		s.energyMatrixCholesky = nullptr;
	}
	if (s.normalizeDeltaCoordinatesCholesky != nullptr)
	{
		delete s.normalizeDeltaCoordinatesCholesky;
		s.normalizeDeltaCoordinatesCholesky = nullptr;
	}
}


END_P3D_NS



