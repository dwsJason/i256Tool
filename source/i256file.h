//
// C++ Encoder/Decoder
// For C256/F256 Foenix Picture File Format
//
//  $$TODO, bring down a text file copy of the spec into the project
// https://docs.google.com/document/d/10ovgMClDAJVgbW0sOhUsBkVABKWhOPM5Au7vbHJymoA/edit?usp=sharing
//
#ifndef I256_FILE_H
#define I256_FILE_H

#include <vector>

#pragma pack(push, 1)

typedef struct I256_Color
{
    unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
} I256_Color;

// Header Chunk (whole file Chunk)
typedef struct I256File_Header
{
	char 			hi,h2,h5,h6;  // 'I','2','5','6'

	unsigned int 	file_length;  // In bytes, including the 16 byte header

	short 			version;  // 0x0000 for now
	short			width;	  // In pixels
	short			height;	  // In pixels

	short			reserved; // Reserved for future expansion, set to 0

//------------------------------------------------------------------------------
// If you're doing C, just get rid of these methods
	bool IsValid(unsigned int fileLength)
	{
		if (file_length != fileLength)
			return false;				// size isn't right

		if (version != 0)
			return false;				// version is not right

		if ((hi!='I')||(h2!='2')||(h5!='5')||(h6!='6'))
			return false;				// signature is not right

		if ((0==width)||(0==height))
			return false;				// invalid dimensions

		if (reserved != 0)				// reserved field is reserved
			return false;

		return true;
	}

} I256File_Header;

// PIXeL chunk
typedef struct I256File_PIXL
{
	char		  p,i,x,l;		// 'P','I','X','L'
	unsigned int  chunk_length; // in bytes, including the 10 bytes header of this chunk
	unsigned short num_blobs;	// number of blobs to decompress

	// Commands Coded Data Follows
//------------------------------------------------------------------------------
// If you're doing C, just get rid of these methods
	bool IsValid()
	{
		if ((p!='P')||(i!='I')||(x!='X')||(l!='L'))
			return false;				// signature is not right

		return true;
	}


} I256File_PIXL;


// TileMAP chunk
typedef struct I256File_TMAP
{
	char		  t,m,a,p;		// 'T','M','A','P'
	unsigned int  chunk_length; // in bytes, including the 14 bytes header of this chunk
	unsigned short width;
	unsigned short height;
	unsigned short num_blobs;	// number of blobs to decompress

	// Commands Coded Data Follows
//------------------------------------------------------------------------------
// If you're doing C, just get rid of these methods
	bool IsValid()
	{
		if ((t!='T')||(m!='M')||(a!='A')||(p!='P'))
			return false;				// signature is not right

		if ((0==width)||(0==height))
			return false;				// invalid dimensions

		return true;
	}


} I256File_TMAP;



// Color LookUp Table, Chunk
typedef struct I256File_CLUT
{
	char		  c,l,u,t;		// 'C','L','U','T'
	unsigned int  chunk_length; // in bytes, including the 8 bytes header of this chunk
	unsigned short num_colors;  // number of colors-1, 1-16384 colors

	// BGRA quads follow, either raw or lzsa2 compressed

//------------------------------------------------------------------------------
// If you're doing C, just get rid of these methods
	bool IsValid()
	{
		int numColors = num_colors & 0x3FFF;
		numColors++;

		if (chunk_length > (sizeof(I256File_CLUT)+(numColors*4)))
			return false;				// size isn't right

		if ((c!='C')||(l!='L')||(u!='U')||(t!='T'))
			return false;				// signature is not right

		return true;
	}


} I256File_CLUT;



// Generic Unknown Chunk
typedef struct I256File_CHUNK
{
	char id0,id1,id2,id3;
	unsigned int chunk_length;

} I256File_CHUNK;

typedef struct I256_Palette
{
    int iNumColors;
    I256_Color* pColors;

} I256_Palette;

class I256File
{
public:
	// Create a Blank Fan File
	I256File(int iWidthPixels, int iHeightPixels, int iNumColors);
	// Load in a I256 Image File
	I256File(const char* pFilePath);

	~I256File();

	// Creation
	void SetPalette( const I256_Palette& palette );
	void AddImages( const std::vector<unsigned char*>& pPixelMaps );
	void AddTileMaps( const std::vector<unsigned short*>& pTileMaps, unsigned short width, unsigned short height );
	void SaveToFile(const char* pFilenamePath);

	// Retrieval
	void LoadFromFile(const char* pFilePath);
	int GetFrameCount() { return (int)m_pPixelMaps.size(); }
	int GetWidth()  { return m_widthPixels; }
	int GetHeight() { return m_heightPixels; }

	const I256_Palette& GetPalette() { return m_pal; }
	const std::vector<unsigned char*> GetPixelMaps() { return m_pPixelMaps; }

private:

	void UnpackClut(I256File_CLUT* pCLUT);
	void UnpackPixel(I256File_PIXL* pPIXL);
	void UnpackTileMap(I256File_TMAP* pTMAP);

//	int EncodeFrame(unsigned char* pCanvas, unsigned char* pFrame, unsigned char* pWorkBuffer, size_t bufferSize );

	int m_widthPixels;		// Width of image in pixels
	int m_heightPixels;		// Height of image in pixels
	int m_numColors;		// number of colors in the initial CLUT

	I256_Palette m_pal;

	std::vector<unsigned char*> m_pPixelMaps;

	int m_widthTiles;
	int m_heightTiles;

	std::vector<unsigned short*> m_pTileMaps;

};

#pragma pack(pop)


#endif // I256_FILE_H

