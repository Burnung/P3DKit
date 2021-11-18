#pragma once
#include "common/P3D_Utils.h"
#include "common/mathIncludes.h"
BEGIN_P3D_NS
class P3DBone :public P3DObject{
public:
	P3DBone() = default;
	~P3DBone() {};
public:
	void update();
	void print(bool b) {
		std::cout << name << ":" << std::endl;
		if (b)
			l2gMat.getTranspose().print();
		else
			l2gMat.getTranspose().print();
		for (auto t : childrens)
			t->print(b);
	}

public:
	int boneId;

	Mat4f l2gMat;
	Mat4f l2pNow;
	//Mat4f g2lMat;
	Mat4f l2pBase;
	//³õÊ¼×´Ì¬µÄ pos3D sx,sy,sz,rx,ry,rz,tx,ty,tz
	std::vector<float32> baseEulers;

	std::string name;
	std::weak_ptr<P3DBone> parent;
	std::vector<std::shared_ptr<P3DBone>> childrens;

};


typedef std::shared_ptr<P3DBone> pBone;


END_P3D_NS
