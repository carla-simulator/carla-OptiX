#include "OptiX_Texture.h"

FCarlaOptiXTexture::FCarlaOptiXTexture()
{
}

FCarlaOptiXTexture::FCarlaOptiXTexture(FCarlaOptiXTexture&& rhs)
{
}

FCarlaOptiXTexture& FCarlaOptiXTexture::operator=(FCarlaOptiXTexture&& rhs)
{
	this->~FCarlaOptiXTexture();
	new (this) FCarlaOptiXTexture(std::move(rhs));
	return *this;
}

FCarlaOptiXTexture::~FCarlaOptiXTexture()
{
}

void FCarlaOptiXTexture::Destroy()
{
}
