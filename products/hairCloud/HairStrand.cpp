// Chongyang Ma - 2013-10-08
// Class implementation of a single hair strand
// --------------------------------------------------------------

#include "HairStrand.h"

# ifdef max
#undef max
#undef min
#endif
//GLUquadric* CHairStrand::m_ptrQuadric = NULL;
Vec3f CHairStrand::m_trans = Vec3f(0.f, 0.f, 0.f);
float CHairStrand::m_scalingFactor = 1.f;

CHairStrand::CHairStrand()
{
    m_color = Vec3f(0.7f, 0.7f, 0.7f);
}
/*
void CHairStrand::RenderStrandAsLineSegmentsFlipX()
{
    if ( GetNumOfSamples() < 2 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
    glLineWidth(1.f);
    if (GetNumOfSamples() == m_vecPerSampleColor.size())
    {
        glBegin(GL_LINE_STRIP);
        for ( int i=0; i<GetNumOfSamples(); i++ )
        {
            Vec3f pi = GetSample(i).GetPos();
            Vec3f cl = m_vecPerSampleColor[i];
            glColor3f(cl[0], cl[1], cl[2]);
            glVertex3f(-pi[0], pi[1], pi[2]);
        }
        glEnd();
    }
    else
    {
        //glDisable(GL_BLEND);
        glBegin(GL_LINE_STRIP);
        for ( int i=0; i<GetNumOfSamples(); i++ )
        {
            glColor3f((float)(i)/(GetNumOfSamples())*119.0f/255.0f+74.0f/255.0f,
                      (float)(i)/(GetNumOfSamples())*-37.0f/255.0f+133.0f/255.0f,
                      (float)(i)/(GetNumOfSamples())*9.0f/255.0f+166.0f/255.0f);
            //            glColor3f(m_color[0], m_color[1], m_color[2]);
            Vec3f pi = GetSample(i).GetPos();
            glVertex3f(-pi[0], pi[1], pi[2]);
        }
        glEnd();
    }
}
*/
/*
void CHairStrand::RenderStrandAsLineSegments()
{
    if ( GetNumOfSamples() < 2 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
    glLineWidth(1.f);
    if (GetNumOfSamples() == m_vecPerSampleColor.size())
    {
        glBegin(GL_LINE_STRIP);
        for ( int i=0; i<GetNumOfSamples(); i++ )
        {
            Vec3f pi = GetSample(i).GetPos();
            Vec3f cl = m_vecPerSampleColor[i];
            glColor3f(cl[0], cl[1], cl[2]);
            glVertex3f(pi[0], pi[1], pi[2]);
        }
        glEnd();
    }
    else
    {
        //glDisable(GL_BLEND);
        glBegin(GL_LINE_STRIP);
        for ( int i=0; i<GetNumOfSamples(); i++ )
        {
            glColor3f((float)(i)/(GetNumOfSamples())*119.0f/255.0f+74.0f/255.0f,
                      (float)(i)/(GetNumOfSamples())*-37.0f/255.0f+133.0f/255.0f,
                      (float)(i)/(GetNumOfSamples())*9.0f/255.0f+166.0f/255.0f);
//            glColor3f(m_color[0], m_color[1], m_color[2]);
            Vec3f pi = GetSample(i).GetPos();
            glVertex3f(pi[0], pi[1], pi[2]);
        }
        glEnd();
    }
}
*/
/*
void CHairStrand::RenderStrandAsStroke()
{
    if ( GetNumOfSamples() <= 1 )
	{
		return;
	}
    
	glShadeModel(GL_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
    
    glLineWidth(3.f);
	glColor3f(0.8f, 0.2f, 0.2f);
	glBegin(GL_LINE_STRIP);
	for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		Vec3f pi = GetSample(i).GetPos();
		glVertex3f(pi[0], pi[1], pi[2]);
	}
	glEnd();
    
    glEnable(GL_DEPTH_TEST);
}
*/
/*
void CHairStrand::RenderStrandAsCylindricalSegments()
{
	int numOfSamples = GetNumOfSamples();
	if ( m_ptrQuadric == NULL ) m_ptrQuadric = gluNewQuadric();
	const Scalar cylinderWd = 0.004f;
	//const Scalar sphereRad = 0.01f;
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat objAmbient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat objDiffuse[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat objSpecular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat objHLAmbient[]  = { 1.0f, 0.0f, 0.0f, 1.0f };
	GLfloat objHLDiffuse[]  = { 0.5f, 0.0f, 0.0f, 1.0f };
	GLfloat objHLSpecular[] = { 0.1f, 0.0f, 0.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT,  objAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,  objDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, objSpecular);
	glMateriali(GL_FRONT, GL_SHININESS, 0.5f);
	glPushMatrix();
	//glTranslatef(trans[0], trans[1], trans[2]);
	for ( int i=0; i<numOfSamples-1; i++ )
	{
        int idx0 = i;
        int idx1 = i + 1;
		Vec3f p1 = GetSample(idx0).GetPos();
		Vec3f p2 = GetSample(idx1).GetPos();
		//Vec3f pMean = (p1 + p2) * 0.5f;
		Vec3f pd = p2 - p1;
		normalize(pd);
		Vec3f v1 = Vec3f(1.0f, 0.0f, 0.0f);
		Vec3f v2 = cross(v1, pd);
		normalize(v2);
		Vec3f v3 = cross(pd, v2);
		Scalar mat[16];
		for ( int n=0; n<16; n++ ) mat[n] = 0.0f;
		mat[15] = 1.0f;
		mat[0] = v2[0]; mat[1] = v2[1]; mat[2] = v2[2];
		mat[4] = v3[0]; mat[5] = v3[1]; mat[6] = v3[2];
		mat[8] = pd[0]; mat[9] = pd[1]; mat[10]= pd[2];
		glPushMatrix();
		Vec3f trans = p1 * m_scalingFactor;
		glTranslatef(trans[0], trans[1], trans[2]);
		glMultMatrixf(mat);
		gluCylinder(m_ptrQuadric, cylinderWd, cylinderWd, dist(p1, p2) * m_scalingFactor, 16, 8);
		glPopMatrix();
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT,  objHLAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,  objHLDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, objHLSpecular);
	glMateriali(GL_FRONT, GL_SHININESS, 0.5f);
	for ( int i=0; i<numOfSamples; i++ )
	{
		Vec3f pi = GetSample(i).GetPos();
		glPushMatrix();
		glTranslatef(pi[0], pi[1], pi[2]);
		//gluSphere(m_ptrQuadric, sphereRad, 10, 10);
		glPopMatrix();
	}
	glPopMatrix();
	glDisable(GL_LIGHTING);
}
*/

void CHairStrand::GetAABB(Vec3f& aabbMin, Vec3f& aabbMax)
{
	if ( GetNumOfSamples() == 0 )
	{
		return;
	}
	aabbMin = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	aabbMax = Vec3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		Vec3f pi = GetSample(i).GetPos();
		for ( int j=0; j<3; j++ )
		{
			aabbMin[j] = (aabbMin[j] <= pi[j]) ? aabbMin[j] : pi[j];
			aabbMax[j] = (aabbMax[j] >= pi[j]) ? aabbMax[j] : pi[j];
		}
	}
}

Vec3f CHairStrand::ComputeRefTranslation(CHairStrand& refStrand, Vec3f cameraPos, Vec3f cameraDir)
{
    Vec3f dir;
    float minDist = std::numeric_limits<Scalar>::max();
    for (int i = 0; i < GetNumOfSamples(); i++)
    {
        float tempMinDist = std::numeric_limits<Scalar>::max();
        int idx;
        Vec3f p, q;
        p = GetSample(i).GetPos();
        p *= m_scalingFactor;
        p += m_trans;
        for (int j = 0; j < refStrand.GetNumOfSamples(); j++)
        {
            q = refStrand.GetSample(j).GetPos();
            q *= m_scalingFactor;
            q += m_trans;
            if (dist2(p, q) < tempMinDist)
            {
                idx = j;
                tempMinDist = dist2(p, q);
            }
        }
        q = refStrand.GetSample(idx).GetPos();
        q *= m_scalingFactor;
        q += m_trans;
        if (dist2(p, cameraPos) > dist2(q, cameraPos))
        {
            float di = dist2(q, cameraPos);
            float a = (cameraDir[0] * cameraDir[0] + cameraDir[1] * cameraDir[1] + cameraDir[2] * cameraDir[2]);
            float b = -2 * cameraPos[0] * cameraDir[0] + 2 * p[0] * cameraDir[0]
                - 2 * cameraPos[1] * cameraDir[1] + 2 * p[1] * cameraDir[1]
                - 2 * cameraPos[2] * cameraDir[2] + 2 * p[2] * cameraDir[2];
            float c = cameraPos[0] * cameraPos[0] - 2 * cameraPos[0] * p[0] + p[0] * p[0]
                + cameraPos[1] * cameraPos[1] - 2 * cameraPos[1] * p[1] + p[1] * p[1]
                + cameraPos[2] * cameraPos[2] - 2 * cameraPos[2] * p[2] + p[2] * p[2] - di;
            float k = (-b + sqrt(b*b-4*a*c)) / (2 * a);
            if (k < minDist)
            {
                minDist = k;
            }
        }
    }
    Vec3f d = cameraDir;
    normalize(d);
    return (minDist * cameraDir - 0.005f * m_scalingFactor * d) / m_scalingFactor;
}

CHairStrand CHairStrand::FlipX()
{
    CHairStrand outputStrand;
    for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		Vec3f pi = GetSample(i).GetPos();
        pi[0] = -pi[0];
        CHairSample sample;
        sample.SetPos(pi);
        outputStrand.AddSample(sample);
    }
    return outputStrand;
}


void CHairStrand::Reverse()
{
    std::reverse(m_vecSample.begin(), m_vecSample.end());
}


void CHairStrand::TranslateStrand(const Vec3f& trans)
{
	for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		GetSample(i).TranslateSample(trans);
	}
}

void CHairStrand::TransformStrand(const Vec3f& trans, const Vec4f& rot, const Vec3f& posRef)
{
	for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		CHairSample& sample = GetSample(i);
		sample.RotateSample(rot, posRef);
		sample.TranslateSample(trans);
	}
}

void CHairStrand::ResampleStrand(const int targetNumOfSamples)
{
    int numOfSamples = GetNumOfSamples();
    if (numOfSamples == 1)
    {
        return;
    }
    if ( targetNumOfSamples == numOfSamples || targetNumOfSamples <= 1 )
    {
        return;
    }
    vector<CHairSample> vecSamples;
    for ( int i=0; i<targetNumOfSamples; i++ )
    {
        Scalar fi = i * (numOfSamples - 1) / Scalar(targetNumOfSamples - 1);
        int idx0 = int(fi);
        int idx1 = idx0 + 1;
        idx1 = (idx1 >= numOfSamples) ? (numOfSamples - 1) : idx1;
        Scalar wt1 = fi - idx0;
        Scalar wt0 = 1.f - wt1;
        CHairSample sample0 = GetSample(idx0);
        CHairSample sample1 = GetSample(idx1);
        Vec3f pos0 = sample0.GetPos();
        Vec3f pos1 = sample1.GetPos();
        Vec3f pos = pos0 * wt0 + pos1 * wt1;
        Vec3f normal = sample0.GetNorm()*wt0+sample1.GetNorm()*wt1;
        normalize(normal);
        CHairSample sample;
        sample.SetPos(pos);
        sample.SetNorm(normal);
        vecSamples.push_back(sample);
    }
    m_vecSample = vecSamples;
}

void CHairStrand::ResampleStrand(int startId, const int targetNumOfSamples)
{
    CHairStrand sub = *this;
    sub.SubStrand(startId, GetNumOfSamples() - 1);
    sub.ResampleStrand(targetNumOfSamples);
    SubStrand(0, startId - 1);
    for (int i = 0; i < sub.GetNumOfSamples(); i++)
    {
        m_vecSample.push_back(sub.GetSample(i));
    }
}

float CHairStrand::GetAverageSegmentLength()
{
    if ( GetNumOfSamples() == 0 )
    {
        return 0.f;
    }
    Scalar lengthSum = 0.f;
    for ( int i=1; i<GetNumOfSamples(); i++ )
    {
        Scalar lengthTmp = dist(GetSample(i).GetPos(), GetSample(i-1).GetPos());
        lengthSum += lengthTmp;
    }
    lengthSum *= 1.f / Scalar(GetNumOfSamples() - 1);
    return lengthSum;
}

void CHairStrand::ResampleStrandViaRelaxation(const int targetNumOfSamples, int iter)
{
    //    if ( targetNumOfSamples == GetNumOfSamples() || targetNumOfSamples <= 0 )
    //    {
    //        return;
    //    }
    Vec3f pn = GetSample(GetNumOfSamples()-1).GetPos();
    ResampleStrand(targetNumOfSamples);
    GetSample(targetNumOfSamples-1).SetPos(pn);
    float forceScale = 0.25f;
    const float damping = 0.9f;
    for ( int n=0; n<iter; n++ )
    {
        float averageSegmentLength = GetAverageSegmentLength();
        vector<Vec3f> vecForce(targetNumOfSamples - 2, Vec3f(0.f, 0.f, 0.f));
        for ( int i=1; i<targetNumOfSamples-1; i++ )
        {
            for ( int j=-1; j<=1; j+=2 )
            {
                int idx = i + j;
                Vec3f pr = GetSample(i).GetPos() - GetSample(idx).GetPos();
                float len = normalize(pr);
                float d = averageSegmentLength - len;
                vecForce[i-1] += d * pr;
            }
        }
        for ( int i=1; i<targetNumOfSamples-1; i++ )
        {
            Vec3f pos = GetSample(i).GetPos() + vecForce[i-1] * forceScale;
            GetSample(i).SetPos(pos);
        }
        forceScale *= damping;
    }
}

float CHairStrand::Dist(CHairStrand& strand2)
{
    float d = 0.0f;
    if (GetNumOfSamples() == 1)
    {
        for (int i=0; i<strand2.GetNumOfSamples(); i++)
        {
            d += dist(GetSample(0).GetPos(), strand2.GetSample(i).GetPos());
        }
        d /= strand2.GetNumOfSamples();
    }
    else if (strand2.GetNumOfSamples() == 1)
    {
        for (int i=0; i<GetNumOfSamples(); i++)
        {
            d += dist(GetSample(i).GetPos(), strand2.GetSample(0).GetPos());
        }
        d /= GetNumOfSamples();
    }
    else
    {
        for (int i=0; i < min(GetNumOfSamples(), strand2.GetNumOfSamples()); i++)
        {
            d += dist(GetSample(i).GetPos(), strand2.GetSample(i).GetPos());
        }
        d /= min(GetNumOfSamples(), strand2.GetNumOfSamples());
    }
    return d;
}

float CHairStrand::Dist2(CHairStrand& strand2)
{
    float d = 0.0f;
    if (GetNumOfSamples() == 1)
    {
        for (int i=0; i<strand2.GetNumOfSamples(); i++)
        {
            d += dist2(GetSample(0).GetPos(), strand2.GetSample(i).GetPos());
        }
        d /= strand2.GetNumOfSamples();
    }
    else if (strand2.GetNumOfSamples() == 1)
    {
        for (int i=0; i<GetNumOfSamples(); i++)
        {
            d += dist2(GetSample(i).GetPos(), strand2.GetSample(0).GetPos());
        }
        d /= GetNumOfSamples();
    }
    else
    {
        for (int i=0; i < min(GetNumOfSamples(), strand2.GetNumOfSamples()); i++)
        {
            d += dist2(GetSample(i).GetPos(), strand2.GetSample(i).GetPos());
        }
        d /= min(GetNumOfSamples(), strand2.GetNumOfSamples());
    }
    return d;
}

float CHairStrand::NearestDist(CHairStrand& strand2)
{
	float distMinMax = std::numeric_limits<Scalar>::min();
	float distMinSum = 0.f;
    for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		Scalar distMin = std::numeric_limits<Scalar>::max();
		int idxMin = -1;
		for ( int j=0; j<strand2.GetNumOfSamples(); j++ )
		{
			float distTmp = dist(GetSample(i).GetPos(), strand2.GetSample(j).GetPos());
			if ( distTmp < distMin )
			{
				distMin = distTmp;
				idxMin = j;
			}
		}
		distMinMax = max(distMinMax, distMin);
		distMinSum = distMinSum + distMin;
	}
    return distMinSum / GetNumOfSamples();
}

float CHairStrand::NearestDist2(CHairStrand& strand2)
{
	float distMinMax = std::numeric_limits<Scalar>::min();
	float distMinSum = 0.f;
    for ( int i=0; i<GetNumOfSamples(); i++ )
	{
		Scalar distMin = std::numeric_limits<Scalar>::max();
		int idxMin = -1;
		for ( int j=0; j<strand2.GetNumOfSamples(); j++ )
		{
			float distTmp = dist2(GetSample(i).GetPos(), strand2.GetSample(j).GetPos());
			if ( distTmp < distMin )
			{
				distMin = distTmp;
				idxMin = j;
			}
		}
		distMinMax = max(distMinMax, distMin);
		distMinSum = distMinSum + distMin;
	}
    return distMinSum / GetNumOfSamples();
}

float CHairStrand::Length(int startIndex)
{
    float length = 0.0f;
    for (int i=startIndex;i<m_vecSample.size()-1;i++) {
        Vec3f v0 = m_vecSample[i].GetPos();
        Vec3f v1 = m_vecSample[i+1].GetPos();
        length += dist(v0, v1);
    }
    return length;
}

void CHairStrand::SubStrand(int start, int end)
{
    vector<CHairSample> vecSamples;
    for (int i = max(start, 0); i <= min(end, GetNumOfSamples()-1); i++)
    {
        vecSamples.push_back(GetSample(i));
    }
    m_vecSample = vecSamples;
}

CHairStrand CHairStrand::LowPassFilterStrand(int halfKernelSize)
{
#if 0
    if ( halfKernelSize <= 0 )
    {
        return *this;
    }
    CHairStrand filteredStrand;
    filteredStrand.ResizeStrand(GetNumOfSamples());
    for ( int i=0; i<GetNumOfSamples(); i++ )
    {
        CHairSample& filteredSample = filteredStrand.GetSample(i);
        int i0 = i - halfKernelSize;
        int i1 = i + halfKernelSize;
        Scalar wtSum = 0.f;
        for ( int j=i0; j<=i1; j++ )
        {
            Scalar wt = 1.f;
            Vec3f pj;
            if ( j < 0 )
            {
                int jn = -j;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(0).GetPos() - samplej.GetPos();
            }
            else if ( j >= GetNumOfSamples() )
            {
                int jn = 2 * GetNumOfSamples() - j - 2;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(GetNumOfSamples()-1).GetPos() - samplej.GetPos();
            }
            else
            {
                CHairSample& samplej = GetSample(j);
                pj = samplej.GetPos();
            }
            wtSum += wt;
            filteredSample.SetPos(filteredSample.GetPos() + pj * wt);
        }
        Scalar wtSumInv = 1.f / wtSum;
        filteredSample.SetPos(filteredSample.GetPos() * wtSumInv);
    }
    filteredStrand.GetSample(0) = GetSample(0);
    //filteredStrand.GetSample(GetNumOfSamples()-1) = GetSample(GetNumOfSamples()-1);
    return filteredStrand;
#elif 1
    if ( halfKernelSize <= 0 )
    {
        return *this;
    }
    CHairStrand filteredStrand;
    filteredStrand.ResizeStrand(GetNumOfSamples());
    for ( int i=0; i<GetNumOfSamples(); i++ )
    {
        CHairSample& filteredSample = filteredStrand.GetSample(i);
        int i0 = i - halfKernelSize;
        int i1 = i + halfKernelSize;
        Scalar wtSum = 0.f;
        for ( int j=i0; j<=i1; j++ )
        {
            Scalar wt = 1.f;
            Vec3f pj;
            if ( j < 0 )
            {
                //continue;
                int jn = -j;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(0).GetPos() - samplej.GetPos();
            }
            else if ( j >= GetNumOfSamples() )
            {
                //continue;
                int jn = 2 * GetNumOfSamples() - j - 2;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(GetNumOfSamples()-1).GetPos() - samplej.GetPos();
            }
            else
            {
                CHairSample& samplej = GetSample(j);
                pj = samplej.GetPos();
            }
            wtSum += wt;
            filteredSample.SetPos(filteredSample.GetPos() + pj * wt);
        }
        Scalar wtSumInv = 1.f / wtSum;
        filteredSample.SetPos(filteredSample.GetPos() * wtSumInv);
    }
    filteredStrand.GetSample(0) = GetSample(0);
    //filteredStrand.GetSample(GetNumOfSamples()-1) = GetSample(GetNumOfSamples()-1);
    return filteredStrand;
#endif
}

CHairStrand CHairStrand::LowPassFilterStrand(int halfKernelSize, int startIdx, int endIdx)
{
    if ( halfKernelSize <= 0 )
    {
        return *this;
    }
    CHairStrand filteredStrand;
    filteredStrand.ResizeStrand(GetNumOfSamples());
    for (int i=max(startIdx, 1); i<=min(endIdx, GetNumOfSamples() - 1); i++) {
        CHairSample& filteredSample = filteredStrand.GetSample(i);
        int i0 = i - halfKernelSize;
        int i1 = i + halfKernelSize;
        Scalar wtSum = 0.f;
        for ( int j=i0; j<=i1; j++ )
        {
            Scalar wt = 1.f;
            Vec3f pj;
            if ( j < 0 )
            {
                int jn = -j;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(0).GetPos() - samplej.GetPos();
            }
            else if ( j >= GetNumOfSamples() )
            {
                int jn = 2 * GetNumOfSamples() - j - 2;
                CHairSample& samplej = GetSample(jn);
                pj = 2.f * GetSample(GetNumOfSamples()-1).GetPos() - samplej.GetPos();
            }
            else
            {
                CHairSample& samplej = GetSample(j);
                pj = samplej.GetPos();
            }
            wtSum += wt;
            filteredSample.SetPos(filteredSample.GetPos() + pj * wt);
        }
        Scalar wtSumInv = 1.f / wtSum;
        filteredSample.SetPos(filteredSample.GetPos() * wtSumInv);
    }
    for (int i = 0; i < max(startIdx, 1); i++)
    {
        filteredStrand.GetSample(i).SetPos(GetSample(i).GetPos());
    }
    for (int i = min(endIdx, GetNumOfSamples() - 1); i < GetNumOfSamples(); i++)
    {
        filteredStrand.GetSample(i).SetPos(GetSample(i).GetPos());
    }
    return filteredStrand;
}