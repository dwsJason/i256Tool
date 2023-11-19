//
// STM - Simple Tile Map Parser
// 
#ifndef STM_FILE_H
#define STM_FILE_H
#include <vector>

#pragma pack(push, 1)

typedef struct STM_Header
{
	unsigned char s,t,m,p;
	unsigned short width;
	unsigned short height;

	bool IsValid(unsigned int TotalSize)
	{
		if (('S'==s)&&('T'==t)&&('M'==m)&&('P'==p))
		{
			unsigned int calculatedSize = width * height;

			calculatedSize *= 4;  // each map name is 32 bit

			calculatedSize += sizeof(STM_Header); // should be 12

			return calculatedSize == TotalSize; // true if size looks right

		}
		return false;
	}

} STM_Header;

class STMFile
{
public:
	STMFile(const char* pFilePath);
	~STMFile();

	void LoadFromFile(const char* pFilePath);

	int GetWidth()  { return m_widthTiles; }
	int GetHeight() { return m_heightTiles; }
	const std::vector<unsigned int>& GetMap() { return m_TileMap; }

private:

	int m_widthTiles;
	int m_heightTiles;

	std::vector<unsigned int> m_TileMap;

};

#pragma pack(pop)

#endif // STM_FILE_H


