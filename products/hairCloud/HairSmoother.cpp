//
//  HairSmoother.cpp
//  HairSyn
//
//  Created by Liwen Hu on 11/9/13.
//  Copyright (c) 2013 Liwen Hu. All rights reserved.
//

#include "HairSmoother.h"
#include "omp.h"

CHairSmoother::CHairSmoother()
{

}

CHairSmoother::CHairSmoother(CHairScalp* _scalp)
{
    m_scalp = _scalp;
    m_smoothStrands = new CHairStrands();
}

CHairSmoother::CHairSmoother(CHairStrands* _inputStrands, float _gridSize)
{
    m_inputStrands = _inputStrands;
    m_gridSize = _gridSize;
    m_smoothStrands = new CHairStrands();
    Vec3f minAABB = m_inputStrands->GetAABBmin();
    Vec3f maxAABB = m_inputStrands->GetAABBmax();
    m_gridNum[0] = (maxAABB[0]-minAABB[0])/m_gridSize;
    m_gridNum[1] = (maxAABB[1]-minAABB[1])/m_gridSize;
    m_gridNum[2] = (maxAABB[2]-minAABB[2])/m_gridSize;
    for (int i=0; i<3; i++) {
        m_gridNum.v[i] = (maxAABB.v[i]-minAABB.v[i])/m_gridSize;
        if ( m_gridNum.v[i]*m_gridSize < maxAABB.v[i]-minAABB.v[i] ) {
            m_gridNum.v[i] = m_gridNum.v[i]+1;
            m_minAABB.v[i] = (maxAABB.v[i]+minAABB.v[i])/2.0f - m_gridNum.v[i]*m_gridSize/2.0f;
            m_maxAABB.v[i] = (maxAABB.v[i]+minAABB.v[i])/2.0f + m_gridNum.v[i]*m_gridSize/2.0f;
        } else {
            m_minAABB.v[i] = minAABB.v[i];
            m_maxAABB.v[i] = maxAABB.v[i];
        }
    }
	my3dVectorMalloc<CHairGrid>(m_tangentsField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	my3dVectorMalloc<CVectorGrid>(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
}

CHairSmoother::CHairSmoother(CHairStrands* _inputStrands, CHairScalp* _scalp, float _gridSize)
{
    m_inputStrands = _inputStrands;
    m_gridSize = _gridSize;
    m_scalp = _scalp;
    m_smoothStrands = new CHairStrands();
    Vec3f minAABB = m_inputStrands->GetAABBmin();
    Vec3f maxAABB = m_inputStrands->GetAABBmax();
    Vec3f diff = maxAABB-minAABB;
    minAABB -= diff*0.05f;
    maxAABB += diff*0.05f;
    m_gridNum[0] = (maxAABB[0]-minAABB[0])/m_gridSize;
    m_gridNum[1] = (maxAABB[1]-minAABB[1])/m_gridSize;
    m_gridNum[2] = (maxAABB[2]-minAABB[2])/m_gridSize;
    for (int i=0; i<3; i++) {
        m_gridNum.v[i] = (maxAABB.v[i]-minAABB.v[i])/m_gridSize;
        if ( m_gridNum.v[i]*m_gridSize < maxAABB.v[i]-minAABB.v[i] ) {
            m_gridNum.v[i] = m_gridNum.v[i]+1;
            m_minAABB.v[i] = (maxAABB.v[i]+minAABB.v[i])/2.0f - m_gridNum.v[i]*m_gridSize/2.0f;
            m_maxAABB.v[i] = (maxAABB.v[i]+minAABB.v[i])/2.0f + m_gridNum.v[i]*m_gridSize/2.0f;
        } else {
            m_minAABB.v[i] = minAABB.v[i];
            m_maxAABB.v[i] = maxAABB.v[i];
        }
    }
	my3dVectorMalloc<CHairGrid>(m_tangentsField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	my3dVectorMalloc<CVectorGrid>(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
   // m_tangentsField = My3dMalloc<CHairGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    //m_vectorField = My3dMalloc<CVectorGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
}

CHairSmoother::CHairSmoother(float _gridSize, Vec3f _minAABB, Vec3f _maxAABB, Vec3i _gridNum)
{
    m_smoothStrands = new CHairStrands();
    m_gridSize = _gridSize;
    m_minAABB = _minAABB;
    m_maxAABB = _maxAABB;
    m_gridNum = _gridNum;
    //m_tangentsField = My3dMalloc<CHairGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    //m_vectorField = My3dMalloc<CVectorGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	my3dVectorMalloc<CHairGrid>(m_tangentsField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	my3dVectorMalloc<CVectorGrid>(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
}

CHairSmoother::CHairSmoother(CHairStrands* _inputStrands, float _gridSize, Vec3f _minAABB, Vec3f _maxAABB, Vec3i _gridNum)
{
    m_smoothStrands = new CHairStrands();
    m_inputStrands = _inputStrands;
    m_gridSize = _gridSize;
    m_minAABB = _minAABB;
    m_maxAABB = _maxAABB;
    m_gridNum = _gridNum;

	my3dVectorMalloc<CHairGrid>(m_tangentsField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	my3dVectorMalloc<CVectorGrid>(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    //m_tangentsField = My3dMalloc<CHairGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    //m_vectorField = My3dMalloc<CVectorGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
}

CHairSmoother::~CHairSmoother()
{
    //delete m_smoothStrands;
	m_tangentsField.clear();
	m_vectorField.clear();
    if (m_smoothStrands) {
		delete m_smoothStrands;
	}
}

void CHairSmoother::ComputeTangentsField(bool flag, CHairStrands* coveredStrands)
{
    if (flag == true)
    {
        m_filledGrids.resize(m_inputStrands->GetNumOfStrands());
        for (int i=0; i<m_inputStrands->GetNumOfStrands(); i++) {
            CHairStrand& strand = m_inputStrands->GetStrand(i);
            if (strand.GetNumOfSamples() == 1)
            {
                Vec3f p = strand.GetSample(0).GetPos();
                Vec3i gridId = GetGridId(p);
                m_filledGrids[i].push_back(gridId);
            }
            else
            {
                for (int j=0; j<strand.GetNumOfSamples()-1; j++) {
                    Vec3f startPos = strand.GetSample(j).GetPos();
                    Vec3f endPos = strand.GetSample(j+1).GetPos();
                    FillTangentsField(startPos, endPos, i);
                }
            }
        }
    }
    else
    {
        for (int i=0; i<m_inputStrands->GetNumOfStrands(); i++) {
            CHairStrand& strand = m_inputStrands->GetStrand(i);
            for (int j=0; j<strand.GetNumOfSamples()-1; j++) {
                Vec3f startPos = strand.GetSample(j).GetPos();
                Vec3f endPos = strand.GetSample(j+1).GetPos();
                if (startPos[0] != startPos[0] || startPos[1] != startPos[1] || startPos[2] != startPos[2])
                    continue;
                if (endPos[0] != endPos[0] || endPos[1] != endPos[1] || endPos[2] != endPos[2])
                    continue;
                FillTangentsField(startPos, endPos, -1);
            }
        }
    }

    if (coveredStrands != NULL) {
        for (int i=0; i<coveredStrands->GetNumOfStrands(); i++) {
            CHairStrand& strand = coveredStrands->GetStrand(i);
            for (int j=0; j<strand.GetNumOfSamples()-1; j++) {
                Vec3f startPos = strand.GetSample(j).GetPos();
                Vec3f endPos = strand.GetSample(j+1).GetPos();
                if (startPos[0] != startPos[0])
                    continue;
                FillTangentsField(startPos, endPos);
            }
        }
    }
}

void CHairSmoother::ComputeVectorField()
{
    //(Example-Based Hair Geometry Synthesis[Section 5.3])
#pragma omp parallel for
    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                CHairGrid& hairGrid = m_tangentsField[i][j][k];
                CVectorGrid& vectorGrid = m_vectorField[i][j][k];
                if (hairGrid.tangents.size() == 0) {
                    vectorGrid.isFull = 0;
                } else {
                    Vec3f vecbar = Vec3f(0.0f, 0.0f, 0.0f);
                    int iteration = 0;
                    int iterationMax = 3;
                    while (iteration < iterationMax) {
                        float totalWeight = 0.0f;
                        Vec3f totalVec = Vec3f(0.0f, 0.0f, 0.0f);
                        for (int n=0; n<hairGrid.tangents.size(); n++) {
                            float weight = 0.5f*(dot(hairGrid.tangents[n], vecbar)+1.0f);
                            totalWeight += weight;
                            totalVec = totalVec + weight*hairGrid.tangents[n];
                        }
                        vecbar = totalVec / totalWeight;
                        iteration++;
                    }
                    vectorGrid.isFull = 1;
                    vectorGrid.vector = vecbar;
                }
            }
        }
    }
    //free(m_tangentsField);
}

float CHairSmoother::CheckUncompatableGrid()
{
    int num = 0;
    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                CHairGrid& hairGrid = m_tangentsField[i][j][k];
                if (hairGrid.tangents.size() >= 2)
                {
                    bool flag = true;
                    for (int n = 0; n < hairGrid.tangents.size() - 1; n++)
                    {
                        for (int m = n + 1; m < hairGrid.tangents.size(); m++)
                        {
                            if (dot(hairGrid.tangents[n], hairGrid.tangents[m]) < 0.5f)
                            {
                                flag = false;
                                break;
                            }
                        }
                        if (flag == false)
                        {
                            break;
                        }
                    }
                    if (flag == false)
                    {
                        num++;
                    }
                }
            }
        }
    }
    return num;
}

void CHairSmoother::FillHole(CHairScalp* scalp)
{
    //(Hair Photobooth: Geometric and Photometric Acquisition of Real Hairstyles[Section 3.3])
    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                CVectorGrid& vectorGrid = m_vectorField[i][j][k];
                if (vectorGrid.isFull > 0) {
                    ComputeTensorFromOrient(vectorGrid.tensor, vectorGrid.vector);
                } else {
                    vectorGrid.isFull = 0;
                    vectorGrid.vector = Vec3f(0.0f, 0.0f, 0.0f);
                    ComputeTensorFromOrient(vectorGrid.tensor, vectorGrid.vector);
                }
            }
        }
    }

    vector<Vec3i>& faces = m_scalp->GetFaces();
    vector<Vec3f>& vertice = m_scalp->GetVertice();
    vector<Vec3f>& normals = m_scalp->GetNormals();

    for (int i=0; i<faces.size(); i++) {
        Vec3f bbmin = vertice[faces[i][0]];
        Vec3f bbmax = vertice[faces[i][0]];
        for (int j=1; j<3; j++) {
            bbmin = Vec3f( min(bbmin[0], vertice[faces[i][j]][0]), min(bbmin[1], vertice[faces[i][j]][1]), min(bbmin[2], vertice[faces[i][j]][2]) );
            bbmax = Vec3f( max(bbmax[0], vertice[faces[i][j]][0]), max(bbmax[1], vertice[faces[i][j]][1]), max(bbmax[2], vertice[faces[i][j]][2]) );
        }
        Vec3f planeNormal = cross(vertice[faces[i][0]]-vertice[faces[i][1]], vertice[faces[i][0]]-vertice[faces[i][2]]);
        normalize(planeNormal);
        if (dot(planeNormal, normals[faces[i][0]])<0.0f) {
            planeNormal = -planeNormal;
        }
        float d = -dot(planeNormal, vertice[faces[i][0]]);
        Vec3i boxmin = GetGridId(bbmin);
        Vec3i boxmax = GetGridId(bbmax);
        if (boxmin[0]==-1 || boxmax[0]==-1)
            continue;
        for (int x=boxmin[0]; x<=boxmax[0]; x++) {
            for (int y=boxmin[1]; y<=boxmax[1]; y++) {
                for (int z=boxmin[2]; z<=boxmax[2]; z++) {
                    Vec3f p = GetGridCenter(Vec3i(x, y, z));
                    Vec3f proj = hair_math::ProjectPointOnPlane(p, planeNormal, d);
                    Vec3f dir;
                    if (dist(p, proj)<m_gridSize && m_scalp->ComputeRootDirection(proj, i, dir)) {
                        CVectorGrid& vectorGrid = m_vectorField[z][y][x];
                        if (vectorGrid.isFull==0) {
                            vectorGrid.isFull = 1;
                            vectorGrid.vector = dir;//planeNormal;
                            ComputeTensorFromOrient(vectorGrid.tensor, vectorGrid.vector);
                        } else {
                            if (dot(vectorGrid.vector, dir)<0.0f) {
                                //vectorGrid.vector = - vectorGrid.vector;
                            }
                        }
                    }
                }
            }
        }
    }

   // CVectorGrid*** tempVectorField = My3dMalloc<CVectorGrid>(m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	std::vector<std::vector<std::vector<CVectorGrid>>> tempVectorField;
	my3dVectorMalloc(tempVectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                tempVectorField[i][j][k] = m_vectorField[i][j][k];
            }
        }
    }

    int numIters = max(max(m_gridNum[0], m_gridNum[1]), m_gridNum[2])*2;
    cout<<"Iter Num: "<<numIters<<endl;
    int curr = 0;
    float inv6 = 1.0f/6.0f;
    //CVectorGrid**** os[2] = {&m_vectorField, &tempVectorField};
//	std::vector<std::vector<std::vector<std::vector<CVectorGrid>*>>> os = { &m_vectorField,&tempVectorField };
	std::vector< std::vector<std::vector<std::vector<CVectorGrid>>>* > os = { &m_vectorField,&tempVectorField };
    for (int iter = 0; iter < numIters; iter++) {
#pragma omp parallel for
        for (int z=0; z<m_gridNum[2]; z++) {
            for (int y=0; y<m_gridNum[1]; y++) {
                for (int x=0; x<m_gridNum[0]; x++) {
                    CVectorGrid& vectorGrid = (*os[1-curr])[z][y][x];
                    if (vectorGrid.isFull == 0) {
                        int x0 = (x-1>=0 ? x-1 : 0);
                        int x1 = (x+1<m_gridNum[0] ? x+1 : m_gridNum[0]-1);
                        int y0 = (y-1>=0 ? y-1 : 0);
                        int y1 = (y+1<m_gridNum[1] ? y+1 : m_gridNum[1]-1);
                        int z0 = (z-1>=0 ? z-1 : 0);
                        int z1 = (z+1<m_gridNum[2] ? z+1 : m_gridNum[2]-1);

//                        (*os[1-curr])[z][y][x].tensor = ((*os[curr])[z][y][x0].tensor + (*os[curr])[z][y][x1].tensor +
//                                                (*os[curr])[z][y0][x].tensor + (*os[curr])[z][y1][x].tensor +
//                                                (*os[curr])[z0][y][x].tensor + (*os[curr])[z1][y][x].tensor)*inv6;

                        (*os[1-curr])[z][y][x].vector = ((*os[curr])[z][y][x0].vector + (*os[curr])[z][y][x1].vector +
                                                         (*os[curr])[z][y0][x].vector + (*os[curr])[z][y1][x].vector +
                                                         (*os[curr])[z0][y][x].vector + (*os[curr])[z1][y][x].vector)*inv6;

                    } else {
                        (*os[1-curr])[z][y][x] = (*os[curr])[z][y][x];
                    }
                }
            }
        }
        curr = 1-curr;
        //cout<<iter<<endl;
    }
    if (curr == 1) {
        for (int i=0; i<m_gridNum[2]; i++) {
            for (int j=0; j<m_gridNum[1]; j++) {
                for (int k=0; k<m_gridNum[0]; k++) {
                    m_vectorField[i][j][k] = tempVectorField[i][j][k];
                }
            }
        }
    }


    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                CVectorGrid& vectorGrid = m_vectorField[i][j][k];
                if (vectorGrid.isFull == 0) {
                    //ComputeOrientFromTensor(vectorGrid.tensor, vectorGrid.vector);
                }
            }
        }
    }

    int diffusionNum = 20.0f/m_gridSize;
    for (int z=0; z<m_gridNum[2]; z++) {
        for (int y=0; y<m_gridNum[1]; y++) {
            for (int x=0; x<m_gridNum[0]; x++) {
                CVectorGrid& vectorGrid = m_vectorField[z][y][x];
                if (vectorGrid.isFull == 1) {
                    Vec3i gridId = Vec3i(x, y, z);
                    for (int k=-diffusionNum; k<=diffusionNum; k++) {
                        for (int j=-diffusionNum; j<=diffusionNum; j++) {
                            for (int i=-diffusionNum; i<=diffusionNum; i++) {
                                Vec3i recGridId = Vec3i(gridId[0]+i, gridId[1]+j, gridId[2]+k);
                                if (recGridId[0]>=0 && recGridId[0]<m_gridNum[0] &&
                                    recGridId[1]>=0 && recGridId[1]<m_gridNum[1] &&
                                    recGridId[2]>=0 && recGridId[2]<m_gridNum[2]) {
                                    tempVectorField[recGridId[2]][recGridId[1]][recGridId[0]].isFull = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (int z=0; z<m_gridNum[2]; z++) {
        for (int y=0; y<m_gridNum[1]; y++) {
            for (int x=0; x<m_gridNum[0]; x++) {
                m_vectorField[z][y][x].isFull = 1;
            }
        }
    }

//    for (int z=0; z<m_gridNum[2]; z++) {
//        for (int y=0; y<m_gridNum[1]; y++) {
//            for (int x=0; x<m_gridNum[0]; x++) {
//                Vec3f pos = GetGridCenter(Vec3i(x, y, z));
//                CHairSample sample;
//                sample.SetPos(pos);
//                if (volume->IsSampleInVolume(sample, 3)) {
//                    m_vectorField[z][y][x].isFull = 1;
//                }
//            }
//        }
//    }
 //   delete tempVectorField;

    for (int i=0; i<m_gridNum[2]; i++) {
        for (int j=0; j<m_gridNum[1]; j++) {
            for (int k=0; k<m_gridNum[0]; k++) {
                CVectorGrid& vectorGrid = m_vectorField[i][j][k];
                if (vectorGrid.isFull == 0) {
                    vectorGrid.vector = Vec3f(0.0f, 0.0f, 0.0f);
                }
            }
        }
    }
}

void CHairSmoother::GenerateHairStrands(int num)
{
    //(Hair Photobooth: Geometric and Photometric Acquisition of Real Hairstyles[Section 3.4])
    //(Example-Based Hair Geometry Synthesis[Section 5.3])
    int coreNum = omp_get_num_procs();
    CHairStrands* recStrands;
    recStrands = new CHairStrands[coreNum];
#pragma omp parallel for
    //for (int i=0; i<1000; i++) {
    //    cout<<i<<" finished"<<endl;
    for (int i=0; i<m_inputStrands->GetNumOfStrands(); i++) {
        CHairSample& sample = m_inputStrands->GetStrand(i).GetSample(0);
        CHairSample sampleOnScalp = m_scalp->GetTheNearestSampleOnScalp(sample);
        if (1){//( dist(sample.GetPos(), sampleOnScalp.GetPos()) < 10.0f ) {
            CHairStrand strand;
            Vec3f rootPos = sample.GetPos();
            Vec3f lastDir;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            int iteration = 0;
            int iterationMax = 2000;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
                if (dot(lastDir, vector) < 0.0f)
                    vector = -vector;
                if (iteration>20 && dot(lastDir, vector) < 0.0f)
                {
                    break;
                }
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            recStrands[coreId].AddStrand(strand);
        }
        for (int j=1; j<num; j++) {
            CHairStrand strand;
            Vec3f rootPos = sample.GetPos();
            rootPos += Vec3f(rand()/(float)RAND_MAX, rand()/(float)RAND_MAX, rand()/(float)RAND_MAX) * m_gridSize *0.5f;
            Vec3f lastDir;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            int iteration = 0;
            int iterationMax = 1000;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
                if (vector[0]==0.0f && vector[1]==0.0f && vector[2]==0.0f) {
                    break;
                }
                if (dot(lastDir, vector) < 0.0f)
                    vector = -vector;
                if (iteration>20 && dot(lastDir, vector) < 0.7f)
                    break;
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            recStrands[coreId].AddStrand(strand);
        }
    }
    for (int i=0; i<coreNum; i++) {
        for (int j=0; j<recStrands[i].GetNumOfStrands(); j++) {
            CHairStrand strand = recStrands[i].GetStrand(j);
            strand.ResampleStrand(100);
            m_smoothStrands->AddStrand(strand);
        }
    }
}

void CHairSmoother::GeneratePonytailStrandsfromRootPositions()
{
//    constrainedBox.push_back(Vec3f(-27.573f, 1689.292f, -151.416f));
//    constrainedBox.push_back(Vec3f(27.427f, 1749.292f, -121.416f));
//
//    constrainedBox.push_back(Vec3f(-37.573f, 1780.292f, -179.416f));
//    constrainedBox.push_back(Vec3f(37.427f, 1840.292f, -119.416f));
//
//    constrainedBox.push_back(Vec3f(-82.215f, 1873.12f, -93.107f));
//    constrainedBox.push_back(Vec3f(-32.215f, 1913.12f, -43.107f));
//    constrainedBox.push_back(Vec3f(22.215f, 1873.12f, -93.107f));
//    constrainedBox.push_back(Vec3f(72.215f, 1913.12f, -43.107f));
//
//    constrainedBox.push_back(Vec3f(-34.573f, 1870.292f, -117.416f));
//    constrainedBox.push_back(Vec3f(44.427f, 1930.292f, -57.416f));

    constrainedBox.push_back(Vec3f(-34.573f, 1799.292f, -153.416f));
    constrainedBox.push_back(Vec3f(34.427f, 1859.292f, -93.416f));




    int coreNum = omp_get_num_procs();
    CHairStrands* recStrands;
    recStrands = new CHairStrands[coreNum];
#pragma omp parallel for
    for (int i = 0; i < rootPostions.size(); i++)
    {
        Vec3f rootPos = rootPostions[i];
        Vec3f rootNormal = rootNormals[i];
        if (1)
        {
            Vec3f lastDir;
            CHairStrand strand;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            lastDir = InterpolateVector(rootPos);
            int iteration = 0;
            int iterationMax = 500;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
                if (iteration>20 && dot(lastDir, vector) < 0.7f)
                {
                    bool flag = true;
                    for (int k = 0; k < constrainedBox.size(); k+=2)
                    {
                        if ((rootPos[0]>constrainedBox[k][0] && rootPos[0]<constrainedBox[k+1][0] &&
                             rootPos[1]>constrainedBox[k][1] && rootPos[1]<constrainedBox[k+1][1] &&
                             rootPos[2]>constrainedBox[k][2] && rootPos[2]<constrainedBox[k+1][2]))
                        {
                            flag = false;
                            break;
                        }
                    }
                    if (flag == true)
                    {
                        break;
                    }
                }
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            if (strand.GetNumOfSamples()>25) {
                if (strand.GetNumOfSamples()>50)
                {
                    int r = rand()%10;
                    for (int k=0; k<r; k++)
                    {
                        strand.RemoveSample(strand.GetNumOfSamples()-1);
                    }
                }
                recStrands[coreId].AddStrand(strand);
            } else {
                strand.Clear();
                CHairSample sample;
                sample.SetPos(rootPostions[i]);
                strand.AddSample(sample);
                recStrands[coreId].AddStrand(strand);
            }
        }
    }
    for (int i=0; i<coreNum; i++) {
        for (int j=0; j<recStrands[i].GetNumOfStrands(); j++) {
            CHairStrand strand = recStrands[i].GetStrand(j);
            strand.ResampleStrand(100);
            m_smoothStrands->AddStrand(strand);
        }
    }
}

void CHairSmoother::GenerateHairStrandsfromRootPositions()
{
    int coreNum = omp_get_num_procs();
    CHairStrands* recStrands;
    recStrands = new CHairStrands[coreNum];
#pragma omp parallel for
    for (int i = 0; i < rootPostions.size(); i++)
    {
        Vec3f rootPos = rootPostions[i];
        Vec3f rootNormal = rootNormals[i];
        if (1)
        {
            Vec3f lastDir;
            CHairStrand strand;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            lastDir = InterpolateVector(rootPos);
            int iteration = 0;
            int iterationMax = 500;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
                if (dot(lastDir, vector) < 0.0f)
                    vector = -vector;
                if (iteration>20 && dot(lastDir, vector) < 0.5f)
                {
                    break;
                }
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            if (strand.GetNumOfSamples()>25) {
                if (strand.GetNumOfSamples()>50)
                {
                    int r = rand()%10;
                    for (int k=0; k<r; k++)
                    {
                        strand.RemoveSample(strand.GetNumOfSamples()-1);
                    }
                }
                recStrands[coreId].AddStrand(strand);
            } else {
                recStrands[coreId].AddStrand(strand);
                //strand.Clear();
                //CHairSample sample;
                //sample.SetPos(rootPostions[i]);
                //strand.AddSample(sample);
                //recStrands[coreId].AddStrand(strand);
				//recStrands[coreId].AddStrand(strand);
            }
        }
    }
    for (int i=0; i<coreNum; i++) {
        for (int j=0; j<recStrands[i].GetNumOfStrands(); j++) {
            CHairStrand strand = recStrands[i].GetStrand(j);
            strand.ResampleStrand(100);
            m_smoothStrands->AddStrand(strand);
        }
    }
}

void CHairSmoother::GenerateHairStrandsBlank(int num)
{
    m_inputStrands->RemoveZeroStrands();
    vector<Vec3f> oldRoots(m_inputStrands->GetNumOfStrands());
    for (int i = 0; i < oldRoots.size(); i++)
    {
        oldRoots[i] = m_inputStrands->GetStrand(i).GetSample(0).GetPos();
    }
    int coreNum = omp_get_num_procs();
    CHairStrands* recStrands;
    recStrands = new CHairStrands[coreNum];
#pragma omp parallel for
    for (int i=0; i<num; i++) {
        Vec3f rootPos, rootNormal;
        m_scalp->GetRandomPointOnScalp(rootPos, rootNormal);
        bool grow = true;
        for (int j = 0; j < oldRoots.size(); j++)
        {
            if (dist(rootPos, oldRoots[j]) < 1)
            {
                grow = false;
                break;
            }
        }
        if (grow == false)
            continue;
        if (1) {
            Vec3f lastDir;
            CHairStrand strand;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            lastDir = InterpolateVector(rootPos);
            int iteration = 0;
            int iterationMax = 500;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
                if (dot(lastDir, vector) < 0.0f)
                    vector = -vector;
                if (iteration>5 && dot(lastDir, vector) < 0.7f)
                    break;
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            if (strand.GetNumOfSamples()>10) {
                if (strand.GetNumOfSamples()>50)
                {
                    int r = rand()%10;
                    for (int k=0; k<r; k++)
                    {
                        strand.RemoveSample(strand.GetNumOfSamples()-1);
                    }
                }
                recStrands[coreId].AddStrand(strand);
            }
        }
    }
    CHairStrands* strands = new CHairStrands();
    for (int i=0; i<coreNum; i++) {
        for (int j=0; j<recStrands[i].GetNumOfStrands(); j++) {
            CHairStrand strand = recStrands[i].GetStrand(j);
            m_smoothStrands->AddStrand(strand);
        }
    }
    delete strands;
}

void CHairSmoother::GenerateUniformDistributionHairStrands(int num)
{
    int coreNum = omp_get_num_procs();
    CHairStrands* recStrands;
    recStrands = new CHairStrands[coreNum];
#pragma omp parallel for
    for (int i=0; i<num; i++) {
        Vec3f rootPos, rootNormal;
        m_scalp->GetRandomPointOnScalp(rootPos, rootNormal);
        if (1) {
            Vec3f lastDir;
            CHairStrand strand;
            CHairSample rootSample;
            rootSample.SetPos(rootPos);
            strand.AddSample(rootSample);
            float segmentLength = m_gridSize;
            lastDir = InterpolateVector(rootPos);
//            Vec3f dir = InterpolateVector(rootPos) + rootNormal*0.1;
//            normalize(dir);
//            rootPos += rootNormal*1.1f;
//            rootSample.SetPos(rootPos);
//            strand.AddSample(rootSample);
            int iteration = 0;
            int iterationMax = 1500;
            while (iteration<iterationMax) {
                Vec3f vector = InterpolateVector(rootPos);
//                Vec3i gridId = GetGridId(rootPos);
//                Vec3f vector ;
//                if (gridId[0] == -1) {
//                    vector = Vec3f(0.0f, 0.0f, 0.0f);
//                } else {
//                    vector = m_vectorField[gridId[2]][gridId[1]][gridId[0]].vector;
//                }
//                if (vector[0]==0.0f && vector[1]==0.0f && vector[2]==0.0f) {
//                    break;
//                }
                if (dot(lastDir, vector) < 0.0f)
                    vector = -vector;
                if (iteration>10 && dot(lastDir, vector) < 0.7f)
                    break;
                lastDir = vector;
                rootPos = rootPos + vector*segmentLength;
                if (rootPos[0]<m_minAABB[0] && rootPos[0]>m_maxAABB[0] &&
                    rootPos[1]<m_minAABB[1] && rootPos[1]>m_maxAABB[1] &&
                    rootPos[2]<m_minAABB[2] && rootPos[2]>m_maxAABB[2]) {
                    break;
                }
                CHairSample recSample;
                recSample.SetPos(rootPos);
                strand.AddSample(recSample);
                iteration++;
            }
            int coreId = omp_get_thread_num();
            if (strand.GetNumOfSamples()>15) {
                if (strand.GetNumOfSamples()>50)
                {
                    int r = rand()%10;
                    for (int k=0; k<r; k++)
                    {
                        strand.RemoveSample(strand.GetNumOfSamples()-1);
                    }
                }
                recStrands[coreId].AddStrand(strand);
            }
        }
    }
    CHairStrands* strands = new CHairStrands();
    for (int i=0; i<coreNum; i++) {
        for (int j=0; j<recStrands[i].GetNumOfStrands(); j++) {
            CHairStrand strand = recStrands[i].GetStrand(j);
            //strand.ResampleStrand(100);
            m_smoothStrands->AddStrand(strand);
        }
    }
    //strands->RandomSelect(m_smoothStrands);
    delete strands;
}

void CHairSmoother::FillTangentsField(Vec3f startPos, Vec3f endPos, int flag)
{
    Vec3f tangent = endPos-startPos;
    if (tangent==Vec3f(0.0f, 0.0f, 0.0f))
        return;
    normalize(tangent);
    Vec3i startGridId = GetGridId(startPos);
    Vec3i endGridId = GetGridId(endPos);
    if (startGridId[0] == -1 || endGridId[0] == -1)
        return;
    Vec3i gridOrientation;
    for (int k=0; k<3; k++) {
        if (endGridId.v[k] == startGridId.v[k]) {
            gridOrientation.v[k] = 0;
        } else if (endGridId.v[k] > startGridId.v[k]) {
            gridOrientation.v[k] = 1;
        } else {
            gridOrientation.v[k] = -1;
        }
    }

    if (gridOrientation[0]==0 && gridOrientation[1]==0 && gridOrientation[2]==0) {
        m_tangentsField[startGridId[2]][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
        if (flag != -1)
        {
            m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], startGridId[2]));
        }
    } else if (gridOrientation[0]==0 && gridOrientation[1]==0) {
        int l = startGridId[2];
        do {
            m_tangentsField[l][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], l));
            }
            l = l+gridOrientation[2];
            if ((gridOrientation[2]==1)?l>endGridId[2]:l<endGridId[2]) {
                break;
            }
        } while (l != endGridId[2]);
    } else if (gridOrientation[1]==0 && gridOrientation[2]==0) {
        int l = startGridId[0];
        do {
            m_tangentsField[startGridId[2]][startGridId[1]][l].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(l, startGridId[1], startGridId[2]));
            }
            l = l+gridOrientation[0];
            if ((gridOrientation[0]==1)?l>endGridId[0]:l<endGridId[0]) {
                break;
            }
        } while (l != endGridId[0]);
    } else if (gridOrientation[2]==0 && gridOrientation[0]==0) {
        int l = startGridId[1];
        do {
            m_tangentsField[startGridId[2]][l][startGridId[0]].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(startGridId[0], l, startGridId[2]));
            }
            l = l+gridOrientation[1];
            if ((gridOrientation[1]==1)?l>endGridId[1]:l<endGridId[1]) {
                break;
            }
        } while (l != endGridId[1]);
    } else if (gridOrientation[0]==0) {
        int l = startGridId[1];
        int m = startGridId[2];
        m_tangentsField[startGridId[2]][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
        if (flag != -1)
        {
            m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], startGridId[2]));
        }
        while ( !(l==endGridId[1] && m==endGridId[2]) ) {
            float yWall = gridOrientation[1]==1?(l+1)*m_gridSize+m_minAABB[1]:l*m_gridSize+m_minAABB[1];
            float zWall = gridOrientation[2]==1?(m+1)*m_gridSize+m_minAABB[2]:m*m_gridSize+m_minAABB[2];
            float uy = (yWall-startPos[1])/tangent[1];
            float uz = (zWall-startPos[2])/tangent[2];
            if (uy == uz) {
                l = l+gridOrientation[1];
                m = m+gridOrientation[2];
            } else if (uy < uz) {
                l = l+gridOrientation[1];
            } else {
                m = m+gridOrientation[2];
            }
            m_tangentsField[m][l][startGridId[0]].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(startGridId[0], l, m));
            }
            if ((gridOrientation[1]==1)?l>endGridId[1]:l<endGridId[1] ||
                (gridOrientation[2]==1)?m>endGridId[2]:m<endGridId[2]) {
                break;
            }
        }
    } else if (gridOrientation[1]==0) {
        int l = startGridId[0];
        int m = startGridId[2];
        m_tangentsField[startGridId[2]][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
        if (flag != -1)
        {
            m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], startGridId[2]));
        }
        while ( !(l==endGridId[0] && m==endGridId[2]) ) {
            float xWall = gridOrientation[0]==1?(l+1)*m_gridSize+m_minAABB[0]:l*m_gridSize+m_minAABB[0];
            float zWall = gridOrientation[2]==1?(m+1)*m_gridSize+m_minAABB[2]:m*m_gridSize+m_minAABB[2];
            float ux = (xWall-startPos[0])/tangent[0];
            float uz = (zWall-startPos[2])/tangent[2];
            if (ux == uz) {
                l = l+gridOrientation[0];
                m = m+gridOrientation[2];
            } else if (ux < uz) {
                l = l+gridOrientation[0];
            } else {
                m = m+gridOrientation[2];
            }
            m_tangentsField[m][startGridId[1]][l].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(l, startGridId[1], m));
            }
            if ( (gridOrientation[0]==1)?l>endGridId[0]:l<endGridId[0] ||
                (gridOrientation[2]==1)?m>endGridId[2]:m<endGridId[2]) {
                break;
            }
        }
    } else if (gridOrientation[2]==0) {
        int l = startGridId[0];
        int m = startGridId[1];
        m_tangentsField[startGridId[2]][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
        if (flag != -1)
        {
            m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], startGridId[2]));
        }
        while ( !(l==endGridId[0] && m==endGridId[1]) ) {
            float xWall = gridOrientation[0]==1?(l+1)*m_gridSize+m_minAABB[0]:l*m_gridSize+m_minAABB[0];
            float yWall = gridOrientation[1]==1?(m+1)*m_gridSize+m_minAABB[1]:m*m_gridSize+m_minAABB[1];
            float ux = (xWall-startPos[0])/tangent[0];
            float uy = (yWall-startPos[1])/tangent[1];
            if (ux == uy) {
                l = l+gridOrientation[0];
                m = m+gridOrientation[1];
            } else if (ux < uy) {
                l = l+gridOrientation[0];
            } else {
                m = m+gridOrientation[1];
            }
            m_tangentsField[startGridId[2]][m][l].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(l, m, startGridId[2]));
            }
            if ( (gridOrientation[0]==1)?l>endGridId[0]:l<endGridId[0] ||
                (gridOrientation[1]==1)?m>endGridId[1]:m<endGridId[1]) {
                break;
            }
        }
    } else {
        int l = startGridId[0];
        int m = startGridId[1];
        int n = startGridId[2];
        m_tangentsField[startGridId[2]][startGridId[1]][startGridId[0]].tangents.push_back(tangent);
        if (flag != -1)
        {
            m_filledGrids[flag].push_back(Vec3i(startGridId[0], startGridId[1], startGridId[2]));
        }
        while ( !(l==endGridId[0] && m==endGridId[1] && n==endGridId[2]) ) {
            float xWall = gridOrientation[0]==1?(l+1)*m_gridSize+m_minAABB[0]:l*m_gridSize+m_minAABB[0];
            float yWall = gridOrientation[1]==1?(m+1)*m_gridSize+m_minAABB[1]:m*m_gridSize+m_minAABB[1];
            float zWall = gridOrientation[2]==1?(n+1)*m_gridSize+m_minAABB[2]:n*m_gridSize+m_minAABB[2];
            float ux = (xWall-startPos[0])/tangent[0];
            float uy = (yWall-startPos[1])/tangent[1];
            float uz = (zWall-startPos[2])/tangent[2];
            if (ux==uy && uy==uz) {
                l = l+gridOrientation[0];
                m = m+gridOrientation[1];
                n = n+gridOrientation[2];
            } else if (ux==uy) {
                if (ux<uz) {
                    l = l+gridOrientation[0];
                    m = m+gridOrientation[1];
                } else {
                    n = n+gridOrientation[2];
                }
            } else if (ux==uz) {
                if (ux<uy) {
                    l = l+gridOrientation[0];
                    n = n+gridOrientation[2];
                } else {
                    m = m+gridOrientation[1];
                }
            } else if (uy==uz) {
                if (uy<ux) {
                    m = m+gridOrientation[1];
                    n = n+gridOrientation[2];
                } else {
                    l = l+gridOrientation[0];
                }
            } else {
                if (ux<uy && ux<uz) {
                    l = l+gridOrientation[0];
                } else if (uy<uz) {
                    m = m+gridOrientation[1];
                } else {
                    n = n+gridOrientation[2];
                }
            }
            if ( (gridOrientation[0]==1)?l>endGridId[0]:l<endGridId[0] ||
                 (gridOrientation[1]==1)?m>endGridId[1]:m<endGridId[1] ||
                (gridOrientation[2]==1)?n>endGridId[2]:n<endGridId[2]) {
                break;
            }
            m_tangentsField[n][m][l].tangents.push_back(tangent);
            if (flag != -1)
            {
                m_filledGrids[flag].push_back(Vec3i(l, m, n));
            }
        }
    }
}

Vec3f CHairSmoother::InterpolateVector(Vec3f pos)
{
    Vec3f vector = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3i gridId = GetGridId(pos);
    if (gridId[0] == -1)
        return vector;
    Vec3f center = GetGridCenter(gridId);
    if ( dist(pos, center)<1e-4 ) {
        vector = m_vectorField[gridId[2]][gridId[1]][gridId[0]].vector;
    } else {
        float totalWeight = 0.0f;
        Vec3f totalVector = Vec3f(0.0f, 0.0f, 0.0f);
        for (int k=-1; k<=1; k++) {
            for (int j=-1; j<=1; j++) {
                for (int i=-1; i<=1; i++) {
                    Vec3i recGridId = Vec3i(gridId[0]+i, gridId[1]+j, gridId[2]+k);
                    if (recGridId[0]>=0 && recGridId[0]<m_gridNum[0] &&
                        recGridId[1]>=0 && recGridId[1]<m_gridNum[1] &&
                        recGridId[2]>=0 && recGridId[2]<m_gridNum[2]) {
                        if (m_vectorField[recGridId[2]][recGridId[1]][recGridId[0]].isFull != 0) {
                            Vec3f recCenter = GetGridCenter(recGridId);
                            float weight = 1.0f/dist(pos, recCenter);
                            totalWeight += weight;
                            totalVector = totalVector + weight*m_vectorField[recGridId[2]][recGridId[1]][recGridId[0]].vector;
                        }
                    }
                }
            }
        }
        if (totalWeight > 0.0)
            vector = totalVector/totalWeight;
    }
    return vector;
}

//T[z][y][x]
template<class T>
T*** CHairSmoother::My3dMalloc(int xSize, int ySize, int zSize)
{
    T*** array = NULL;
    size_t zHeader = zSize * sizeof(T**);
    size_t yHeader = zSize * ySize * sizeof(T*);
    size_t size = zHeader + yHeader + sizeof(T)*xSize*ySize*zSize;
    array = (T***)malloc(size);
    T** buf = (T**)(array+zSize);
    for (int i=0; i<zSize; i++) {
        array[i] = buf + i*ySize;
        T* buf2 = (T*)(array[0]+ySize*zSize);
        for (int j=0; j<ySize; j++) {
            array[i][j] = buf2 + (i*ySize+j)*xSize;
        }
    }
    return array;
}

Vec3i CHairSmoother::GetGridId(Vec3f pos)
{
    if (pos[0]<m_minAABB[0] || pos[0]>m_maxAABB[0] ||
        pos[1]<m_minAABB[1] || pos[1]>m_maxAABB[1] ||
        pos[2]<m_minAABB[2] || pos[2]>m_maxAABB[2]) {
        return Vec3i(-1, -1, -1);
    }
    return Vec3i( (int)((pos[0]-m_minAABB[0])/m_gridSize)>=m_gridNum[0]?(int)((pos[0]-m_minAABB[0])/m_gridSize)-1:(int)((pos[0]-m_minAABB[0])/m_gridSize),
                 (int)((pos[1]-m_minAABB[1])/m_gridSize)>=m_gridNum[1]?(int)((pos[1]-m_minAABB[1])/m_gridSize)-1:(int)((pos[1]-m_minAABB[1])/m_gridSize),
                 (int)((pos[2]-m_minAABB[2])/m_gridSize)>=m_gridNum[2]?(int)((pos[2]-m_minAABB[2])/m_gridSize)-1:(int)((pos[2]-m_minAABB[2])/m_gridSize));
}

Vec3f CHairSmoother::GetGridCenter(Vec3i gridId)
{
    Vec3f minAABB = Vec3f(gridId[0]*m_gridSize+m_minAABB[0],
                          gridId[1]*m_gridSize+m_minAABB[1],
                          gridId[2]*m_gridSize+m_minAABB[2]);
    Vec3f maxAABB = Vec3f((gridId[0]+1)*m_gridSize+m_minAABB[0],
                          (gridId[1]+1)*m_gridSize+m_minAABB[1],
                          (gridId[2]+1)*m_gridSize+m_minAABB[2]);
    Vec3f center = (maxAABB+minAABB)/2.0f;
    return center;
}

Vec3f CHairSmoother::GetGridVector(Vec3i gridId)
{
    if (gridId[0] >= 0 && gridId[0] < m_gridNum[0] &&
        gridId[1] >= 0 && gridId[1] < m_gridNum[1] &&
        gridId[2] >= 0 && gridId[2] < m_gridNum[2])
    {
        return m_vectorField[gridId[2]][gridId[1]][gridId[0]].vector;
    }
    else
    {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
}

CVectorGrid CHairSmoother::GetGrid(Vec3i gridId)
{
    if (gridId[0] >= 0 && gridId[0] < m_gridNum[0] &&
        gridId[1] >= 0 && gridId[1] < m_gridNum[1] &&
        gridId[2] >= 0 && gridId[2] < m_gridNum[2])
    {
        return m_vectorField[gridId[2]][gridId[1]][gridId[0]];
    }
    else
    {
        CVectorGrid grid;
        return grid;
    }
}

void CHairSmoother::SetGridVector(Vec3i gridId, Vec3f vector)
{
    if (gridId[0] >= 0 && gridId[0] < m_gridNum[0] &&
        gridId[1] >= 0 && gridId[1] < m_gridNum[1] &&
        gridId[2] >= 0 && gridId[2] < m_gridNum[2])
    {
        m_vectorField[gridId[2]][gridId[1]][gridId[0]].vector = vector;
    }
}

void CHairSmoother::SetGrid(Vec3i gridId, CVectorGrid grid)
{
    if (gridId[0] >= 0 && gridId[0] < m_gridNum[0] &&
        gridId[1] >= 0 && gridId[1] < m_gridNum[1] &&
        gridId[2] >= 0 && gridId[2] < m_gridNum[2])
    {
        m_vectorField[gridId[2]][gridId[1]][gridId[0]] = grid;
    }
}


void CHairSmoother::ComputeOrientFromTensor(Tensor& tensor, Vec3f& orient)
{
    if (fabs(tensor.cov[0])<1e-6 && fabs(tensor.cov[1])<1e-6 && fabs(tensor.cov[2])<1e-6 && fabs(tensor.cov[3])<1e-6
        && fabs(tensor.cov[4])<1e-6 && fabs(tensor.cov[5])<1e-6) {
        orient = Vec3f(0.0f, 0.0f, 0.0f);
        return;
    }
    float cov[3][3] = { {tensor.cov[0], tensor.cov[1], tensor.cov[2]},
        {tensor.cov[1], tensor.cov[3], tensor.cov[4]},
        {tensor.cov[2], tensor.cov[4], tensor.cov[5]}};
    float* retval = new float[3];
    ::max_eigen_vec<3, float>(cov, (float*) retval);
    orient = Vec3f(retval[0], retval[1], retval[2]);
    if (mag(orient)<1e-6)
        orient = Vec3f(0.0f, 0.0f, 0.0f);
    else
        normalize(orient);
    delete retval;
}

void CHairSmoother::ComputeTensorFromOrient(Tensor& tensor, Vec3f& orient)
{
    tensor.cov[0] = orient[0] * orient[0];
    tensor.cov[1] = orient[0] * orient[1];
    tensor.cov[2] = orient[0] * orient[2];
    tensor.cov[3] = orient[1] * orient[1];
    tensor.cov[4] = orient[1] * orient[2];
    tensor.cov[5] = orient[2] * orient[2];
}

bool CHairSmoother::DumpRootPosition(const char* fileName, int num)
{
    vector<Vec3f> rootsPos;
    vector<Vec3f> rootsNorm;
    while (rootsPos.size() < num)
    {
        Vec3f rootPos, rootNormal;
        m_scalp->GetRandomPointOnScalp(rootPos, rootNormal);
        bool flag = true;
        for (int i = 0; i < rootsPos.size(); i++)
        {
            if (rootPos[0] == rootsPos[i][0] && rootPos[1] == rootsPos[i][1] && rootPos[2] == rootsPos[i][2])
            {
                flag = false;
                break;
            }
        }
        if (flag == true)
        {
            rootsPos.push_back(rootPos);
            rootsNorm.push_back(rootNormal);
        }
    }
    ofstream fout(fileName);
    fout << rootsPos.size() << endl;
    for (int i = 0; i < rootsPos.size(); i++)
    {
        fout<<rootsPos[i][0]<<" "<<rootsPos[i][1]<<" "<<rootsPos[i][2]<<endl;
        fout<<rootsNorm[i][0]<<" "<<rootsNorm[i][1]<<" "<<rootsNorm[i][2]<<endl;
    }
    fout.close();
    return true;
}

bool CHairSmoother::LoadRootPosition(const char* fileName)
{
    ifstream fin(fileName);
    int num;
    fin>>num;
    rootPostions.resize(num);
    rootNormals.resize(num);
    for (int i = 0; i < num; i++)
    {
        float x, y, z;
        fin >> x >> y >> z;
        rootPostions[i] = Vec3f(x, y, z);
        fin >> x >> y >> z;
        rootNormals[i] = Vec3f(x, y, z);
    }
    fin.close();
    return true;
}

bool CHairSmoother::DumpOrientationField(const char* fileName)
{
    FILE *out;
    out = fopen(fileName, "w+");
    cout<<"Dump Orientation Field to "<<fileName<<endl;
    fprintf(out, "%f\n", m_gridSize);
    fprintf(out, "%d %d %d\n", m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    fprintf(out, "%f %f %f\n", m_minAABB[0], m_minAABB[1], m_minAABB[2]);
    fprintf(out, "%f %f %f\n", m_maxAABB[0], m_maxAABB[1], m_maxAABB[2]);
    for (int z=0; z<m_gridNum[2]; z++) {
        for (int y=0; y<m_gridNum[1]; y++) {
            for (int x=0; x<m_gridNum[0]; x++) {
                CVectorGrid& vectorGrid = m_vectorField[z][y][x];
                fprintf(out, "%f %f %f %d\n", vectorGrid.vector[0], vectorGrid.vector[1], vectorGrid.vector[2], vectorGrid.isFull);
            }
        }
    }
    fclose(out);
    return true;
}


void CHairSmoother::compNewStrands(const std::string& fPath) {
	if (!m_smoothStrands) {
		m_smoothStrands = new CHairStrands();
	}
	m_smoothStrands->Clear();
	GenerateHairStrandsfromRootPositions();
	for (int i = 0; i < m_smoothStrands->GetNumOfStrands(); ++i) {
		auto& s = m_smoothStrands->GetStrand(i);
		for (int j = 0; j < s.GetNumOfSamples(); ++j) {
			auto tPos = s.GetSample(j).GetPos() / 10.0f;
			//tPos[2] = -tPos[2];
			s.GetSample(j).SetPos(tPos);
		}
	}

	m_smoothStrands->DumpStrandsAsBinaryData(fPath);
}

bool CHairSmoother::init(const std::string rootPositionFile) {
	//loadScalp(calPath);
	LoadRootPosition(rootPositionFile.c_str());
	return true;
}
bool CHairSmoother::loadScalp(const std::string& scalPath) {
	if (m_scalp) {
		delete m_scalp;
		m_scalp = nullptr;
	}
	m_scalp = new CHairScalp(scalPath.c_str());
	return true;
}

bool CHairSmoother::setGridSize(float grideSize,const std::vector<float>& minAABB, const std::vector<float>& maxAABB) {
    m_gridSize = grideSize;
    m_minAABB = Vec3f(minAABB[0],minAABB[1],minAABB[2]);
    m_maxAABB = Vec3f(maxAABB[0],maxAABB[1],maxAABB[2]);
	return true;
}
bool CHairSmoother::testNdArray(P3D::PNdArrayF arrF, P3D::PNdArrayI arrI) {
	std::cout << "arrF is " << arrF.shape()[0] << std::endl;
	std::cout << "arrI is " << arrI.shape()[0] << std::endl;
	return true;
}
// input size is w*h*d*c grid size is
bool CHairSmoother::setOrientationField(P3D::PNdArrayF arrayV, P3D::PNdArrayF arrayF,float threshold) {
	auto tShape = arrayV.shape();
	m_gridNum[0] = tShape[0], m_gridNum[1] = tShape[1], m_gridNum[2] = tShape[2];
	if (m_vectorField.empty()) {
		my3dVectorMalloc(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	}
	if (m_vectorField.size() != m_gridNum[2] || m_vectorField[0].size() != m_gridNum[1] || m_vectorField[0][0].size() != m_gridNum[0]) {
		my3dVectorMalloc(m_vectorField, m_gridNum[0], m_gridNum[1], m_gridNum[2]);
	}

	for (int z = 0; z < m_gridNum[2]; ++z) {
		for (int y = 0; y < m_gridNum[1]; ++y) {
			for (int x = 0; x < m_gridNum[0]; ++x) {
				auto& vectorGrid = m_vectorField[z][m_gridNum[1] -  y-1][x];
				vectorGrid.vector[0] = arrayV[{x,y,z, 0}];
				vectorGrid.vector[1] = arrayV[{x, y, z, 1}];
				vectorGrid.vector[2] = arrayV[{x, y, z, 2}];
				normalize(vectorGrid.vector);
				vectorGrid.isFull = arrayF[{x, y, z}] > threshold ? 1 : 0;
			}
		}
	}
	return true;

}
bool CHairSmoother::LoadOrientationField(const char* fileName)
{
    FILE *in;
    in = fopen(fileName, "r");
    fscanf(in,"%f\n",&m_gridSize);
    fscanf(in,"%d %d %d\n", &m_gridNum[0], &m_gridNum[1], &m_gridNum[2]);
    fscanf(in,"%f %f %f\n", &m_minAABB[0], &m_minAABB[1], &m_minAABB[2]);
    fscanf(in,"%f %f %f\n", &m_maxAABB[0], &m_maxAABB[1], &m_maxAABB[2]);
    my3dVectorMalloc<CVectorGrid>(m_vectorField ,m_gridNum[0], m_gridNum[1], m_gridNum[2]);
    for (int z=0; z<m_gridNum[2]; z++) {
        for (int y=0; y<m_gridNum[1]; y++) {
            for (int x=0; x<m_gridNum[0]; x++) {
                CVectorGrid& vectorGrid = m_vectorField[z][y][x];
                fscanf(in,"%f %f %f %d\n", &(vectorGrid.vector[0]), &(vectorGrid.vector[1]), &(vectorGrid.vector[2]), &(vectorGrid.isFull));
            }
        }
    }
    fclose(in);
    return true;
}

