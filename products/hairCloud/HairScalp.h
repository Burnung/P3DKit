//
//  HairScalp.h
//  HairSyn
//
//  Created by Liwen Hu on 13-10-19.
//  Copyright (c) 2013 Liwen Hu. All rights reserved.
//

#ifndef __HairSyn__HairScalp__
#define __HairSyn__HairScalp__

#include "HairSample.h"
#include "HairMath.h"
#include "MathUtils.hpp"

class CHairScalp
{
public:
    CHairScalp(const char * fileName);
    
    ~CHairScalp();
    
    CHairSample GetTheNearestSampleOnScalp(const CHairSample& _sample);
    
    void GetScalpMeshFromHead(char* inputName, char* outputName);
    
    void FindPartingLinePointIndices();
    
    void FindScalpPointIndices();
    
    void ComputeDistanceToLine();
    
    bool AssignRoot(Vec3f p, Vec3f& root, Vec3f& rootDirection);
    
    void SampleRandomPointAroundVertex(int vertId, Vec3f& point, Vec3f& normal);
    
    void FindShortestPath(int startIdx, int endIdx, float* graph, vector<int>& path);
    
    void GetNearestNeighbor(Vec3f curr, Vec3f& q);
    
    bool IsSampleInScalp(const CHairSample &_sample);
    
    bool IsLineIntersectScalp(const CHairSample &_sample1, const CHairSample &_sample2);
    
    void GetRandomPointOnScalp(Vec3f& point, Vec3f& normal);
    
    float RayIntersectionWithTriangle(Vec3f start, Vec3f orientation, Vec3f v0, Vec3f v1, Vec3f v2);
    
    float DistToLine(Vec3f p);
    
    float DistToScalp(Vec3f p);
    
    int IntersectPartingLine(vector<Vec3f> curve, vector<int>& intersectIds);
    
    bool IntersectHalfPlaneWithSegment(Vec3f linePoint0, Vec3f linePoint1, Vec3f lineNormal, Vec3f seg0, Vec3f seg1);
    
    void GetAABB(Vec3f& aabbMin, Vec3f& aabbMax);
    
    vector<Vec3i>& GetFaces() { return m_faces; }
    
    vector<Vec3f>& GetVertice() { return m_vertice; }
    
    vector<Vec3f>& GetNormals() { return m_normals; }
    
    void GetRandomPoint(Vec3f& point, Vec3f& normal);
    
    float DisttoLine(Vec3f pos);
    
    bool ComputeRootDirection(Vec3f pos, int faceIndex, Vec3f& rootDirection);
    
    Vec3f ComputeDirection(int scalpId);
    
    int IsVertexOnScalp(int vertexId);
    
    Vec3f FindNearestPointOnScalp(Vec3f pos);
    
    void FindNearestPointOnScalp(Vec3f p0, Vec3f& p, Vec3f& n);
    
private:    
    vector<Vec3f> m_vertice;
    vector<Vec3f> m_normals;
    vector<Vec3f> m_colors;
    vector<Vec3i> m_faces;
    
    vector<int>             m_lineIds;
    vector<Vec3f>           m_linePoints;
    vector<Vec3f>           m_lineNormals;
    vector<int>             m_scalpIds;
    vector<int>             m_IdsInScalp;
    vector<Vec3f>           m_scalpPoints;
    vector<Vec3f>           m_scalpNormals;
    vector<float>           m_scalpDists;
    vector< vector<int> >   m_scalpPaths;
};

#endif /* defined(__HairSyn__HairScalp__) */
