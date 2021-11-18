// Chongyang Ma - 2013-10-08
// Class implementation of hair strands
// --------------------------------------------------------------
#include "HairStrands.h"
#include <stdlib.h>
#include <limits>
# ifdef max
#undef max
#undef min
#endif
CHairStrands::CHairStrands()
{
	m_aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	m_aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
}

void CHairStrands::UpdateAABB(bool flag)
{
	if ( GetNumOfStrands() == 0 )
	{
		return;
	}
    
	m_aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	m_aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    
	for ( int i=0; i<GetNumOfStrands(); i++ )
	{
		Vec3f aabbMinTmp, aabbMaxTmp;
		GetStrand(i).GetAABB(aabbMinTmp, aabbMaxTmp);
		for ( int j=0; j<3; j++ )
		{
			m_aabbMin[j] = (m_aabbMin[j] <= aabbMinTmp[j]) ? m_aabbMin[j] : aabbMinTmp[j];
			m_aabbMax[j] = (m_aabbMax[j] >= aabbMaxTmp[j]) ? m_aabbMax[j] : aabbMaxTmp[j];
		}
	}
    
    if (flag == true)
    {
        float scale = max(max((m_aabbMax.v[0]-m_aabbMin.v[0]), (m_aabbMax.v[1]-m_aabbMin.v[1])), (m_aabbMax.v[2]-m_aabbMin.v[2]));
        CHairStrand::m_scalingFactor = 2.f / scale;
        CHairStrand::m_trans = -(m_aabbMax + m_aabbMin) * 0.5f * CHairStrand::m_scalingFactor;
    }
}
/*
void CHairStrands::RenderStrandsAsStrokes()
{
    if ( GetNumOfStrands() == 0 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
	glPushMatrix();
    float scale = CHairStrand::m_scalingFactor;
    Vec3f translate = CHairStrand::m_trans;
    glTranslatef(translate[0], translate[1], translate[2]);
    glScalef(scale, scale, scale);
	for ( int i=0; i<GetNumOfStrands(); i++ )
	{
        glPushMatrix();
        glLoadName(i);
		GetStrand(i).RenderStrandAsStroke();
        glPopMatrix();
	}
	glPopMatrix();
}
*/
/*
void CHairStrands::RenderStrandsAsLineSegmentsFlipX(int interval)
{
	if ( GetNumOfStrands() == 0 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
	glPushMatrix();
    float scale = CHairStrand::m_scalingFactor;
    Vec3f translate = CHairStrand::m_trans;
    glTranslatef(translate[0], translate[1], translate[2]);
    glScalef(scale, scale, scale);
	for ( int i=0; i<GetNumOfStrands(); i=i+interval )
	{
        //        if (GetStrand(i).GetSample(50).GetPos()[2]<0.0f)
        //            continue;
		GetStrand(i).RenderStrandAsLineSegmentsFlipX();
	}
	glPopMatrix();
}
*/
/*
void CHairStrands::RenderStrandsAsLineSegments(int interval)
{
	if ( GetNumOfStrands() == 0 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
	glPushMatrix();
    float scale = CHairStrand::m_scalingFactor;
    Vec3f translate = CHairStrand::m_trans;
    glTranslatef(translate[0], translate[1], translate[2]);
    glScalef(scale, scale, scale);
	for ( int i=0; i<GetNumOfStrands(); i=i+interval )
	{
//        if (GetStrand(i).GetSample(50).GetPos()[2]<0.0f)
//            continue;
		GetStrand(i).RenderStrandAsLineSegments();
	}
	glPopMatrix();
}

void CHairStrands::RenderStrandRootsAsSpheres()
{
    Vec3f minAABB = GetAABBmin();
    Vec3f maxAABB = GetAABBmax();
    glPushMatrix();
    float scale = max(max((maxAABB.v[0]-minAABB.v[0]), (maxAABB.v[1]-minAABB.v[1])), (maxAABB.v[2]-minAABB.v[2]));
	glScalef(2.0f/scale, 2.0f/scale, 2.0f/scale);
    glTranslatef(-(maxAABB.v[0]+minAABB[0])/2.0f, -(maxAABB.v[1]+minAABB[1])/2.0f, -(maxAABB.v[2]+minAABB[2])/2.0f);
    for ( int i=0; i<GetNumOfStrands(); i++ )
    {
        glPushMatrix();
        glLoadName(i);
        glColor3f(0.8f, 0.2f, 0.2f);
        Vec3f posRoot = GetStrand(i).GetSample(0).GetPos();
        glTranslatef(posRoot[0], posRoot[1], posRoot[2]);
        if (GetStrand(i).GetLength(0)>100)
        glutSolidSphere(1.0f, 60, 60);
        glPopMatrix();
    }
    glPopMatrix();
}

void CHairStrands::RenderStrandsAsCylindricalSegments()
{
	if ( GetNumOfStrands() == 0 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
    
	glPushMatrix();
    Vec3f trans = CHairStrand::m_trans;
	glTranslatef(trans[0], trans[1], trans[2]);
	for ( int i=0; i<GetNumOfStrands(); i++ )
	{
		GetStrand(i).RenderStrandAsCylindricalSegments();
	}
	glPopMatrix();
}
*/
void CHairStrands::RandomSelect(CHairStrands *outputStrands, int seedNum)
{
    int num = GetNumOfStrands();
    int *randSeed = (int *)malloc(sizeof(int)*num);
    int *rec = (int *)malloc(sizeof(int)*num);
    for (int i=0;i<num;i++) {
        rec[i] = i;
    }
    for (int i=0;i<num;i++) {
        int index = i+(int)((((float)rand())/(float)RAND_MAX)*(num-i));
        randSeed[i] = rec[index];
        rec[index] = rec[i];
    }
    for (int i=0; i<seedNum; i++) {
        CHairStrand strand = GetStrand(randSeed[i]);
        outputStrands->AddStrand(strand);
    }
    free(rec);
    free(randSeed);
}

void CHairStrands::FlipX()
{
    for (int i = 0; i < m_vecStrand.size(); i++)
    {
        m_vecStrand[i] = m_vecStrand[i].FlipX();
    }
}

void CHairStrands::ScaleStrands(float k)
{
    for (int i=0; i<GetNumOfStrands(); i++)
    {
        CHairStrand& s = GetStrand(i);
        for (int j=0; j<s.GetNumOfSamples(); j++)
        {
            s.GetSample(j).SetPos(s.GetSample(j).GetPos() * k);
        }
    }
}

void CHairStrands::ScaleStrands(Vec3f k)
{
    for (int i=0; i<GetNumOfStrands(); i++)
    {
        CHairStrand& s = GetStrand(i);
        for (int j=0; j<s.GetNumOfSamples(); j++)
        {
            s.GetSample(j).SetPos(s.GetSample(j).GetPos() * k);
        }
    }
}

void CHairStrands::TranslateStrands(Vec3f trans)
{
    for (int i=0; i<GetNumOfStrands(); i++)
    {
        CHairStrand& s = GetStrand(i);
        for (int j=0; j<s.GetNumOfSamples(); j++)
        {
            s.GetSample(j).SetPos(s.GetSample(j).GetPos() + trans);
        }
    }
}

void CHairStrands::FindNearestStrand(Vec3f p, CHairStrand& strand)
{
    int idx;
    float distMin = std::numeric_limits<float>::max();
    for (int i = 0; i < m_vecStrand.size(); i++)
    {
        Vec3f root = m_vecStrand[i].GetSample(0).GetPos();
        float d = dist2(p, root);
        if ( d < distMin )
        {
            distMin = d;
            idx = i;
        }
    }
    strand = m_vecStrand[idx];
}

bool CHairStrands::DumpStrandsAsBinaryData(string fileName)
{
	FILE* f = fopen(fileName.c_str(), "wb");
	if (!f) {
        fprintf(stderr, "Couldn't write %s\n", fileName.c_str());
        return false;
	}
    
	int nstrands = GetNumOfStrands();
	fwrite(&nstrands, 4, 1, f);
	printf("writing %d strands...\n", nstrands);
    
	for ( int i=0; i<nstrands; i++ )
    {
	    int nverts = GetStrand(i).GetNumOfSamples();
	    fwrite(&nverts, 4, 1, f);
	    for ( int j=0; j<nverts; j++ )
        {
            Vec3f pj = GetStrand(i).GetSample(j).GetPos();
            fwrite(&pj[0], 12, 1, f);
	    }
	}
    fclose(f);
    return true;
}

bool CHairStrands::DumpStrandsWithSegmentColor(string fileName)
{
    CHairStrands* outputStrands = new CHairStrands();
    vector<Vec3f> strandColors;
    for (int i = 0; i < m_vecStrand.size(); i++)
    {
        CHairStrand strand = m_vecStrand[i];
        vector<Vec3f> colors = strand.GetVerPerSampleColor();
        CHairStrand add;
        add.AddSample(strand.GetSample(0));
        Vec3f last = colors[0];
        for (int j = 1; j < strand.GetNumOfSamples(); j++)
        {
            add.AddSample(strand.GetSample(j));
            if (fabs(last[0] - colors[j][0]) < 1e-4 &&
                fabs(last[1] - colors[j][1]) < 1e-4 &&
                fabs(last[2] - colors[j][2]) < 1e-4)
            {
                
            }
            else
            {
                if (add.GetNumOfSamples() > 1)
                {
                    outputStrands->AddStrand(add);
                    strandColors.push_back(last);
                }
                add.Clear();
                add.AddSample(strand.GetSample(j));
                last = colors[j];
            }
            if (colors[j][0] == 0.0f && colors[j][1] == 0.0f && colors[j][2] == 0.0f)
                break;
        }
        if (add.GetNumOfSamples() > 1)
        {
            outputStrands->AddStrand(add);
            strandColors.push_back(last);
        }
    }
    outputStrands->DumpStrandsAsBinaryData(fileName);
    ofstream fout(CHairConfig::AddOutputPrefix("colors.txt").c_str());
    for (int i = 0; i < strandColors.size(); i++)
    {
        fout<<strandColors[i]<<endl;
    }
    fout.close();
    return true;
}

bool CHairStrands::DumpStrandsToTXT(string fileName)
{
	ofstream fout(fileName.c_str());
	if ( fout.fail() == true )
	{
		cout << "Failed to dump strands to file " << fileName << "!\n";
		return false;
	}
	int numOfStrands = GetNumOfStrands();
	fout << numOfStrands << endl;
	for ( int n=0; n<numOfStrands; n++ )
	{
		CHairStrand& strand = GetStrand(n);
		int numOfSamples = strand.GetNumOfSamples();
		fout << numOfSamples << " ";
		for ( int i=0; i<numOfSamples; i++ )
		{
			Vec3f pos = strand.GetSample(i).GetPos();
			fout << pos[0] << " " << pos[1] << " " << pos[2] << " ";
		}
        fout<<endl;
	}
	return true;
}

bool CHairStrands::DumpStrandsToOBJ(string fileName, Scalar rad, int sampleNum, int interval , int interp)
{
	ofstream fout(fileName.c_str());
	if ( fout.fail() )
	{
		cout << "Failed to dump strands to " << fileName << "!\n";
		return false;
	}
    
	int vAccum = 0;
	for ( int i=0; i<GetNumOfStrands(); i++ )
	{
		CHairStrand& strand = GetStrand(i);
		DumpStrandToOBJWithColor(strand, fout, vAccum, rad, sampleNum, interval);
	}
    
	return true;
}

void CHairStrands::GetShortStrands(int num, CHairStrands* outputStrands)
{
    for (int i = 0; i < GetNumOfStrands(); i++)
    {
        CHairStrand& strand = GetStrand(i);
        if (strand.GetNumOfSamples() <= num)
        {
            outputStrands->AddStrand(strand);
        }
        else
        {
            CHairStrand ostrand;
            for (int j = 0; j <= num; j++)
            {
                ostrand.AddSample(strand.GetSample(j));
            }
            outputStrands->AddStrand(ostrand);
        }
    }
}

void CHairStrands::DumpStrandToOBJWithColor(CHairStrand& strand, ofstream& fout, int& vAccum, Scalar rad, int sampleNum, int interval)
{
    vector<Vec3f> colors;
    if (strand.GetVerPerSampleColor().size() == strand.GetNumOfSamples())
    {
        colors = strand.GetVerPerSampleColor();
    }
    else
    {
        colors.resize(strand.GetNumOfSamples());
        for (int i = 0; i < strand.GetNumOfSamples(); i++)
        {
            colors[i] = strand.GetColor();
        }
    }
	vector<Vec3f> vecVertices;
	int vCnt = 0;
	int ptNum = strand.GetNumOfSamples();
	Scalar angleDelta = TWO_PI / Scalar(sampleNum);
	Vec3f pr, pi;
	int iMax = -1;
	for ( int i=0; i<ptNum; i+=interval )
	{
		pi = strand.GetSample(i).GetPos();
		iMax = max(i, iMax);
		if ( i < ptNum - interval )
		{
			pr = strand.GetSample(i+interval).GetPos() - pi;
		}
        normalize(pr);
		Vec3f d1 = fabs(dot(pr, Vec3f(0.f, 0.f, 1.f)))<0.99f?cross(pr, Vec3f(0.f, 0.f, 1.f)):cross(pr, Vec3f(0.f, 1.f, 0.f));
		Vec3f d2 = cross(d1, pr);
		normalize(d1);
		normalize(d2);
		Scalar angleVal = 0.f;
		for ( int j=0; j<sampleNum; j++ )
		{
			Scalar cv = cos(angleVal);
			Scalar sv = sin(angleVal);
			Vec3f pj = pi + rad * (cv * d1 + sv * d2);
			fout << "v " << pj << " " << colors[i] << endl;
			vecVertices.push_back(pj);
			angleVal += angleDelta;
			vCnt ++;
		}
	}
	Scalar s = 0.f; //0.5f; // Weight for extrapolation
	Vec3f v0 = (1.f + s) * strand.GetSample(0).GetPos() - s * strand.GetSample(interval).GetPos();
	Vec3f v1 = (1.f + s) * pi - s * strand.GetSample(iMax - interval).GetPos();
	fout << "v " << v0 << " " << colors[0] <<endl;
	fout << "v " << v1 << " " << colors[colors.size() - 1] << endl;
	vecVertices.push_back(v0);
	vecVertices.push_back(v1);
    
	int sampleCnt = 0;
	for ( int i=0; i<ptNum-interval; i+=interval )
	{
		vector<int> vecIndices0;
		vector<int> vecIndices1;
		vector<int> vecIndices2;
		vector<int> vecIndices3;
		for ( int j=0; j<sampleNum; j++ )
		{
			int idx0 = vAccum + j + 1 + sampleCnt;
			int idx1 = vAccum + (j + 1) % sampleNum + 1 + sampleCnt;
			int idx2 = idx0 + sampleNum;
			int idx3 = idx1 + sampleNum;
			vecIndices0.push_back(idx0);
			vecIndices1.push_back(idx1);
			vecIndices2.push_back(idx2);
			vecIndices3.push_back(idx3);
		}
		Scalar distSumMin = std::numeric_limits<Scalar>::max();
		int bestShift = 0;
		for ( int j=0; j<sampleNum; j++ )
		{
			Scalar distSumTmp = 0.f;
			for ( int k=0; k<sampleNum; k++ )
			{
				Vec3f& p0 = vecVertices[vecIndices0[k] - vAccum - 1];
				Vec3f& p2 = vecVertices[vecIndices2[(k + j) % sampleNum] - vAccum - 1];
				distSumTmp += dist2(p0, p2);
			}
			if ( distSumTmp < distSumMin )
			{
				distSumMin = distSumTmp;
				bestShift = j;
			}
		}
		for ( int j=0; j<sampleNum; j++ )
		{
			int idx0 = vecIndices0[j];
			int idx1 = vecIndices1[j];
			int idx2 = vecIndices2[(j + bestShift) % sampleNum];
			int idx3 = vecIndices3[(j + bestShift) % sampleNum];
			fout << "f " << idx0 << " " << idx3 << " " << idx1 << endl;
			fout << "f " << idx0 << " " << idx2 << " " << idx3 << endl;
		}
		sampleCnt += sampleNum;
	}
    
	// Close the two ends...
	for ( int j=0; j<sampleNum; j++ )
	{
		int idx0 = vAccum + vCnt + 1;
		int idx1 = vAccum + j + 1;
		int idx2 = vAccum + (j + 1) % sampleNum + 1;
		fout << "f " << idx0 << " " << idx1 << " " << idx2 << endl;
	}
	for ( int j=0; j<sampleNum; j++ )
	{
		int idx0 = vAccum + vCnt + 2;
		int idx1 = vAccum + vCnt - sampleNum + (j + 1) % sampleNum + 1;
		int idx2 = vAccum + vCnt - sampleNum + j + 1;
		fout << "f " << idx0 << " " << idx1 << " " << idx2 << endl;
	}
    
	vCnt += 2;
	vAccum += vCnt;
}

void CHairStrands::DumpStrandToOBJ(CHairStrand& strand, ofstream& fout, int& vAccum, Scalar rad, int sampleNum, int interval)
{
	vector<Vec3f> vecVertices;
	int vCnt = 0;
	int ptNum = strand.GetNumOfSamples();
	Scalar angleDelta = TWO_PI / Scalar(sampleNum);
	Vec3f pr, pi;
	int iMax = -1;
	for ( int i=0; i<ptNum; i+=interval )
	{
		pi = strand.GetSample(i).GetPos();
		iMax = max(i, iMax);
		if ( i < ptNum - interval )
		{
			pr = strand.GetSample(i+interval).GetPos() - pi;
		}
		Vec3f d1 = cross(pr, Vec3f(0.f, 0.f, 1.f));
		Vec3f d2 = cross(d1, pr);
		normalize(d1);
		normalize(d2);
		Scalar angleVal = 0.f;
		for ( int j=0; j<sampleNum; j++ )
		{
			Scalar cv = cos(angleVal);
			Scalar sv = sin(angleVal);
			Vec3f pj = pi + rad * (cv * d1 + sv * d2);
			fout << "v " << pj << endl;
			vecVertices.push_back(pj);
			angleVal += angleDelta;
			vCnt ++;
		}
	}
	Scalar s = 0.f; //0.5f; // Weight for extrapolation
	Vec3f v0 = (1.f + s) * strand.GetSample(0).GetPos() - s * strand.GetSample(interval).GetPos();
	Vec3f v1 = (1.f + s) * pi - s * strand.GetSample(iMax - interval).GetPos();
	fout << "v " << v0 << endl;
	fout << "v " << v1 << endl;
	vecVertices.push_back(v0);
	vecVertices.push_back(v1);
    
	int sampleCnt = 0;
	for ( int i=0; i<ptNum-interval; i+=interval )
	{
		vector<int> vecIndices0;
		vector<int> vecIndices1;
		vector<int> vecIndices2;
		vector<int> vecIndices3;
		for ( int j=0; j<sampleNum; j++ )
		{
			int idx0 = vAccum + j + 1 + sampleCnt;
			int idx1 = vAccum + (j + 1) % sampleNum + 1 + sampleCnt;
			int idx2 = idx0 + sampleNum;
			int idx3 = idx1 + sampleNum;
			vecIndices0.push_back(idx0);
			vecIndices1.push_back(idx1);
			vecIndices2.push_back(idx2);
			vecIndices3.push_back(idx3);
		}
		Scalar distSumMin = std::numeric_limits<Scalar>::max();
		int bestShift = 0;
		for ( int j=0; j<sampleNum; j++ )
		{
			Scalar distSumTmp = 0.f;
			for ( int k=0; k<sampleNum; k++ )
			{
				Vec3f& p0 = vecVertices[vecIndices0[k] - vAccum - 1];
				Vec3f& p2 = vecVertices[vecIndices2[(k + j) % sampleNum] - vAccum - 1];
				distSumTmp += dist2(p0, p2);
			}
			if ( distSumTmp < distSumMin )
			{
				distSumMin = distSumTmp;
				bestShift = j;
			}
		}
		for ( int j=0; j<sampleNum; j++ )
		{
			int idx0 = vecIndices0[j];
			int idx1 = vecIndices1[j];
			int idx2 = vecIndices2[(j + bestShift) % sampleNum];
			int idx3 = vecIndices3[(j + bestShift) % sampleNum];
			fout << "f " << idx0 << " " << idx3 << " " << idx1 << endl;
			fout << "f " << idx0 << " " << idx2 << " " << idx3 << endl;
		}
		sampleCnt += sampleNum;
	}
    
	// Close the two ends...
	for ( int j=0; j<sampleNum; j++ )
	{
		int idx0 = vAccum + vCnt + 1;
		int idx1 = vAccum + j + 1;
		int idx2 = vAccum + (j + 1) % sampleNum + 1;
		fout << "f " << idx0 << " " << idx1 << " " << idx2 << endl;
	}
	for ( int j=0; j<sampleNum; j++ )
	{
		int idx0 = vAccum + vCnt + 2;
		int idx1 = vAccum + vCnt - sampleNum + (j + 1) % sampleNum + 1;
		int idx2 = vAccum + vCnt - sampleNum + j + 1;
		fout << "f " << idx0 << " " << idx1 << " " << idx2 << endl;
	}
    
	vCnt += 2;
	vAccum += vCnt;
}

bool CHairStrands::LoadHairStrandsFromTXT(string fileName)
{
	ifstream fin(fileName.c_str());
	if ( fin.fail() == true )
	{
		cout << "Failed to load strands from TXT file " << fileName << "!\n";
		return false;
	}
	int numOfStrands;
	fin >> numOfStrands;
	ResizeStrands(numOfStrands);
	for ( int n=0; n<numOfStrands; n++ )
	{
		int numOfSamples;
		fin >> numOfSamples;
		GetStrand(n).ResizeStrand(numOfSamples);
		for ( int i=0; i<numOfSamples; i++ )
		{
			float px, py, pz;
			fin >> px >> py >> pz;
			Vec3f pos = Vec3f(px, py, pz);
			GetStrand(n).GetSample(i).SetPos(pos);
		}
        Vec3f randColor((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
        GetStrand(n).SetColor(randColor);
	}
    UpdateAABB();

	return true;
}

bool CHairStrands::LoadHairStrands(const char* fileName)
{
	FILE *f = fopen(fileName, "rb");
	if ( !f )
	{
		fprintf(stderr, "Couldn't read hair strands from %s\n", fileName);
		return false;
	}

	int nstrands = 0;
	if ( !fread(&nstrands, 4, 1, f) )
	{
		fprintf(stderr, "Couldn't read number of strands\n");
		fclose(f);
		return false;
	}
    ResizeStrands(nstrands);
    
    int strandCount = 0;
	for ( int i=0; i<nstrands; i++ )
	{
		int nverts = 0;
		if ( !fread(&nverts, 4, 1, f) )
		{
			fprintf(stderr, "Couldn't read number of vertices\n");
			fclose(f);
			return false;
		}
        CHairStrand strand;
		strand.ResizeStrand(nverts);
		vector<Vec3f> strandData(nverts);
		for ( int j=0; j<nverts; j++ )
		{
			if ( !fread(&strandData[j][0], 12, 1, f) )
			{
				fprintf(stderr, "Couldn't read vertex\n");
				fclose(f);
				return false;
			}
			strand.GetSample(j).SetPos(strandData[j]);
		}
        Vec3f randColor((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
        strand.SetColor(randColor);
        GetStrand(strandCount) = strand;
        strandCount ++;
	}
	fclose(f);

	return true;
}

bool CHairStrands::LoadChaiData(const char* fileName)
{
    FILE *f = fopen(fileName, "rb");
    if ( !f )
	{
		fprintf(stderr, "Couldn't read hair strands from %s\n", fileName);
		return false;
	}
    
	int nstrands = 0;
	if ( !fread(&nstrands, 4, 1, f) )
	{
		fprintf(stderr, "Couldn't read number of strands\n");
		fclose(f);
		return false;
	}
    ResizeStrands(nstrands);
    int strandCount = 0;
	for ( int i=0; i<nstrands; i++ )
	{
		int nverts = 0;
		if ( !fread(&nverts, 4, 1, f) )
		{
			fprintf(stderr, "Couldn't read number of vertices\n");
			fclose(f);
			return false;
		}
        CHairStrand strand;
		strand.ResizeStrand(nverts);
		vector<Vec3f> strandData(nverts);
		for ( int j=0; j<nverts; j++ )
		{
			if ( !fread(&strandData[j][0], 12, 1, f) )
			{
				fprintf(stderr, "Couldn't read vertex\n");
				fclose(f);
				return false;
			}
            Vec4f c;
            fread(&c, 16, 1, f);
			strand.GetSample(j).SetPos(strandData[j]);
		}
        Vec3f randColor((float)rand()/RAND_MAX, (float)rand()/RAND_MAX, (float)rand()/RAND_MAX);
        strand.SetColor(randColor);
        GetStrand(strandCount) = strand;
        strandCount ++;
	}
	fclose(f);
    
	return true;
}

void CHairStrands::RemoveZeroStrands()
{
    for (int i = 0; i < m_vecStrand.size();)
    {
        if (m_vecStrand[i].GetNumOfSamples() == 0 || m_vecStrand[i].GetNumOfSamples() == 1 || m_vecStrand[i].GetSample(0).GetPos()[0] != m_vecStrand[i].GetSample(0).GetPos()[0])
        {
            m_vecStrand.erase(m_vecStrand.begin() + i);
        }
        else
        {
            i++;
        }
    }
}

struct compareStrand {
    bool operator() (Vec2f strand0, Vec2f strand1) {
        return strand0[0] > strand1[0];
    }
} myCompareStrand;

void CHairStrands::SortAllStrandsByLength()
{
    vector<Vec2f> length(m_vecStrand.size());
    for (int i = 0; i < m_vecStrand.size(); i++) {
        length[i][0] =  m_vecStrand[i].Length();
        length[i][1] = i;
    }
    std::sort(length.begin(), length.end(), myCompareStrand);
    vector<CHairStrand> vecTempStrand(m_vecStrand.size());
    for (int i = 0; i < m_vecStrand.size(); i++)
    {
        vecTempStrand[i] = m_vecStrand[length[i][1]];
    }
    m_vecStrand = vecTempStrand;
}

void CHairStrands::GetStrandsAroundSegment(Vec3f endpoint1, Vec3f endpoint2, float radius, vector<int>& indices, vector<bool>& isMasked)
{
    for (int i=0; i<m_vecStrand.size(); i++) {
        if (isMasked[i] == false) {
            bool mask = false;
            for (int j=0; j<indices.size(); j++) {
                if (indices[j]==i) {
                    mask = true;
                    break;
                }
            }
            if (mask == false) {
                for (int j=0; j<m_vecStrand[i].GetNumOfSamples(); j++) {
                    Vec3f pos = m_vecStrand[i].GetSample(j).GetPos();
                    Vec3f v0 = endpoint1;
                    Vec3f v1 = endpoint2;
                    float u = ((v1[0]-v0[0])*(pos[0]-v0[0])+(v1[1]-v0[1])*(pos[1]-v0[1])+(v1[2]-v0[2])*(pos[2]-v0[2]))/
                    ((v1[0]-v0[0])*(v1[0]-v0[0])+(v1[1]-v0[1])*(v1[1]-v0[1])+(v1[2]-v0[2])*(v1[2]-v0[2]));
                    if (u>-0.5f&&u<1.5f) {
                        Vec3f v = v0+(v1-v0)*u;
                        float l = dist(pos, v);
                        if (l<radius) {
                            indices.push_back(i);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void CHairStrands::CheckStrands()
{
    for (int i = 0; i < GetNumOfStrands(); i++)
    {
        CHairStrand& strand = GetStrand(i);
        for (int j = 0; j < strand.GetNumOfSamples(); j++)
        {
            Vec3f p = strand.GetSample(j).GetPos();
            if (p[0] != p[0] || p[1] != p[1] || p[2] != p[2])
            {
                strand.Clear();
                break;
            }
        }
    }
    RemoveZeroStrands();
}

/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#define B 0x100
#define BM 0xff

#define N 0x1000
#define NP 12   /* 2^N */
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);

#define s_curve(t) ( t * t * (3. - 2. * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

#define setup(i,b0,b1,r0,r1)\
t = vec[i] + N;\
b0 = ((int)t) & BM;\
b1 = (b0+1) & BM;\
r0 = t - (int)t;\
r1 = r0 - 1.;

double noise1(double arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];
    
	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}
    
	setup(0, bx0,bx1, rx0,rx1);
    
	sx = s_curve(rx0);
    
	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];
    
	return lerp(sx, u, v);
}

float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;
    
	if (start) {
		start = 0;
		init();
	}
    
	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
    
	i = p[ bx0 ];
	j = p[ bx1 ];
    
	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];
    
	sx = s_curve(rx0);
	sy = s_curve(ry0);
    
#define at2(rx,ry) ( rx * q[0] + ry * q[1] )
    
	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = lerp(sx, u, v);
    
	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = lerp(sx, u, v);
    
	return lerp(sy, a, b);
}

float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;
    
	if (start) {
		start = 0;
		init();
	}
    
	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);
    
	i = p[ bx0 ];
	j = p[ bx1 ];
    
	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];
    
	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);
    
#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )
    
	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = lerp(t, u, v);
    
	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = lerp(t, u, v);
    
	c = lerp(sy, a, b);
    
	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = lerp(t, u, v);
    
	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = lerp(t, u, v);
    
	d = lerp(sy, a, b);
    
	return lerp(sz, c, d);
}

static void normalize2(float v[2])
{
	float s;
    
	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;
    
	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

static void init(void)
{
	int i, j, k;
    
	for (i = 0 ; i < B ; i++) {
		p[i] = i;
        
		g1[i] = (float)((rand() % (B + B)) - B) / B;
        
		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);
        
		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}
    
	while (--i) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}
    
	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}

void CHairStrands::AddNoise()
{
    for (int i=0; i<GetNumOfStrands(); i++) {
        CHairStrand &strand = GetStrand(i);
        float randId = rand()/(float)RAND_MAX*10.0f;
        for (int k=50; k<strand.GetNumOfSamples(); k++)
        {
            Vec3f p = strand.GetSample(k).GetPos();
            float e0[2] = {p[0]*50.0f, randId};
            float e1[2] = {p[1]*50.0f, randId};
            float e2[2] = {p[2]*50.0f, randId};
            Vec3f e = Vec3f(noise2(e0), noise2(e1), noise2(e2));
            e = e*0.01f;
            strand.GetSample(k).SetPos(strand.GetSample(k).GetPos()+e);
        }
    }
}

void CHairStrands::AddNoise(CHairStrand strand, CHairStrand& outputStrand)
{
    outputStrand = strand;
    float randId = rand()/(float)RAND_MAX*10.0f;
    for (int k=0; k<outputStrand.GetNumOfSamples(); k++)
    {
        Vec3f p = outputStrand.GetSample(k).GetPos();
        float e0[2] = {p[0]/50.0f, randId};
        float e1[2] = {p[1]/50.0f, randId};
        float e2[2] = {p[2]/50.0f, randId};
        Vec3f e = Vec3f(noise2(e0), noise2(e1), noise2(e2));
        e = e*5.0f;
        outputStrand.GetSample(k).SetPos(outputStrand.GetSample(k).GetPos()+e);
    }
}