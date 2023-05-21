//
// STM - Simple Tile Map Parser
//

#include "stm_file.h"

//------------------------------------------------------------------------------
// Load in a STMFile constructor
//
STMFile::STMFile(const char *pFilePath)
	: m_widthTiles(0)
	, m_heightTiles(0)
{
	LoadFromFile(pFilePath);
}

//------------------------------------------------------------------------------

STMFile::~STMFile()
{
}

//------------------------------------------------------------------------------

void STMFile::LoadFromFile(const char* pFilePath)
{
	// Clear Previous File
	m_heightTiles = 0;
	m_widthTiles = 0;
	m_TileMap.clear();

	//--------------------------------------------------------------------------

	std::vector<unsigned char> bytes;

	//--------------------------------------------------------------------------
	// Read the file into memory
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, pFilePath, "rb");

	if (0==err)
	{
		fseek(pFile, 0, SEEK_END);
		size_t length = ftell(pFile);	// get file size
		fseek(pFile, 0, SEEK_SET);

		bytes.resize( length );			// make sure buffer is large enough

		// Read in the file
		fread(&bytes[0], sizeof(unsigned char), bytes.size(), pFile);
		fclose(pFile);
	}

	if (bytes.size())
	{
		size_t file_offset = 0;	// File Cursor

		// Bytes are in the buffer, so let's start looking at what we have
		STM_Header* pHeader = (STM_Header*) &bytes[0];

		// Early out if things don't look right
		if (!pHeader->IsValid((unsigned int)bytes.size()))
		{
			printf("STM FILE %s INVALID\n", pFilePath);
			return;
		}


		m_widthTiles = pHeader->width;
		m_heightTiles = pHeader->height;

		m_TileMap.resize(m_widthTiles * m_heightTiles);

		memcpy(&m_TileMap[0], &bytes[ sizeof(STM_Header)], m_widthTiles*m_heightTiles*sizeof(unsigned int));
	}
}

//------------------------------------------------------------------------------

