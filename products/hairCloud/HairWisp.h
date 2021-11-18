//
//  HairWisp.h
//  HairSelector
//
//  Created by Liwen Hu on 1/15/14.
//  Copyright (c) 2014 Liwen Hu. All rights reserved.
//

#ifndef __HairSelector__HairWisp__
#define __HairSelector__HairWisp__

#include "HairStrands.h"

class CHairWisp : public CHairStrands
{
public:
    CHairWisp();
    ~CHairWisp();
    
    int Width() { return (int)m_vecStrand.size(); }
    
    int Length() { return (int)(m_vecStrand.empty()?0:m_vecStrand[0].GetNumOfSamples()); }
    
    void SetColor(Vec3f _color) { m_color = _color; }
    
    void Reverse();
    
    void Rotate();
    
    Vec3f GetNormal(int w, int l);
    
    Vec3f GetNormal(float w, float l);
    
    Vec3f Lerp(float w, float l);
    
    void GenerateStrand(float w, float thick, CHairStrand& strand);
    
    void RenderHairWisp();
    
private:
    Vec3f m_color;
};

#endif /* defined(__HairSelector__HairWisp__) */
