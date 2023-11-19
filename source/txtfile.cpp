//
// txtfile.cpp
//
#include "txtfile.h"
#include <stdio.h>

//------------------------------------------------------------------------------
// Static Helpers

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

// make this string lowercase
static void tolower(std::string& s)
{
	for (int index = 0; index < s.length(); ++index)
	{
		s[index] = tolower( s[index] );
	}
}

//------------------------------------------------------------------------------
TXTFile::TXTFile( std::string filepath )
	: m_filepath( filepath )
{
	FILE* pFile = nullptr;
	errno_t err = fopen_s(&pFile, filepath.c_str(), "rb");

	if (0==err)
	{
		fseek(pFile, 0, SEEK_END);
		size_t length = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		printf("\nTXTFile %s - %lld bytes\n", filepath.c_str(), length);

		printf("Reading %s\n", filepath.c_str());

		u8* pData = new u8[ length ];

		fread(pData, sizeof(u8), length / sizeof(u8), pFile);

		// Now we have a buffer, with the whole OMF File
		// Read in the Sections
		{
			MemoryStream memStream(pData, length);

			while (memStream.NumBytesAvailable())
			{
				std::string lineData = memStream.ReadLine();
				m_lines.push_back(lineData);
			}
		}

		delete[] pData;

		fclose(pFile);

		printf("\nRead %lld Lines Completed\n", m_lines.size());
	}
}

//------------------------------------------------------------------------------

TXTFile::~TXTFile()
{
}

//------------------------------------------------------------------------------
