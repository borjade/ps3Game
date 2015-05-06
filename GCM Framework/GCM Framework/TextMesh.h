#pragma once
#include "Mesh.h"

struct Font {
	CellGcmTexture texture;
	int		xCount;
	int		yCount;

	Font(CellGcmTexture tex, unsigned int xCount, unsigned int yCount) {
		this->texture = tex;
		this->xCount = xCount;
		this->yCount = yCount;
	}
	~Font() {
		
	}
};

class TextMesh : public Mesh	{
public:
	TextMesh(const std::string &text,  Font &font);
	~TextMesh(void);

	void BufferTextData();

protected:

	 Font& f;
};

