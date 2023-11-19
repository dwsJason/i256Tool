//
// I256 - I256 Parse/Modify Tool
// 

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "bctypes.h"
#include "txtfile.h"
#include "i256file.h"
#include "stm_file.h"

//------------------------------------------------------------------------------
static void helpText()
{
	printf("i256Dump - v0.1\n");
	printf("--------------\n");
	printf("I256Dump, dump the contents of an i256 into raw files\n");
	printf("\n");
	printf("\ni256dump <file.256>\n\n");
	// https://docs.google.com/document/d/10ovgMClDAJVgbW0sOhUsBkVABKWhOPM5Au7vbHJymoA/edit?usp=sharing

	exit(-1);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local helper functions

static bool contains(char x, const char* pSeparators)
{
	while (*pSeparators)
	{
		if (x == *pSeparators)
		{
			return true;
		}
		pSeparators++;
	}

	return false;
}

static std::vector<std::string> split(const std::string& s, const char* separators)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;

	for (int index = 0; index < s.length(); ++index)
	{
		pos = index;

		// if the index is a separator
		if (contains(s[index], separators))
		{
			// if we've skipped a token, collect it
			if (prev_pos != index)
			{
				output.push_back(s.substr(prev_pos, index-prev_pos));

				// skip white space here
				while (index < s.length())
				{
					if (contains(s[index], separators))
					{
						++index;
					}
					else
					{
						prev_pos = index;
						pos = index;
						break;
					}
				}
			}
			else
			{
				prev_pos++;
			}
		}
	}

    output.push_back(s.substr(prev_pos, pos-prev_pos+1)); // Last word

    return output;
}

static std::string toLower(const std::string s)
{
	std::string result = s;

	for (int idx = 0; idx < result.size(); ++idx)
	{
		result[ idx ] = (char)tolower(result[idx]);
	}

	return result;
}

// Case Insensitive
static bool endsWith(const std::string& S, const std::string& SUFFIX)
{
	bool bResult = false;

	std::string s = toLower(S);
	std::string suffix = toLower(SUFFIX);

    bResult = s.rfind(suffix) == (s.size()-suffix.size());

	return bResult;
}
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	char* pFileName = nullptr;

	if (argc < 2) helpText();

	for (int idx = 1; idx < argc; ++idx )
	{
		char* arg = argv[ idx ];

		if ('-' == arg[0])
		{
			// Parse as an option
		}
		else if (nullptr == pFileName)
		{
			// Assume the first non-option is working directory
			pFileName = argv[ idx ];
		}
		else
		{
			// Oh Crap, we have a non-option, but we don't know what to do with
			// it
			printf("ERROR: Invalid option, Arg %d = %s\n\n", idx, argv[ idx ]);
			helpText();
		}
	}

	if (pFileName)
	{

		printf("Image File = %s\n\n", pFileName);

		if (endsWith(pFileName, ".256"))
		{
		   	I256File sourcefile(pFileName);
			printf("pixel frames = %d\n", sourcefile.GetFrameCount());
			printf("pixel width  = %d\n", sourcefile.GetWidth());
			printf("pixel height = %d\n", sourcefile.GetHeight());
			printf("\n");

			const std::vector<unsigned char*>& pPixelMaps = sourcefile.GetPixelMaps();

			for (int frame_no = 0; frame_no < (int)pPixelMaps.size(); ++frame_no )
			{
				std::string pixlName = pFileName;

				pixlName = pixlName + ".PIXL." + std::to_string(frame_no);

				printf("Dump %s\n", pixlName.c_str());

				FILE* pFile = nullptr;
				errno_t err = fopen_s(&pFile, pixlName.c_str(), "wb");

				if (0==err)
				{
					fwrite(pPixelMaps[frame_no], sizeof(unsigned char),
						   sourcefile.GetWidth()*sourcefile.GetHeight(),
						   pFile);

					fclose(pFile);
				}
			}

			printf("\n");

			const std::vector<unsigned short*>& pTileMaps = sourcefile.GetTileMaps();

			if (pTileMaps.size())
			{
				printf("tilemap frames = %d\n", (int)pTileMaps.size());
				printf("tilemap width  = %d\n", sourcefile.GetWidthTiles());
				printf("tilemap height = %d\n", sourcefile.GetHeightTiles());

				for (int frame_no = 0; frame_no < (int) pTileMaps.size(); ++frame_no)
				{
					std::string tmapName = pFileName;

					tmapName = tmapName + ".TMAP." + std::to_string(frame_no);

					printf("\nDump %s\n",tmapName.c_str());

					FILE* pFile = nullptr;
					errno_t err = fopen_s(&pFile, tmapName.c_str(), "wb");

					if (0==err)
					{
						fwrite(pTileMaps[frame_no], sizeof(unsigned short),
							   sourcefile.GetWidthTiles()*sourcefile.GetHeightTiles(),
							   pFile);

						fclose(pFile);
					}

				}
			}

			printf("\n");

			const I256_Palette& palette = sourcefile.GetPalette();

			printf("CLUT Number of Colors = %d\n", palette.iNumColors);
			printf("\n");
		
			std::string clutName = pFileName;

			clutName = clutName + ".CLUT";

			printf("Dump %s\n", clutName.c_str());

			FILE* pFile = nullptr;
			errno_t err = fopen_s(&pFile, clutName.c_str(), "wb");

			if (0==err)
			{
				fwrite(palette.pColors, sizeof(I256_Color),
					   palette.iNumColors,
					   pFile);

				fclose(pFile);
			}

			printf("\nDump Complete.\n");

		}
		else
		{
			printf("\nFile must have .256 file extension to be processed\n\n");
			helpText();
		}

		
	}
	else
	{
		helpText();
	}


	return 0;
}

