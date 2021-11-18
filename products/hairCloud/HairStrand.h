// Chongyang Ma - 2013-10-08
// Class declaration of a single hair strand
// --------------------------------------------------------------

#ifndef HAIRSTRAND_H
#define HAIRSTRAND_H

#include "HairSample.h"

class CHairStrand
{
public:
	CHairStrand();
    inline void ResizeStrand(int sz) { m_vecSample.resize(sz); }
    inline int GetNumOfSamples() const { return int(m_vecSample.size()); }
    inline CHairSample& GetSample(int idx) { return m_vecSample[idx]; }
    inline void SetColor(Vec3f color) { m_color = color; }
    Vec3f GetColor() { return m_color; }
    inline void AddSample(CHairSample& sample) { m_vecSample.push_back(sample); }
    inline void RemoveSample(int index) { m_vecSample.erase(m_vecSample.begin()+index); }
    CHairStrand FlipX();
    
    void AddPerSampleColor(Vec3f color) { m_vecPerSampleColor.push_back(color); }
    
    void SetPerSampleColor(vector<Vec3f>& vecColor) { m_vecPerSampleColor = vecColor; }
    
    void RenderStrandAsLineSegmentsFlipX();
    void RenderStrandAsLineSegments();
    void RenderStrandAsStroke();
    void RenderStrandAsCylindricalSegments();
    
    void GetAABB(Vec3f& aabbMin, Vec3f& aabbMax);
    
    float GetLength(int startIndex)
    {
        float length = 0.0f;
        for (int i=startIndex;i<m_vecSample.size()-1;i++) {
            Vec3f v0 = m_vecSample[i].GetPos();
            Vec3f v1 = m_vecSample[i+1].GetPos();
            length += dist(v0, v1);
        }
        return length;
    }
    
    void TranslateStrand(const Vec3f& trans);
    void TransformStrand(const Vec3f& trans, const Vec4f& rot, const Vec3f& posRef);
    Vec3f ComputeRefTranslation(CHairStrand& refStrand, Vec3f cameraPos, Vec3f cameraDir);
    void Reverse();
    void Clear() { m_vecSample.clear(); }
    void ResampleStrand(const int targetNumOfSamples);
    void ResampleStrand(int startId, const int targetNumOfSamples);
    float GetAverageSegmentLength();
    void ResampleStrandViaRelaxation(const int targetNumOfSamples, int iter = 10);
    float Dist2(CHairStrand& strand2);
    float Dist(CHairStrand& strand2);
    float NearestDist(CHairStrand& strand2);
    float NearestDist2(CHairStrand& strand2);
    float Length(int startIndex = 0);
    void SubStrand(int start, int end);
    
    CHairStrand LowPassFilterStrand(int halfKernelSize = 3);
    CHairStrand LowPassFilterStrand(int halfKernelSize, int startIdx, int endIdx);
    
  //  static GLUquadric* m_ptrQuadric;
    static Vec3f m_trans; // For rendering
	static float m_scalingFactor; // For rendering
    
    vector<Vec3f>& GetVerPerSampleColor() { return m_vecPerSampleColor; }

private:
	vector<CHairSample> m_vecSample;
    Vec3f m_color;
    vector<Vec3f> m_vecPerSampleColor;
};

#endif // HAIRSTRAND_H
