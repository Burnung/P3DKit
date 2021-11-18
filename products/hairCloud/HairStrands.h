// Chongyang Ma - 2013-10-08
// Class declaration of hair strands
// --------------------------------------------------------------

#ifndef HAIRSTRANDS_H
#define HAIRSTRANDS_H

#include "HairStrand.h"
#include <iostream>
#include <fstream>

class CHairStrands
{
public:
	CHairStrands();
    inline int GetNumOfStrands() { return int(m_vecStrand.size()); }
    inline void ResizeStrands(int sz) { m_vecStrand.resize(sz); }
    inline CHairStrand& GetStrand(int idx) { return m_vecStrand[idx]; }
    inline void AddStrand(CHairStrand& strand) { m_vecStrand.push_back(strand); }
    inline Vec3f GetAABBmin() { return m_aabbMin; }
	inline Vec3f GetAABBmax() { return m_aabbMax; }
    void SetAABBmin(Vec3f _aabbMin) {m_aabbMin = _aabbMin;}
    void SetAABBmax(Vec3f _aabbMax) {m_aabbMax = _aabbMax;}
    void UpdateAABB(bool flag = true);
    void Clear() { m_vecStrand.clear(); }
    void RandomSelect(CHairStrands *outputStrands, int seedNum=10000);
    void FlipX();
    void FindNearestStrand(Vec3f p, CHairStrand& strand);
    void ScaleStrands(float k);
    void ScaleStrands(Vec3f k);
    void TranslateStrands(Vec3f trans);
    void RemoveStrand(int idx) { m_vecStrand.erase(m_vecStrand.begin() + idx); }
    void SortAllStrandsByLength();
    void GetStrandsAroundSegment(Vec3f endpoint1, Vec3f endpoint2, float radius, vector<int>& indices, vector<bool>& isMasked);
    void GetShortStrands(int num, CHairStrands* outputStrands);

    void RenderStrandsAsLineSegmentsFlipX(int interval = 1);
    void RenderStrandsAsLineSegments(int interval = 1);
    void RenderStrandsAsStrokes();
    void RenderStrandRootsAsSpheres();
    void RenderStrandsAsCylindricalSegments();
    
    bool LoadHairStrandsFromTXT(string fileName);
	bool LoadHairStrands(const char* fileName);
    bool LoadChaiData(const char* fileName);
    bool DumpStrandsWithSegmentColor(string fileName);
    bool DumpStrandsAsBinaryData(string fileName);
	bool DumpStrandsToTXT(string fileName);
    bool DumpSelectedStrands(string fileName);
    bool DumpSelectedIndices(string fileName);
    bool DumpStrandsToOBJ(string fileName, Scalar rad = 0.01f, int sampleNum = 8, int interval = 1, int interp = 0);
    void DumpStrandToOBJ(CHairStrand& strand, ofstream& fout, int& vAccum, Scalar rad, int sampleNum, int interval);
    void DumpStrandToOBJWithColor(CHairStrand& strand, ofstream& fout, int& vAccum, Scalar rad, int sampleNum, int interval);
    
    void RemoveZeroStrands();
    
    void CheckStrands();
    
    void AddNoise();
    
    void AddNoise(CHairStrand strand, CHairStrand& outputStrand);

protected:
	vector<CHairStrand> m_vecStrand;
	Vec3f m_aabbMin;
	Vec3f m_aabbMax;
};

#endif // HAIRSTRANDS_H
