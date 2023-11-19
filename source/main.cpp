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
	printf("i256Tool - v0.1\n");
	printf("--------------\n");
	printf("I256Tool for manipulating I256 files\n");
	printf("\n");
	printf("\ni256tool [options] <filelist.txt>\n\n");
	printf("options:\n\n");
	// https://docs.google.com/document/d/10ovgMClDAJVgbW0sOhUsBkVABKWhOPM5Au7vbHJymoA/edit?usp=sharing
	printf("Adds TMAP section into existing I256 catalog\n");

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

bool Debug = true;

int main(int argc, char* argv[])
{
	//while (Debug)
	{
		// wait for debugger
	}
	char* pWorkDirectory = nullptr;
	char* pBaseFileName = nullptr;

	if (argc < 2) helpText();

	for (int idx = 1; idx < argc; ++idx )
	{
		char* arg = argv[ idx ];

		if ('-' == arg[0])
		{
			// Parse as an option
		}
		else if (nullptr == pWorkDirectory)
		{
			// Assume the first non-option is working directory
			pWorkDirectory = argv[ idx ];
		}
		else if (nullptr == pBaseFileName)
		{
			// Assume second non-option is base filename
			pBaseFileName = argv[ idx ];
		}
		else
		{
			// Oh Crap, we have a non-option, but we don't know what to do with
			// it
			printf("ERROR: Invalid option, Arg %d = %s\n\n", idx, argv[ idx ]);
			helpText();
		}
	}

	if (pWorkDirectory && pBaseFileName)
	{

		std::string pathFileList = pWorkDirectory;
		pathFileList = pathFileList + "\\" + pBaseFileName + ".filelist.txt";

		// It's a txt file, hopefully one from Promotion, with a file list
		TXTFile text_file(pathFileList.c_str());

		const std::vector<std::string>& lines = text_file.GetLines();

		if (lines.size()==2)
		{
			const char* pImageFile = lines[0].c_str();
			const char* pSTMFile = lines[1].c_str();

			printf("Image File = %s\n", pImageFile);
			printf("STM File = %s\n", pSTMFile);

			if (endsWith(pImageFile, ".256") && endsWith(pSTMFile, ".stm"))
			{
				std::string IFileName = pWorkDirectory;
				IFileName = IFileName + "\\" + pImageFile;

				I256File catalog(IFileName.c_str());
				STMFile  mapfile(pSTMFile);

				int width  = mapfile.GetWidth();
				int height = mapfile.GetHeight();

				int numEntry = width * height;

				std::vector<u16> newMap;

				newMap.resize(numEntry);

				const std::vector<unsigned int>& sourceMap = mapfile.GetMap();

				// convert from 32 bit to 16 bit, so we don't have too at runtime
				for (int idx = 0; idx < numEntry; ++idx)
				{
					newMap[idx] = (u16) sourceMap[idx];
				}

				std::vector<unsigned short*> maps;
				maps.push_back(&newMap[0]);

				catalog.AddTileMaps(maps, width, height);

				catalog.SaveToFile(IFileName.c_str()); // save the 256 file back out, with a TMAP section
			}
		}
		
	}
	else
	{
		helpText();
	}


	return 0;
}

