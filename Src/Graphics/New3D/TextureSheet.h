#ifndef _TEXTURE_SHEET_H_
#define _TEXTURE_SHEET_H_

#include "Types.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include "Texture.h"

namespace New3D {

#define REAL_SHEET_WIDTH	2048
#define REAL_SHEET_HEIGHT	2048

class TextureSheet
{
public:
	TextureSheet();

	std::shared_ptr<Texture>	BindTexture		(const UINT16* src, int format, bool mirrorU, bool mirrorV, int x, int y, int width, int height);
	void						Invalidate		(int x, int y, int width, int height); // release parts of the memory
	void						Release			();		// release all texture objects and memory
	int							GetTexFormat	(int originalFormat, bool contour);

private:

	int ToIndex(int x, int y);

	std::unordered_multimap<int, std::shared_ptr<Texture>> m_texMap[12];

	// the key for the above maps is the x/y position in the 2048x2048 texture
	// array of 8 planes for each texture type

	std::vector<UINT8> m_temp;
};

} // New3D

#endif