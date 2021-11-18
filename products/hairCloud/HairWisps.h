//
//  HairWisps.h
//  HairSelector
//
//  Created by Liwen Hu on 1/15/14.
//  Copyright (c) 2014 Liwen Hu. All rights reserved.
//

#ifndef __HairSelector__HairWisps__
#define __HairSelector__HairWisps__

#include "HairWisp.h"
#include "HairScalp.h"
#include "HungarianAlgorithm.h"
#include "HairMath.h"
#include <queue>

class CHairWisps
{
public:
    CHairWisps();
    ~CHairWisps();
    
    void RenderHairWisps();
    bool DumpWispsToTXT(string fileName);
    bool LoadWispsFromTXT(string fileName);
    bool LoadWispsFromSimOBJ(string fileName);
    void VerifyDirection(CHairScalp* scalp);
    void GenerateStrands(CHairStrands* outputstrands, int num = 10000);
    int GetNumOfWisps() { return int(m_vecWisp.size()); }
    CHairWisp& GetWisp(int idx) { return m_vecWisp[idx]; }
    
    inline Vec3f GetAABBmin() { return m_aabbMin; }
	inline Vec3f GetAABBmax() { return m_aabbMax; }

    void SetAABBmin(Vec3f _aabbMin) {m_aabbMin = _aabbMin;}
    void SetAABBmax(Vec3f _aabbMax) {m_aabbMax = _aabbMax;}
    void UpdateAABB();
    
private:
    vector<CHairWisp> m_vecWisp;
	Vec3f m_aabbMin;
	Vec3f m_aabbMax;
};

#endif /* defined(__HairSelector__HairWisps__) */
