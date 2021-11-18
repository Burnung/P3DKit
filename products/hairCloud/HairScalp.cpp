//
//  HairScalp.cpp
//  HairSyn
//
//  Created by Liwen Hu on 13-10-19.
//  Copyright (c) 2013 Liwen Hu. All rights reserved.
//

#include "HairScalp.h"
#include "HairConfig.h"
#include <iostream>
#include <fstream>

CHairScalp::CHairScalp(const char* fileName)
{
    std::fstream file;
    file.open(fileName, std::ios::in);
    if (!file)
    {
        cout << "Filename Read error!" ;
        exit(-1);
    }
    char temp[100];
    while (!file.eof())
    {
        float a[6];
        int b[6];
        file.getline(temp,100);
        if (temp[0]=='v' && temp[1]==' ')
        {
            sscanf(temp,"%*s%f %f %f %f %f %f",&a[0],&a[1],&a[2],&a[3],&a[4],&a[5]);
            m_vertice.push_back(Vec3f(a[0], a[1], a[2]));
            m_colors.push_back(Vec3f(a[3], a[4], a[5]));
        }
        else if (temp[0]=='v' && temp[1]=='n')
        {
            sscanf(temp,"%*s%f %f %f",&a[0],&a[1],&a[2]);
            m_normals.push_back(Vec3f(a[0], a[1], a[2]));
        }
        else if (temp[0] == 'f')
        {
            sscanf(temp,"%*s%d//%d %d//%d %d//%d",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5]);
            for (int n=0;n<6;n++)
                b[n]=b[n]-1;
            m_faces.push_back(Vec3i(b[0], b[2], b[4]));
            m_faces.push_back(Vec3i(b[1], b[3], b[5]));
        }
    }
    file.close();
    FindPartingLinePointIndices();
    FindScalpPointIndices();
    
    m_linePoints.resize(m_lineIds.size());
    m_lineNormals.resize(m_lineIds.size());
    for (int i = 0; i < m_lineIds.size(); i++) {
        m_linePoints[i] = m_vertice[ m_lineIds[i] ];
        m_lineNormals[i] = m_normals[ m_lineIds[i] ];
    }
        
    m_scalpPoints.resize(m_scalpIds.size());
    m_scalpNormals.resize(m_scalpIds.size());
    for (int i = 0; i < m_scalpIds.size(); i++) {
        m_scalpPoints[i] = m_vertice[ m_scalpIds[i] ];
        m_scalpNormals[i] = m_normals[ m_scalpIds[i] ];
    }
    
    ComputeDistanceToLine();
}

CHairScalp::~CHairScalp()
{

}

CHairSample CHairScalp::GetTheNearestSampleOnScalp(const CHairSample& _sample)
{
    CHairSample sampleNearest;
    Scalar distMin = std::numeric_limits<Scalar>::max();;
    int idxMin = -1;
    Vec3f pNearest;
    Vec3f nNearest;
    for ( unsigned int i=0; i<m_faces.size(); i++ )
    {
        Vec3f p1 = m_vertice[m_faces[i][0]];
        Vec3f p2 = m_vertice[m_faces[i][1]];
        Vec3f p3 = m_vertice[m_faces[i][2]];
        Vec3f n1 = m_normals[m_faces[i][0]];
        Vec3f n2 = m_normals[m_faces[i][1]];
        Vec3f n3 = m_normals[m_faces[i][2]];
        Vec3f pNearestTmp, nNearestTmp;
        Scalar distTmp = hair_math::GetPointToTriangleDistance(p1, p2, p3, _sample.GetPos(), pNearestTmp);
        if ( distTmp < distMin )
        {
            hair_math::GetNearesetPointFromPointToTriangle(p1, p2, p3, n1, n2, n3, _sample.GetPos(), pNearestTmp, nNearestTmp);
            distMin = distTmp;
            idxMin = i;
            pNearest = pNearestTmp;
            nNearest = nNearestTmp;
        }
    }
    sampleNearest.SetPos(pNearest);
    sampleNearest.SetNorm(nNearest);
    return sampleNearest;
}

void CHairScalp::GetScalpMeshFromHead(char* inputName, char* outputName)
{
    std::fstream file;
	file.open(inputName, std::ios::in);
	if (!file)
	{
		cout << "Filename Read error!" ;
		exit(-1);
	}
	char temp[100];
	vector<Vec3f> vertice;
	vector<Vec3f> normals;
    vector<Vec3f> colors;
	vector<Vec3i> faces;
	while (!file.eof())
	{
		float a[6];
		int b[6];
		file.getline(temp,100);
		if (temp[0]=='v' && temp[1]==' ')
		{
			sscanf(temp,"%*s%f %f %f %f %f %f",&a[0],&a[1],&a[2],&a[3],&a[4],&a[5]);
			vertice.push_back(Vec3f(a[0], a[1], a[2]));
            colors.push_back(Vec3f(a[3], a[4], a[5]));
		}
		else if (temp[0]=='v' && temp[1]=='n')
		{
			sscanf(temp,"%*s%f %f %f",&a[0],&a[1],&a[2]);
			normals.push_back(Vec3f(a[0], a[1], a[2]));
		}
		else if (temp[0] == 'f')
		{
			sscanf(temp,"%*s%d//%d %d//%d %d//%d",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5]);
			for (int n=0;n<6;n++)
				b[n]=b[n]-1;
            faces.push_back(Vec3i(b[0], b[2], b[4]));
            faces.push_back(Vec3i(b[1], b[3], b[5]));
		}
	}
	file.close();
    for (int i=0; i<faces.size();) {
        if ((colors[faces[i][0]][0]>0.8f && colors[faces[i][0]][1]>0.8f && colors[faces[i][0]][2]>0.8f)||
            (colors[faces[i][1]][0]>0.8f && colors[faces[i][1]][1]>0.8f && colors[faces[i][1]][2]>0.8f)||
            (colors[faces[i][2]][0]>0.8f && colors[faces[i][2]][1]>0.8f && colors[faces[i][2]][2]>0.8f)) {
            faces.erase(faces.begin()+i);
            faces.erase(faces.begin()+i);
        } else {
            i = i+2;
        }
    }
    FILE *out;
    out = fopen(outputName, "w+");
    for (int i=0; i<vertice.size(); i++) {
        fprintf(out, "v %lf %lf %lf\n", vertice[i][0], vertice[i][1], vertice[i][2]);
    }
    for (int i=0; i<normals.size(); i++) {
        fprintf(out, "vn %lf %lf %lf\n", normals[i][0], normals[i][1], normals[i][2]);
    }
    for (int i=0; i<faces.size(); i=i+2) {
        fprintf(out, "f %d//%d %d//%d %d//%d\n", faces[i][0]+1, faces[i+1][0]+1, faces[i][1]+1, faces[i+1][1]+1, faces[i][2]+1, faces[i+1][2]+1);
    }
    fclose(out);
}

void CHairScalp::FindPartingLinePointIndices()
{
    for (int i=0; i<m_vertice.size(); i++) {
        Vec3f c = m_colors[i];
        if (c[0] - c[2] > 0.5f && c[1] - c[2] > 0.7f) {
            m_lineIds.push_back(i);
        }
    }
}

void CHairScalp::FindScalpPointIndices()
{
    m_IdsInScalp.resize(m_vertice.size(), -1);
    for (int i=0; i<m_vertice.size(); i++) {
        Vec3f c = m_colors[i];
        if (c[0] - c[2] > 0.5f) {
            m_IdsInScalp[i] = (int)m_scalpIds.size();
            m_scalpIds.push_back(i);
        }
    }
}

void CHairScalp::ComputeDistanceToLine()
{
    int nodeNum = (int)m_scalpIds.size();
    float* graph = new float[nodeNum*nodeNum];
    for (int i=0; i<nodeNum; i++) {
        for (int j=0; j<nodeNum; j++) {
            graph[j+i*nodeNum] = 1e10f;
        }
    }
    for (int i=0; i<m_faces.size(); i=i+2) {
        Vec3i face = m_faces[i];
        if (m_IdsInScalp[face[0]]!=-1 && m_IdsInScalp[face[1]]!=-1) {
            int id0 = m_IdsInScalp[face[0]];
            int id1 = m_IdsInScalp[face[1]];
            graph[id0+id1*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
            graph[id1+id0*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
        }
        if (m_IdsInScalp[face[0]]!=-1 && m_IdsInScalp[face[2]]!=-1) {
            int id0 = m_IdsInScalp[face[0]];
            int id1 = m_IdsInScalp[face[2]];
            graph[id0+id1*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
            graph[id1+id0*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
        }
        if (m_IdsInScalp[face[1]]!=-1 && m_IdsInScalp[face[2]]!=-1) {
            int id0 = m_IdsInScalp[face[1]];
            int id1 = m_IdsInScalp[face[2]];
            graph[id0+id1*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
            graph[id1+id0*nodeNum] = dist(m_scalpPoints[id0], m_scalpPoints[id1]);
        }
    }
    for (int i=0; i<m_scalpIds.size(); i++) {
        int startIdx = i;
        int endIdx;
        float minDist = 1e10f;
        for (int j=0; j<m_lineIds.size(); j++) {
            float recDist = dist(m_scalpPoints[i], m_linePoints[j]);
            if (recDist<minDist) {
                minDist = recDist;
                endIdx = m_lineIds[j];
            }
        }
        endIdx = m_IdsInScalp[endIdx];
        vector<int> path;
        if (startIdx == endIdx) {
            path.push_back(startIdx);
            m_scalpPaths.push_back(path);
        } else {
            FindShortestPath(startIdx, endIdx, graph, path);
            m_scalpPaths.push_back(path);
        }
    }
    delete graph;
}

void CHairScalp::FindShortestPath(int startIdx, int endIdx, float* graph, vector<int>& path)
{
    int nodeNum = (int)m_scalpIds.size();
    float* d = new float[nodeNum];
    int* previous = new int[nodeNum];
    for (int i=0; i<nodeNum; i++) {
        d[i] = 1e11f;
        previous[i] = -1;
    }
    d[startIdx] = 0.0f;
    vector<int> s;
    vector<int> q(nodeNum);
    for (int i=0; i<nodeNum; i++) {
        q[i] = i;
    }
    while (q.size()>0) {
        int minId = 0;
        float minDist = 1e10f;
        for (int i=0; i<q.size(); i++) {
            if (d[q[i]]<minDist) {
                minId = i;
                minDist = d[q[i]];
            }
        }
        int u = q[minId];
        if (u == endIdx)
            break;
        s.push_back(u);
        q.erase(q.begin()+minId);
        for (int i=0; i<nodeNum; i++) {
            if (d[u]+graph[u+i*nodeNum]<d[i]) {
                d[i] = d[u]+graph[u+i*nodeNum];
                previous[i] = u;
            }
        }
    }
    int u = endIdx;
    path.push_back(u);
    while (u>=0) {
        u = previous[u];
        if (u>=0)
            path.insert(path.begin(), u);
    }
    delete d;
    delete previous;
}

void CHairScalp::SampleRandomPointAroundVertex(int vertId, Vec3f& point, Vec3f& normal)
{
    vector<int> adjFaces;
    for (int i=0; i<m_faces.size(); i=i+2) {
        Vec3i face = m_faces[i];
        if (face[0]==vertId || face[1]==vertId || face[2]==vertId) {
            adjFaces.push_back(i/2);
        }
    }
    int selectedFaceId = floor(adjFaces.size() * rand()/(float)RAND_MAX);
    float a0, a1, a2;
    a0 = (float)rand() / (float)RAND_MAX;
    a1 = (float)rand() / (float)RAND_MAX;
    if(a0 + a1 > 1.0f)
    {
        a0 = 1.0f - a0;
        a1 = 1.0f - a1;
    }
    a2 = 1.0f-a0-a1;
    point = (m_vertice[ m_faces[adjFaces[selectedFaceId]*2][0] ] * a0 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2][1] ] * a1 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2][2] ] * a2);
    normal= (m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][0] ] * a0 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][1] ] * a1 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][2] ] * a2);
}

int CHairScalp::IsVertexOnScalp(int vertexId)
{
    for (int i=0; i<m_scalpIds.size(); i++) {
        if (vertexId == m_scalpIds[i]) {
            return i;
        }
    }
    return -1;
}

Vec3f CHairScalp::ComputeDirection(int scalpId)
{
    Vec3f rootDirection;
    int nearestMatchScalpId = scalpId;
    vector<int> path = m_scalpPaths[ nearestMatchScalpId ];
    int bestMatchScalpId = 0;
    
    vector<int> feasiblePath;
    for (int i=bestMatchScalpId; i<path.size(); i++) {
        feasiblePath.push_back(path[i]);
    }
    
    Vec3f n, root;
    root = m_scalpPoints[scalpId];
    n = m_scalpNormals[scalpId];
    
    if (path.size()>1) {
        Vec3f rootDir = (bestMatchScalpId>0)?m_scalpPoints[path[bestMatchScalpId-1]] -  m_scalpPoints[path[bestMatchScalpId]]:
        m_scalpPoints[path[bestMatchScalpId]] -  m_scalpPoints[path[bestMatchScalpId+1]];
        normalize(rootDir);
        //rootDir = rootDir - (n*dot(n, rootDir));
        rootDir += (n*0.1f);
        normalize(rootDir);
        rootDirection = rootDir;
    } else {
        rootDirection = m_scalpNormals[path[0]];
        normalize(rootDirection);
    }
    
    return rootDirection;
}

bool CHairScalp::ComputeRootDirection(Vec3f pos, int faceIndex, Vec3f& rootDirection)
{
    Vec3i face = m_faces[faceIndex];
    int v0, v1, v2;
    v0 = IsVertexOnScalp(face[0]);
    v1 = IsVertexOnScalp(face[1]);
    v2 = IsVertexOnScalp(face[2]);
    if (v0==-1 || v1==-1 || v2==-1)
        return false;
    Vec3f dir0, dir1, dir2;
    dir0 = ComputeDirection(v0);
    dir1 = ComputeDirection(v1);
    dir2 = ComputeDirection(v2);
    float a0, a1, a2;
    hair_math::GetBarycentricCoordinates(pos, m_vertice[face[0]], m_vertice[face[1]], m_vertice[face[2]], a0, a1, a2);
    rootDirection = dir0*a0+dir1*a1+dir2*a2;
    normalize(rootDirection);
    return true;
}

bool CHairScalp::AssignRoot(Vec3f p, Vec3f& root, Vec3f& rootDirection)
{
    int nearestMatchScalpId = -1;
    float distMin = 1e10f;
    for (int i=0; i<m_scalpIds.size(); i++) {
        float recDist = dist(p, m_scalpPoints[i]);
        if (recDist<distMin) {
            distMin = recDist;
            nearestMatchScalpId = i;
        }
    }
    if ( nearestMatchScalpId < 0 )
    {
        return false;
    }
    vector<int> path = m_scalpPaths[ nearestMatchScalpId ];
    int bestMatchScalpId = 0;
    float maxDot = -1.0f;
    for (int i=0; i<path.size()-1; i++) {
        Vec3f dir = p - m_scalpPoints[path[i]];
        Vec3f scalpDir = m_scalpPoints[path[i]] - m_scalpPoints[path[i+1]];
        normalize(dir);
        float d = dot(dir, scalpDir);
        if (d > maxDot) {
            maxDot = d;
            bestMatchScalpId = i;
        }
    }
    
    vector<int> feasiblePath;
    for (int i=bestMatchScalpId; i<path.size(); i++) {
        feasiblePath.push_back(path[i]);
    }
    
    int selectedId = floor(feasiblePath.size() * rand()/(float)RAND_MAX);
    Vec3f n;
    SampleRandomPointAroundVertex(m_scalpIds[ feasiblePath[selectedId] ],  root, n);
    
    if (path.size()>1) {
        Vec3f rootDir = (bestMatchScalpId>0)?m_scalpPoints[path[bestMatchScalpId-1]] -  m_scalpPoints[path[bestMatchScalpId]]:
                        m_scalpPoints[path[bestMatchScalpId]] -  m_scalpPoints[path[bestMatchScalpId+1]];
        normalize(rootDir);
        rootDir = rootDir - (n*dot(n, rootDir));
        rootDir += (n*0.3f);
        normalize(rootDir);
        rootDirection = rootDir;
    } else {
        rootDirection = m_scalpNormals[path[0]];
        normalize(rootDirection);
    }
    
    return true;
}

void CHairScalp::GetAABB(Vec3f& aabbMin, Vec3f& aabbMax)
{
    aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    for (int i=0; i<m_vertice.size(); i++) {
        Vec3f p = m_vertice[i];
        aabbMin = Vec3f( min(p[0], aabbMin[0]), min(p[1], aabbMin[1]), min(p[2], aabbMin[2]) );
        aabbMax = Vec3f( max(p[0], aabbMax[0]), max(p[1], aabbMax[1]), max(p[2], aabbMax[2]) );
    }
}

void CHairScalp::GetNearestNeighbor(Vec3f curr, Vec3f& q)
{
    float minDist = 1e10f;
    for (int i=0; i<m_scalpIds.size(); i++) {
        float d = dist(m_scalpPoints[i], curr);
        if (d<minDist) {
            minDist = d;
            q = m_scalpPoints[i];
        }
    }
}

float CHairScalp::RayIntersectionWithTriangle(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2)
{
    Vec3f e1 = v1-v0;
    Vec3f e2 = v2-v0;
    Vec3f q = cross(orientation, e2);
    float tmp = dot(q, e1);
    if (fabsf(tmp)<1e-6)
        return -1000.0f;
    tmp = 1.0f/tmp;
    Vec3f s = start-v0;
    float u = tmp*dot(s, q);
    if (u<0.0f || u>1.0f)
        return -1000.0f;
    q = cross(s, e1);
    float vv = tmp*dot(orientation, q);
    if (vv<0.0 || vv+u>1.0)
        return -1000.0f;
    float dist = tmp*dot(e2, q);
    return dist;
}

bool CHairScalp::IsLineIntersectScalp(const CHairSample &_sample1, const CHairSample &_sample2)
{
    Vec3f start = _sample1.GetPos();
    Vec3f end = _sample2.GetPos();
    
    Vec3f orientation = end-start;
    normalize(orientation);
    
    for (int i=0; i<m_faces.size(); i++) {
        Vec3f v0 = m_vertice[m_faces[i][0]];
        Vec3f v1 = m_vertice[m_faces[i][1]];
        Vec3f v2 = m_vertice[m_faces[i][2]];
        float d = RayIntersectionWithTriangle(start, orientation, v0, v1, v2);
        if (d>0.0f && d<dist(start, end)) {
            return true;
        }
    }
    return false;
}

bool CHairScalp::IsSampleInScalp(const CHairSample &_sample)
{
    int hitTriangleNum = 0;
    float threshold = 0.00000001f;
    
    Vec3f orientation = Vec3f(0.0f, 1.0f, 0.0f);
    Vec3f start = _sample.GetPos();
    
    for (int i=0; i<m_faces.size(); i+=2) {
        Vec3f v0 = m_vertice[m_faces[i][0]];
        Vec3f v1 = m_vertice[m_faces[i][1]];
        Vec3f v2 = m_vertice[m_faces[i][2]];
        float dist0 = dist(start, v0);
        float dist1 = dist(start, v1);
        float dist2 = dist(start, v2);
        if (dist0<threshold || dist1<threshold || dist2<threshold) {
            hitTriangleNum = 2;
            break;
        }
        float dist = RayIntersectionWithTriangle(start, orientation, v0, v1, v2);
        if (dist > -threshold) {
            if (dist < threshold) {
                hitTriangleNum = 2;
                break;
            } else {
                hitTriangleNum++;
            }
        }
    }
    if (hitTriangleNum%2 == 1) {
        return true;
    } else {
        return false;
    }
}

float CHairScalp::DisttoLine(Vec3f pos)
{
    float minDist=1e10f;
    for (int i=0; i<m_linePoints.size(); i++) {
        float d = dist(pos, m_linePoints[i]);
        if (d<minDist) {
            minDist = d;
        }
    }
    return minDist;
}

void CHairScalp::GetRandomPointOnScalp(Vec3f& point, Vec3f& normal)
{
#if 1
    int vertId = m_scalpIds[rand()%m_scalpIds.size()];
    vector<int> adjFaces;
    for (int i=0; i<m_faces.size(); i=i+2) {
        Vec3i face = m_faces[i];
        if (face[0]==vertId || face[1]==vertId || face[2]==vertId) {
            adjFaces.push_back(i/2);
        }
    }
    int selectedFaceId = floor(adjFaces.size() * rand()/(float)RAND_MAX);
    float a0, a1, a2;
    a0 = (float)rand() / (float)RAND_MAX;
    a1 = (float)rand() / (float)RAND_MAX;
    if(a0 + a1 > 1.0f)
    {
        a0 = 1.0f - a0;
        a1 = 1.0f - a1;
    }
    a2 = 1.0f-a0-a1;
    point = (m_vertice[ m_faces[adjFaces[selectedFaceId]*2][0] ] * a0 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2][1] ] * a1 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2][2] ] * a2);
    normal= (m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][0] ] * a0 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][1] ] * a1 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2+1][2] ] * a2);
#elif 1
    int vertId = m_scalpIds[rand()%m_scalpIds.size()];
    vector<int> adjFaces;
    for (int i=1; i<m_faces.size(); i=i+2) {
        Vec3i face = m_faces[i];
        if (face[0]==vertId || face[1]==vertId || face[2]==vertId) {
            adjFaces.push_back(i/2);
        }
    }
    int selectedFaceId = floor(adjFaces.size() * rand()/(float)RAND_MAX);
    float a0, a1, a2;
    a0 = (float)rand() / (float)RAND_MAX;
    a1 = (float)rand() / (float)RAND_MAX;
    if(a0 + a1 > 1.0f)
    {
        a0 = 1.0f - a0;
        a1 = 1.0f - a1;
    }
    a2 = 1.0f-a0-a1;
    point = (m_vertice[ m_faces[adjFaces[selectedFaceId]*2+1][0] ] * a0 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2+1][1] ] * a1 +
             m_vertice[ m_faces[adjFaces[selectedFaceId]*2+1][2] ] * a2);
    normal= (m_normals[ m_faces[adjFaces[selectedFaceId]*2][0] ] * a0 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2][1] ] * a1 +
             m_normals[ m_faces[adjFaces[selectedFaceId]*2][2] ] * a2);
#endif
}

float CHairScalp::DistToLine(Vec3f p)
{
    float minDist = 1e10f;
    for (int i=0; i<m_linePoints.size(); i++) {
        float d = dist(m_linePoints[i], p);
        if (d<minDist) {
            minDist = d;
        }
    }
    return minDist;
}

float CHairScalp::DistToScalp(Vec3f p)
{
    float minDist = 1e10f;
    for (int i=0; i<m_scalpPoints.size(); i++) {
        float d = dist(m_scalpPoints[i], p);
        if (d<minDist) {
            minDist = d;
        }
    }
    return minDist;
}

int CHairScalp::IntersectPartingLine(vector<Vec3f> curve, vector<int>& intersectIds)
{
    for (int i = 0; i < curve.size() - 5; i++) {
        Vec3f p = curve[i];
        Vec3f q = curve[i + 5];
        float minDist = 1e10f;
        int lineId;
        for (int j=0; j<m_linePoints.size(); j++) {
            float d = dist(m_linePoints[j], p);
            if (d<minDist) {
                minDist = d;
                lineId = j;
            }
        }
        Vec3f linePoint0 = m_linePoints[lineId - (lineId>0)];
        Vec3f linePoint1 = m_linePoints[lineId + (lineId<m_linePoints.size()-1)];
        if (IntersectHalfPlaneWithSegment(linePoint0, linePoint1, m_lineNormals[lineId], p, q)) {
            intersectIds.push_back(i);
        }
    }
    return (int)intersectIds.size();
}

bool Solve3x3(Vec3f v1, Vec3f v2, Vec3f v3, Vec3f b, Vec3f& x)
{
    float z = dot(v1, cross(v2, v3));
    if (z == 0.0f) return false;
    float iz = 1.0f / z;
    // Crame's rule.
    x[0] = dot(b, cross(v2, v3)) * iz;
    x[1] = dot(v1, cross(b, v3)) * iz;
    x[2] = dot(v1, cross(v2, b )) * iz;
    return true;
}


bool CHairScalp::IntersectHalfPlaneWithSegment(Vec3f linePoint0, Vec3f linePoint1, Vec3f lineNormal, Vec3f seg0, Vec3f seg1)
{
    Vec3f binormal = cross((linePoint1 - linePoint0), lineNormal);
    float dot0 = dot((seg0 - linePoint0), binormal);
    float dot1 = dot((seg1 - linePoint0), binormal);
    if ((dot0 > 0) ^ (dot1 > 0) == false)
        return false; // same side
    Vec3f x;
    bool isNonsingular = Solve3x3(linePoint1 - linePoint0, lineNormal, seg1 - seg0, seg1 - linePoint0, x);
    if (!isNonsingular)
        return true;
    return (x[0] >= 0.0f && x[0] <= 1.0f);
}

void CHairScalp::GetRandomPoint(Vec3f& point, Vec3f& normal)
{
    int index = rand()%m_scalpPoints.size();
    point = m_scalpPoints[index];
    normal = m_scalpNormals[index];
}

Vec3f CHairScalp::FindNearestPointOnScalp(Vec3f p0)
{
    float distMin = std::numeric_limits<float>::max();;
    int idxMin = -1;
    Vec3f pNearest;
    for ( unsigned int i=0; i<m_faces.size(); i++ )
    {
        Vec3f v1 = m_vertice[m_faces[i][0]];
        Vec3f v2 = m_vertice[m_faces[i][1]];
        Vec3f v3 = m_vertice[m_faces[i][2]];
        Vec3f pNearestTmp;
        float distTmp;
        Vec3f e1 = v2 - v1;
        Vec3f e2 = v3 - v1;
        Vec3f n = cross(e1, e2);
        normalize(n);
        Scalar distToPlane = dot(n, (p0 - v1));
        Vec3f pc = p0 - distToPlane * n;
        distToPlane = abs(distToPlane);
        if ( hair_math::InsideTriangle(v1, v2, v3, pc) == true )
        {
            pNearestTmp = pc;
            distTmp = distToPlane;
        }
        else
        {
            float distSqr1 = dist2(v1, p0);
            float distSqr2 = dist2(v2, p0);
            float distSqr3 = dist2(v3, p0);
            if ( distSqr1 < distSqr2 && distSqr1 < distSqr3 )
            {
                pNearestTmp = v1;
                distTmp = sqrt(distSqr1);
            }
            else if ( distSqr2 < distSqr3 )
            {
                pNearestTmp = v2;
                distTmp = sqrt(distSqr2);
            }
            else
            {
                pNearestTmp = v3;
                distTmp = sqrt(distSqr3);
            }
        }
        if ( distTmp < distMin )
        {
            distMin = distTmp;
            idxMin = i;
            pNearest = pNearestTmp;
        }
    }
    return pNearest;
}

void CHairScalp::FindNearestPointOnScalp(Vec3f p0, Vec3f& p, Vec3f& n)
{
    float distMin = std::numeric_limits<float>::max();;
    int idxMin = -1;
    Vec3f pNearest;
    Vec3f nNearest;
    for ( unsigned int i=0; i<m_faces.size(); i++ )
    {
        Vec3f v1 = m_vertice[m_faces[i][0]];
        Vec3f v2 = m_vertice[m_faces[i][1]];
        Vec3f v3 = m_vertice[m_faces[i][2]];
        Vec3f pNearestTmp;
        float distTmp;
        Vec3f e1 = v2 - v1;
        Vec3f e2 = v3 - v1;
        Vec3f n = cross(e1, e2);
        normalize(n);
        Scalar distToPlane = dot(n, (p0 - v1));
        Vec3f pc = p0 - distToPlane * n;
        distToPlane = abs(distToPlane);
        if ( hair_math::InsideTriangle(v1, v2, v3, pc) == true )
        {
            pNearestTmp = pc;
            distTmp = distToPlane;
        }
        else
        {
            float distSqr1 = dist2(v1, p0);
            float distSqr2 = dist2(v2, p0);
            float distSqr3 = dist2(v3, p0);
            if ( distSqr1 < distSqr2 && distSqr1 < distSqr3 )
            {
                pNearestTmp = v1;
                distTmp = sqrt(distSqr1);
            }
            else if ( distSqr2 < distSqr3 )
            {
                pNearestTmp = v2;
                distTmp = sqrt(distSqr2);
            }
            else
            {
                pNearestTmp = v3;
                distTmp = sqrt(distSqr3);
            }
        }
        if ( distTmp < distMin )
        {
            distMin = distTmp;
            idxMin = i;
            pNearest = pNearestTmp;
            nNearest = n;
        }
    }
    p = pNearest;
    n = nNearest;
}

