#include "OptiX_Texture.h"

FOptixTexture::FOptixTexture()
{
}

FOptixTexture::FOptixTexture(FOptixTexture&& rhs)
{
}

FOptixTexture& FOptixTexture::operator=(FOptixTexture&& rhs)
{
	this->~FOptixTexture();
	new (this) FOptixTexture(std::move(rhs));
	return *this;
}

FOptixTexture::~FOptixTexture()
{
}

void FOptixTexture::Destroy()
{
}
