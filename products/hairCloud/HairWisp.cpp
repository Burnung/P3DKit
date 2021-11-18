//
//  HairWisp.cpp
//  HairSelector
//
//  Created by Liwen Hu on 1/15/14.
//  Copyright (c) 2014 Liwen Hu. All rights reserved.
//

#include "HairWisp.h"

CHairWisp::CHairWisp()
{
    
}

CHairWisp::~CHairWisp()
{
    
}
/*
void CHairWisp::RenderHairWisp()
{
    for ( int i=1; i<GetNumOfStrands(); i++ )
    {
        int strandIdx0 = i - 1;
        int strandIdx1 = i;
        CHairStrand& strand0 = GetStrand(strandIdx0);
        CHairStrand& strand1 = GetStrand(strandIdx1);
        if ( strand0.GetNumOfSamples() != strand1.GetNumOfSamples() )
        {
            continue;
        }
        
        glBegin(GL_TRIANGLES);
        for ( int j=1; j<strand0.GetNumOfSamples(); j++ )
        {
            glColor3f((float)(strand0.GetNumOfSamples()-j)/strand0.GetNumOfSamples(),
                      (float)j/strand0.GetNumOfSamples(), 0.0f);
            int sampleIdx0 = j - 1;
            int sampleIdx1 = j;
            Vec3f p00 = strand0.GetSample(sampleIdx0).GetPos();
            Vec3f p01 = strand0.GetSample(sampleIdx1).GetPos();
            Vec3f p10 = strand1.GetSample(sampleIdx0).GetPos();
            Vec3f p11 = strand1.GetSample(sampleIdx1).GetPos();
            glVertex3f(p00[0], p00[1], p00[2]);
            glVertex3f(p10[0], p10[1], p10[2]);
            glVertex3f(p01[0], p01[1], p01[2]);
            glVertex3f(p10[0], p10[1], p10[2]);
            glVertex3f(p11[0], p11[1], p11[2]);
            glVertex3f(p01[0], p01[1], p01[2]);
        }
        glEnd();
    }
}
*/
void CHairWisp::Reverse()
{
    for ( int i=0; i<GetNumOfStrands(); i++ )
    {
        GetStrand(i).Reverse();
    }
}

void CHairWisp::Rotate()
{
    int w = Width();
    int l = Length();
    vector<CHairStrand> tempStrands(l);
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < w; j++)
        {
            tempStrands[i].AddSample(GetStrand(j).GetSample(i));
        }
    }
    m_vecStrand = tempStrands;
}

Vec3f CHairWisp::GetNormal(int w, int l)
{
    int width = this->Width();
    int length = this->Length();
    w = std::clamp<int>(w, 0, width - 1);
    l = std::clamp<int>(l, 0, length - 1);
    Vec3f dw = m_vecStrand[w+(w<width-1)].GetSample(l).GetPos() -
    m_vecStrand[w-(w>0)].GetSample(l).GetPos();
    Vec3f dl = m_vecStrand[w].GetSample(l+(l<length-1)).GetPos() -
    m_vecStrand[w].GetSample(l-(l>0)).GetPos();
    if (dw==Vec3f(0.0f, 0.0f, 0.0f) || dl==Vec3f(0.0f, 0.0f, 0.0f))
        return Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f n = cross(dw, dl);
    normalize(n);
    return n;
}

Vec3f CHairWisp::GetNormal(float w, float l)
{
    w = std::clamp(w, 0.0f, float(Width()-1));
    l = std::clamp(l, 0.0f, float(Length()-1));
    int wl = floor(w);
    int wu = ceil(w);
    int ll = floor(l);
    int lu = ceil(l);
    float wt = w - wl;
    float lt = l - ll;
    Vec3f n = ((GetNormal(wl, ll) * (1-lt) + GetNormal(wl, lu) * lt) * (1-wt) +
               (GetNormal(wu, ll) * (1-lt) + GetNormal(wu, lu) * lt) * wt);
    if (n==Vec3f(0.0f, 0.0f, 0.0f))
        return Vec3f(0.0f, 0.0f, 0.0f);
    normalize(n);
    return n;
}

Vec3f CHairWisp::Lerp(float w, float l)
{
    w = std::clamp(w, 0.0f, float(Width()-1));
    l = std::clamp(l, 0.0f, float(Length()-1));
    int wl = floor(w);
    int wu = ceil(w);
    int ll = floor(l);
    int lu = ceil(l);
    float wt = w - wl;
    float lt = l - ll;
    return ((m_vecStrand[wl].GetSample(ll).GetPos() * (1 - lt) + m_vecStrand[wl].GetSample(lu).GetPos() * lt) * (1-wt) +
            (m_vecStrand[wu].GetSample(ll).GetPos() * (1 - lt) + m_vecStrand[wu].GetSample(lu).GetPos() * lt) * wt);
}

void CHairWisp::GenerateStrand(float w, float thick, CHairStrand& strand)
{
    int beginL = 0;
    int endL = Length()-1;
    int step = 1;
    for (int l=beginL; l!=endL+step; l+=step)
    {
        Vec3f n = GetNormal(w, (float)l);
        if (n==Vec3f(0.0f, 0.0f, 0.0f))
            continue;
        Vec3f p = Lerp(w, (float)l) - n * thick;
        CHairSample sample;
        sample.SetPos(p);
        sample.SetNorm(n);
        strand.AddSample(sample);
    }
}
