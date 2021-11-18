#pragma once
#include "mmu_utils.h"

BEGIN_MMU_NS
class IImageIO : public StaticImplFactory<IImageIO>, public MMUObject {
public:
	virtual PImage read(cstring fname) = 0;
	virtual void write(cstring fname, PImage img) = 0;
};
typedef std::shared_ptr<IImageIO> PImageIO;
END_MMU_NS
