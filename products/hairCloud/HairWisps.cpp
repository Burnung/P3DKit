//
//  HairWisps.cpp
//  HairSelector
//
//  Created by Liwen Hu on 1/15/14.
//  Copyright (c) 2014 Liwen Hu. All rights reserved.
//

#include "HairWisps.h"

#define OUTPUT_ERROR_WISP

CHairWisps::CHairWisps()
{
    m_aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	m_aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
}

CHairWisps::~CHairWisps()
{
    
}

void CHairWisps::UpdateAABB()
{
	if ( m_vecWisp.size() == 0 )
	{
		return;
	}
    
	m_aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	m_aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    
	for ( int i=0; i<m_vecWisp.size(); i++ )
	{
		Vec3f aabbMinTmp, aabbMaxTmp;
        m_vecWisp[i].UpdateAABB();
        aabbMaxTmp = m_vecWisp[i].GetAABBmax();
        aabbMinTmp = m_vecWisp[i].GetAABBmin();
		for ( int j=0; j<3; j++ )
		{
			m_aabbMin[j] = (m_aabbMin[j] <= aabbMinTmp[j]) ? m_aabbMin[j] : aabbMinTmp[j];
			m_aabbMax[j] = (m_aabbMax[j] >= aabbMaxTmp[j]) ? m_aabbMax[j] : aabbMaxTmp[j];
		}
	}
}
/*
void CHairWisps::RenderHairWisps()
{
    glDisable(GL_LIGHTING);
    Vec3f minAABB = GetAABBmin();
    Vec3f maxAABB = GetAABBmax();
    glPushMatrix();
    float scale = max(max((maxAABB.v[0]-minAABB.v[0]), (maxAABB.v[1]-minAABB.v[1])), (maxAABB.v[2]-minAABB.v[2]));
	glScalef(2.0f/scale, 2.0f/scale, 2.0f/scale);
    glTranslatef(-(maxAABB.v[0]+minAABB[0])/2.0f, -(maxAABB.v[1]+minAABB[1])/2.0f, -(maxAABB.v[2]+minAABB[2])/2.0f);
    for ( int i=0; i<m_vecWisp.size(); i++ )
    {
        glPushMatrix();
        glLoadName(i);
        m_vecWisp[i].RenderHairWisp();
        glPopMatrix();
    }
    glPopMatrix();
    glEnable(GL_LIGHTING);
}
*/
bool CHairWisps::DumpWispsToTXT(string fileName)
{
    if (m_vecWisp.size() == 0)
        return false;
    FILE *out;
    out = fopen(fileName.c_str(), "w+");
    cout<<"Dump Wisps to "<<fileName<<endl;
    
    fprintf(out, "%d\n", (int)m_vecWisp.size());
    for (int i=0; i<m_vecWisp.size(); i++) {
        CHairWisp wisp = m_vecWisp[i];
        fprintf(out, "%d\n", (int)wisp.GetNumOfStrands());
        for (int j=0; j<wisp.GetNumOfStrands(); j++) {
            CHairStrand strand = wisp.GetStrand(j);
            fprintf(out, "%d\n", (int)strand.GetNumOfSamples());
            for (int k=0; k<strand.GetNumOfSamples(); k++) {
                Vec3f pos = strand.GetSample(k).GetPos();
                fprintf(out, "%f %f %f\n", pos[0], pos[1], pos[2]);
            }
        }
    }
    fclose(out);
    return true;
}

bool CHairWisps::LoadWispsFromTXT(string fileName)
{
    FILE *in;
    in = fopen(fileName.c_str(), "r");
    int wnum;
    fscanf(in,"%d\n",&wnum);
    cout<<"Load Wisps Number: "<<wnum<<endl;
    float x, y, z;
    for (int i=0; i<wnum; i++) {
        CHairWisp wisp;
        int snum;
        fscanf(in,"%d\n",&snum);
        for (int j=0; j<snum; j++) {
            CHairStrand strand;
            int sampleNum;
            fscanf(in,"%d\n",&sampleNum);
            for (int k=0; k<sampleNum; k++) {
                CHairSample sample;
                fscanf(in,"%f %f %f\n",&x, &y, &z);
                sample.SetPos(Vec3f(x, y, z));
                strand.AddSample(sample);
            }
            wisp.AddStrand(strand);
        }
        Vec3f randColor((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
        wisp.SetColor(randColor);
        m_vecWisp.push_back(wisp);
    }
    fclose(in);

    UpdateAABB();
    return true;
}

void insertNew(vector<int>& vec, int i)
{
    bool flag = true;
    for (int j=0; j<vec.size(); j++)
    {
        if (vec[j] == i)
        {
            flag = false;
            break;
        }
    }
    if (flag == true)
    {
        vec.push_back(i);
    }
}

bool findV(vector<int>& vec, int i)
{
    bool flag = true;
    for (int j=0; j<vec.size(); j++)
    {
        if (vec[j] == i)
        {
            flag = false;
            break;
        }
    }
    if (flag == false)
    {
        return true;
    }
    return false;
}

Vec3f HSVtoRGB(Vec3f hsv)
{
    // From FvD
    float H = hsv[0];
    float S = hsv[1];
    float V = hsv[2];
    if ( S <= 0.0f )
    {
        return Vec3f(V, V, V);
    }
    H = std::fmod(H, 8.0f * atan(1.0f));
    if ( H < 0.0f )
    {
        H += 8.0f * atan(1.0f);
    }
    H *= 6.0f / (8.0f * atan(1.0f));
    int i = int(std::floor(H));
    float f = H - i;
    float p = V * (1.0f - S);
    float q = V * (1.0f - (S * f));
    float t = V * (1.0f - (S * (1.0f - f)));
    switch (i)
    {
        case 0:
            return Vec3f(V, t, p);
        case 1:
            return Vec3f(q, V, p);
        case 2:
            return Vec3f(p, V, t);
        case 3:
            return Vec3f(p, q, V);
        case 4:
            return Vec3f(t, p, V);
        default:
            return Vec3f(V, p, q);
    }
}

bool CHairWisps::LoadWispsFromSimOBJ(string fileName)
{
    vector<Vec3f> vertices;
    vector<Vec2f> texCoords;
    vector<Vec3i> faces;
    char temp[100];
    fstream file1;
	file1.open(fileName.c_str(), ios::in);
    while (!file1.eof())
	{
		float a[3];
		int b[9];
		file1.getline(temp,100);
		if (temp[0]=='v' && temp[1]==' ')
		{
			sscanf(temp,"%*s%f %f %f",&a[0],&a[1],&a[2]);
            vertices.push_back(Vec3f(a[0], a[1], a[2]));
		}
        else if (temp[0]=='v' && temp[1]=='t')
        {
            sscanf(temp,"%*s%f %f %f",&a[0],&a[1]);
            texCoords.push_back(Vec2f(a[0], a[1]));
        }
		else if (temp[0] == 'f')
		{
			sscanf(temp,"%*s%d/%d/%d %d/%d/%d %d/%d/%d",&b[0],&b[1],&b[2],&b[3],&b[4],&b[5],&b[6],&b[7],&b[8]);
			for (int n=0;n<9;n++)
				b[n]=b[n]-1;
			faces.push_back(Vec3i(b[0], b[3], b[6]));
		}
	}
    file1.close();
    
    vector<vector<int> > neighbors(vertices.size());
    for (int i=0; i<faces.size(); i++)
    {
        Vec3i f = faces[i];
        insertNew(neighbors[f[0]], f[1]);
        insertNew(neighbors[f[0]], f[2]);
        insertNew(neighbors[f[1]], f[0]);
        insertNew(neighbors[f[1]], f[2]);
        insertNew(neighbors[f[2]], f[0]);
        insertNew(neighbors[f[2]], f[1]);
    }
    vector<bool> isGrouped(vertices.size(), false);
    vector<vector<int> > vGroups;
    vector<vector<int> > fGroups;
    
    for (int i=0; i<vertices.size(); i++)
    {
        if (isGrouped[i] == true)
            continue;
        vector<int> vGroup;
        queue<int> vQueue;
        vQueue.push(i);
        isGrouped[i] = true;
        while (vQueue.size() != 0)
        {
            int tmpV = vQueue.front();
            vQueue.pop();
            vGroup.push_back(tmpV);
            for (int j=0; j<neighbors[tmpV].size(); j++)
            {
                if (isGrouped[neighbors[tmpV][j]] == false)
                {
                    vQueue.push(neighbors[tmpV][j]);
                    isGrouped[neighbors[tmpV][j]] = true;
                }
            }
        }
        vGroups.push_back(vGroup);
    }
    
    for (int i=0; i<vGroups.size(); i++)
    {
        vector<int> vGroup = vGroups[i];
        vector<int> fGroup;
        for (int j=0; j<faces.size(); j++)
        {
            if (findV(vGroup, faces[j][0]) == true)
            {
                fGroup.push_back(j);
            }
        }
        fGroups.push_back(fGroup);
        vector<vector<int> > edges(vGroup.size(), vector<int>(vGroup.size(), 0));
        vector<int> vMap = vector<int>(vertices.size(), -1);
        for (int j=0; j<vGroup.size(); j++)
        {
            vMap[vGroup[j]] = j;
        }
        
        for (int j=0; j<fGroup.size(); j++)
        {
            Vec3i f = faces[fGroup[j]];
            edges[vMap[f[0]]][vMap[f[1]]]++;
            edges[vMap[f[0]]][vMap[f[2]]]++;
            edges[vMap[f[1]]][vMap[f[0]]]++;
            edges[vMap[f[1]]][vMap[f[2]]]++;
            edges[vMap[f[2]]][vMap[f[0]]]++;
            edges[vMap[f[2]]][vMap[f[1]]]++;
        }
        
        vector<vector<int> > boundEdge(vGroup.size());
        for (int j=0; j<vGroup.size(); j++)
        {
            for (int k=0; k<vGroup.size(); k++)
            {
                if (edges[j][k]==1)
                {
                    insertNew(boundEdge[j], k);
                    insertNew(boundEdge[k], j);
                }
            }
        }
        
        vector<int> boundSequence;
        int boundNum = 0;
        int startV = -1;
        int nextV;
        for (int j=0; j<boundEdge.size(); j++)
        {
            if (boundEdge[j].size() > 0)
            {
                if (startV == -1)
                {
                    startV = j;
                    nextV = boundEdge[j][0];
                }
                boundNum++;
            }
        }
        
        boundSequence.push_back(startV);
        boundSequence.push_back(nextV);
        int iter = 0;
        while (boundSequence.size()<boundNum)
        {
            int tmpV = boundSequence[boundSequence.size()-1];
            for (int j=0; j<boundEdge[tmpV].size(); j++)
            {
                insertNew(boundSequence, boundEdge[tmpV][j]);
            }
            iter++;
            if (iter>boundNum)
                break;
        }
        if (iter>boundNum) {
#ifdef OUTPUT_ERROR_WISP
            char fileName[260];
            sprintf(fileName, "error_%04d.obj", i);
            
            ofstream fout(CHairConfig::AddOutputPrefix(fileName).c_str());
            
            for (int j=0; j<vGroup.size(); j++)
            {
                fout<<"v "<<vertices[vGroup[j]][0]<<" "<<vertices[vGroup[j]][1]<<" "<<vertices[vGroup[j]][2]<<endl;
                fout<<"vn "<<"0.0"<<" "<<"0.0"<<" "<<"-1.0"<<endl;
                fout<<"vt "<<texCoords[vGroup[j]][0]<<" "<<texCoords[vGroup[j]][1]<<endl;
            }
            
            for (int j=0; j<fGroup.size(); j++)
            {
                int idx1 = vMap[faces[fGroup[j]][0]]+1;
                int idx2 = vMap[faces[fGroup[j]][1]]+1;
                int idx3 = vMap[faces[fGroup[j]][2]]+1;
                fout<<"f "<<idx1<<"/"<<idx1<<"/"<<idx1<<" "<<idx2<<"/"<<idx2<<"/"<<idx2<<" "<<idx3<<"/"<<idx3<<"/"<<idx3<<std::endl;
            }
#endif
            continue;
        }
        
        int width;
        vector<int> startVs;
        bool ok = false;
        for (width=2; width<=sqrt((float)vGroup.size()); width++)
        {
            if (vGroup.size()%width != 0)
                continue;
            for (int j=0; j<boundSequence.size(); j++)
            {
                startVs.clear();
                vector<bool> isMarked(vertices.size(), false);
                for (int k=0; k<width; k++)
                {
                     startVs.push_back(vGroup[boundSequence[(j+k)%boundSequence.size()]]);
                }
                for (int k=0; k<width; k++)
                {
                    isMarked[startVs[k]] = true;
                }
                int step = (int)vGroup.size()/(int)startVs.size();
                vector<int> tmpVs = startVs;
                bool flag = true;
                for (int l=1; l<step; l++)
                {
                    vector<int> nextVs;
                    for (int m=0; m<tmpVs.size(); m++)
                    {
                        for (int n=0; n<neighbors[tmpVs[m]].size(); n++)
                        {
                            if (isMarked[neighbors[tmpVs[m]][n]]==false)
                            {
                                insertNew(nextVs, neighbors[tmpVs[m]][n]);
                                isMarked[neighbors[tmpVs[m]][n]] = true;
                            }
                        }
                    }
                    if (nextVs.size() != width)
                    {
                        flag = false;
                        break;
                    }
                    tmpVs = nextVs;
                }
                if (flag == true)
                {
                    ok = true;
                    break;
                }
            }
            if (ok == true)
            {
                break;
            }
        }

        if (ok==false)
        {
            int width;
            vector<int> startVs;
            int largeWidth;
            bool ok1 = false;
            vector<int> boundVertices(boundSequence.size());
            for (int j=0; j<boundSequence.size(); j++)
            {
                boundVertices[j] = vGroup[boundSequence[j]];
            }
            for (width=1; width<=sqrt((float)vGroup.size()); width++)
            {
                for (int j=0; j<boundSequence.size(); j++)
                {
                    largeWidth = width;
                    startVs.clear();
                    vector<bool> isMarked(vertices.size(), false);
                    vector<int> markerVertices;
                    for (int k=0; k<width; k++)
                    {
                        startVs.push_back(vGroup[boundSequence[(j+k)%boundSequence.size()]]);
                    }
                    for (int k=0; k<width; k++)
                    {
                        isMarked[startVs[k]] = true;
                        insertNew(markerVertices, startVs[k]);
                    }
                    vector<int> tmpVs = startVs;
                    bool flag = true;
                    while (markerVertices.size() != vGroup.size())
                    {
                        vector<int> nextVs;
                        for (int m=0; m<tmpVs.size(); m++)
                        {
                            for (int n=0; n<neighbors[tmpVs[m]].size(); n++)
                            {
                                if (isMarked[neighbors[tmpVs[m]][n]]==false)
                                {
                                    insertNew(nextVs, neighbors[tmpVs[m]][n]);
                                    isMarked[neighbors[tmpVs[m]][n]] = true;
                                    insertNew(markerVertices, neighbors[tmpVs[m]][n]);
                                }
                            }
                        }
                        largeWidth = (largeWidth >= nextVs.size())?largeWidth:(int)nextVs.size();
                        if (nextVs.size()==1)
                        {
                            
                        }
                        else if (nextVs.size()==2)
                        {
                            if (findV(neighbors[nextVs[0]], nextVs[1])==false)
                            {
                                flag = false;
                                break;
                            }
                        }
                        else
                        {
                            vector<int> line;
                            vector<bool> isInLine(nextVs.size(), false);
                            for (int m=0; m<nextVs.size(); m++)
                            {
                                if (findV(boundVertices, nextVs[m]) == true)
                                {
                                    line.push_back(nextVs[m]);
                                    isInLine[m] = true;
                                    break;
                                }
                            }
                            if (line.size() == 0)
                            {
                                flag = false;
                                break;
                            }
                            for (int m=0; m<nextVs.size()-1; m++)
                            {
                                int tmp = line[line.size()-1];
                                for (int n=0; n<nextVs.size(); n++)
                                {
                                    if (isInLine[n]==true)
                                        continue;
                                    if (findV(neighbors[tmp], nextVs[n])==true)
                                    {
                                        line.push_back(nextVs[n]);
                                        isInLine[n] = true;
                                        break;
                                    }
                                }
                            }
                            if (line.size() != nextVs.size())
                            {
                                flag = false;
                                break;
                            }
                            for (int m=0; m<nextVs.size()-2; m++)
                            {
                                Vec3f t0 = vertices[line[m+1]]-vertices[line[m]];
                                Vec3f t1 = vertices[line[m+2]]-vertices[line[m+1]];
                                normalize(t0);
                                normalize(t1);
                                if (dot(t0, t1) < 0.5f)
                                {
                                    flag = false;
                                    break;
                                }
                            }
                        }
                        tmpVs = nextVs;
                    }
                    if (flag == true)
                    {
                        ok1 = true;
                        break;
                    }
                }
                if (ok1 == true)
                {
                    break;
                }
            }
            
            if (ok1 == false)
            {
#ifdef OUTPUT_ERROR_WISP
                char fileName[260];
                sprintf(fileName, "error_%04d.obj", i);
                
                ofstream fout(CHairConfig::AddOutputPrefix(fileName).c_str());
                
                for (int j=0; j<vGroup.size(); j++)
                {
                    fout<<"v "<<vertices[vGroup[j]][0]<<" "<<vertices[vGroup[j]][1]<<" "<<vertices[vGroup[j]][2]<<endl;
                    fout<<"vn "<<"0.0"<<" "<<"0.0"<<" "<<"-1.0"<<endl;
                    fout<<"vt "<<texCoords[vGroup[j]][0]<<" "<<texCoords[vGroup[j]][1]<<endl;
                }
                
                for (int j=0; j<fGroup.size(); j++)
                {
                    int idx1 = vMap[faces[fGroup[j]][0]]+1;
                    int idx2 = vMap[faces[fGroup[j]][1]]+1;
                    int idx3 = vMap[faces[fGroup[j]][2]]+1;
                    fout<<"f "<<idx1<<"/"<<idx1<<"/"<<idx1<<" "<<idx2<<"/"<<idx2<<"/"<<idx2<<" "<<idx3<<"/"<<idx3<<"/"<<idx3<<std::endl;
                }
#endif
                continue;
            }
            
            CHairWisp wisp;
            wisp.ResizeStrands(largeWidth);
            vector<int> tmpVs = startVs;
            vector<bool> isMarked(vertices.size(), false);
            vector<int> markerVertices;
            for (int k=0; k<width; k++)
            {
                isMarked[startVs[k]] = true;
                insertNew(markerVertices, startVs[k]);
            }
            if (startVs.size() == 1)
            {
                for (int k=0; k<wisp.Width(); k++)
                {
                    CHairSample sample;
                    sample.SetPos(vertices[startVs[0]]);
                    wisp.GetStrand(k).AddSample(sample);
                }
            }
            else
            {
                vector<int> line;
                vector<bool> isInLine(startVs.size(), false);
                for (int m=0; m<startVs.size(); m++)
                {
                    if (findV(boundVertices, startVs[m]) == true)
                    {
                        line.push_back(startVs[m]);
                        isInLine[m] = true;
                        break;
                    }
                }
                for (int m=0; m<startVs.size()-1; m++)
                {
                    int tmp = line[line.size()-1];
                    for (int n=0; n<startVs.size(); n++)
                    {
                        if (isInLine[n]==true)
                            continue;
                        if (findV(neighbors[tmp], startVs[n])==true)
                        {
                            line.push_back(startVs[n]);
                            isInLine[n] = true;
                            break;
                        }
                    }
                }
                CHairStrand s;
                for (int m=0; m<line.size(); m++)
                {
                    CHairSample sample;
                    sample.SetPos(vertices[line[m]]);
                    s.AddSample(sample);
                }
                s.ResampleStrand(largeWidth);
                for (int m=0; m<largeWidth; m++)
                {
                    wisp.GetStrand(m).AddSample(s.GetSample(m));
                }
            }
            
            while (markerVertices.size() != vGroup.size())
            {
                vector<int> nextVs;
                for (int m=0; m<tmpVs.size(); m++)
                {
                    for (int n=0; n<neighbors[tmpVs[m]].size(); n++)
                    {
                        if (isMarked[neighbors[tmpVs[m]][n]]==false)
                        {
                            insertNew(nextVs, neighbors[tmpVs[m]][n]);
                            isMarked[neighbors[tmpVs[m]][n]] = true;
                            insertNew(markerVertices, neighbors[tmpVs[m]][n]);
                        }
                    }
                }
                if (nextVs.size() == 1)
                {
                    for (int k=0; k<wisp.Width(); k++)
                    {
                        CHairSample sample;
                        sample.SetPos(vertices[nextVs[0]]);
                        wisp.GetStrand(k).AddSample(sample);
                    }
                }
                else {
                    vector<int> line;
                    vector<bool> isInLine(nextVs.size(), false);
                    for (int m=0; m<nextVs.size(); m++)
                    {
                        if (findV(boundVertices, nextVs[m]) == true)
                        {
                            line.push_back(nextVs[m]);
                            isInLine[m] = true;
                            break;
                        }
                    }
                    for (int m=0; m<nextVs.size()-1; m++)
                    {
                        int tmp = line[line.size()-1];
                        for (int n=0; n<nextVs.size(); n++)
                        {
                            if (isInLine[n]==true)
                                continue;
                            if (findV(neighbors[tmp], nextVs[n])==true)
                            {
                                line.push_back(nextVs[n]);
                                isInLine[n] = true;
                                break;
                            }
                        }
                    }
                    CHairStrand s;
                    for (int m=0; m<line.size(); m++)
                    {
                        CHairSample sample;
                        sample.SetPos(vertices[line[m]]);
                        s.AddSample(sample);
                    }
                    s.ResampleStrand(largeWidth);
                    if (dist(wisp.GetStrand(0).GetSample(wisp.GetStrand(0).GetNumOfSamples()-1).GetPos(), s.GetSample(0).GetPos()) >
                        dist(wisp.GetStrand(0).GetSample(wisp.GetStrand(0).GetNumOfSamples()-1).GetPos(), s.GetSample(s.GetNumOfSamples()-1).GetPos()))
                    {
                        s.Reverse();
                    }
                    for (int m=0; m<largeWidth; m++)
                    {
                        wisp.GetStrand(m).AddSample(s.GetSample(m));
                    }
                }
                tmpVs = nextVs;
            }
            int m = rand();
            Vec3f hsv = Vec3f(-3.88f * m, 0.6f + 0.2f * sin(0.42f * m), 1.0f);
            Vec3f randColor = hair_math::HSVtoRGB(hsv);
            wisp.SetColor(randColor);
            if (wisp.Width() == 2 && wisp.Length() == 2)
            {
                
            }
            else
            {
                m_vecWisp.push_back(wisp);
            }
            
#ifdef OUTPUT_ERROR_WISP
//            char fileName[260];
//            sprintf(fileName, "error_%04d.obj", i);
//
//            ofstream fout(CHairConfig::AddOutputPrefix(fileName).c_str());
//
//            for (int j=0; j<vGroup.size(); j++)
//            {
//                fout<<"v "<<vertices[vGroup[j]][0]<<" "<<vertices[vGroup[j]][1]<<" "<<vertices[vGroup[j]][2]<<endl;
//                fout<<"vn "<<"0.0"<<" "<<"0.0"<<" "<<"-1.0"<<endl;
//                fout<<"vt "<<texCoords[vGroup[j]][0]<<" "<<texCoords[vGroup[j]][1]<<endl;
//            }
//            
//            for (int j=0; j<fGroup.size(); j++)
//            {
//                int idx1 = vMap[faces[fGroup[j]][0]]+1;
//                int idx2 = vMap[faces[fGroup[j]][1]]+1;
//                int idx3 = vMap[faces[fGroup[j]][2]]+1;
//                fout<<"f "<<idx1<<"/"<<idx1<<"/"<<idx1<<" "<<idx2<<"/"<<idx2<<"/"<<idx2<<" "<<idx3<<"/"<<idx3<<"/"<<idx3<<std::endl;
//            }
#endif
            continue;
        }
        
        CHairWisp wisp;
        wisp.ResizeStrands((int)startVs.size());
        int step = (int)vGroup.size()/(int)startVs.size();
        vector<int> tmpVs = startVs;
        vector<bool> isMarked(vertices.size(), false);
        for (int j=0; j<width; j++)
        {
            CHairSample sample;
            sample.SetPos(vertices[startVs[j]]);
            wisp.GetStrand(j).AddSample(sample);
            isMarked[startVs[j]] = true;
        }
        
        for (int j=1; j<step; j++)
        {
            vector<int> nextVs;
            for (int k=0; k<tmpVs.size(); k++)
            {
                for (int l=0; l<neighbors[tmpVs[k]].size(); l++)
                {
                    if (isMarked[neighbors[tmpVs[k]][l]]==false)
                    {
                        insertNew(nextVs, neighbors[tmpVs[k]][l]);
                        isMarked[neighbors[tmpVs[k]][l]] = true;
                    }
                }
            }

            vector<double> distMatrix(width * width);
            for (int k=0; k<width; k++) {
                for (int l=0; l<width; l++) {
                    if (findV(neighbors[tmpVs[k]], nextVs[l])==true)
                        distMatrix[k * width + l] = 0;
                    else
                        distMatrix[k * width + l] = 100;
                }
            }
            
            vector<int> assignment(width);
            double cost = 0.0;
            CHungarianAlgorithm ha;
            ha.FindOptimalAssignment(assignment, &cost, distMatrix, width, width);

            for (int k=0; k<width; k++)
            {
                CHairSample sample;
                sample.SetPos(vertices[nextVs[assignment[k]-1]]);
                wisp.GetStrand(k).AddSample(sample);
                tmpVs[k] = nextVs[assignment[k]-1];
            }
        }
        
        int m = rand();
        Vec3f hsv = Vec3f(-3.88f * m, 0.6f + 0.2f * sin(0.42f * m), 1.0f);
        Vec3f randColor = hair_math::HSVtoRGB(hsv);
        wisp.SetColor(randColor);
        if (wisp.Width() == 2 && wisp.Length() == 2)
        {
            
        }
        else
        {
            m_vecWisp.push_back(wisp);
        }
    }
    
    UpdateAABB();
    
    return true;
}

void CHairWisps::VerifyDirection(CHairScalp* scalp)
{
    //Vec3f root = Vec3f(0.00042f, 1.89479f, -0.046782f);
    //Vec3f root = Vec3f(-0.003422f, 1.80183f, 0.111984f);
    Vec3f root = Vec3f(-0.00289f, 1.83428f, 0.090914f);
    //Vec3f root = Vec3f(0.065883f, 1.84039f, 0.075905f);
    //Vec3f root = Vec3f(-0.021318f, 1.88626f, -0.069447f);
    //Vec3f root = Vec3f(0.002689f, 1.85257f, -0.1070f);
    //Vec3f root = Vec3f(0.078564f, 1.83935f, -0.049214f);
    //Vec3f root = Vec3f(0.01032f, 1.8852f, 0.0243f);
    for (int i=0; i<m_vecWisp.size(); i++)
    {
        CHairStrand strand = m_vecWisp[i].GetStrand(m_vecWisp[i].Width()/2);
        if (dist2(root, strand.GetSample(0).GetPos()) > dist2(root, strand.GetSample(strand.GetNumOfSamples()-1).GetPos()))
        {
            m_vecWisp[i].Reverse();
        }
    }
//    for (int i=0; i<m_vecWisp.size(); i++)
//    {
//        CHairSample s0 = m_vecWisp[i].GetStrand(m_vecWisp[i].Width()/2).GetSample(0);
//        CHairSample s1 = m_vecWisp[i].GetStrand(m_vecWisp[i].Width()/2).GetSample(m_vecWisp[i].GetStrand(m_vecWisp[i].Width()/2).GetNumOfSamples()-1);
//        CHairSample sonScalp0 = scalp->GetTheNearestSampleOnScalp(s0);
//        CHairSample sonScalp1 = scalp->GetTheNearestSampleOnScalp(s1);
//        if (dist(s0.GetPos(), sonScalp0.GetPos()) > dist(s1.GetPos(), sonScalp1.GetPos()))
//        {
//            m_vecWisp[i].Reverse();
//        }
//    }
}

void CHairWisps::GenerateStrands(CHairStrands* outputstrands, int num)
{
    if (m_vecWisp.size()==0)
        return;
    float wispThickness = 0.003f;
    vector<float> widthPerWisp(m_vecWisp.size());
    vector<int> strandsNumPerWisp(m_vecWisp.size());
    float widthTotal = 0.0f;
    for (int i=0; i<m_vecWisp.size(); i++)
    {
        widthPerWisp[i] = m_vecWisp[i].Width(); //dist( m_vecWisp[i].GetStrand(0).GetSample(0).GetPos(), m_vecWisp[i].GetStrand(m_vecWisp[i].Width()-1).GetSample(0).GetPos() );
        widthTotal += widthPerWisp[i];
    }
    int strandsRemain = num;
    for (int i=0; i<m_vecWisp.size()-1; i++)
    {
        strandsNumPerWisp[i] = widthPerWisp[i]/widthTotal*num;
        strandsRemain -= strandsNumPerWisp[i];
    }
    strandsNumPerWisp[m_vecWisp.size()-1] = strandsRemain;
    
    for (int i=0; i<m_vecWisp.size(); i++)
    {
        vector<float> ws(m_vecWisp[i].Width()-1);
        float wt = 0.0f;
        for (int j=0; j<m_vecWisp[i].Width()-1; j++)
        {
            ws[j] = dist( m_vecWisp[i].GetStrand(j).GetSample(0).GetPos(), m_vecWisp[i].GetStrand(j+1).GetSample(0).GetPos() );
            wt += ws[j];
        }
        for (int j=0 ;j<strandsNumPerWisp[i]; j++)
        {
            float t = float(j)/(strandsNumPerWisp[i] - 1);
            float l = t*wt;
            float w = 0.0f;
            int k;
            for (k=0; k<m_vecWisp[i].Width()-1; k++)
            {
                if (l-ws[k]<=0.0f)
                    break;
                l -= ws[k];
                w += 1.0f;
            }
            
            w += l/ws[k];
            
            w = t*(float)(m_vecWisp[i].Width()-1);
            
            float thick = rand()/(float)RAND_MAX * wispThickness;
            CHairStrand strand;
            m_vecWisp[i].GenerateStrand(w, thick, strand);
            outputstrands->AddStrand(strand);
        }
    }
}
