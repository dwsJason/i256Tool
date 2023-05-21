//
// TXTFile Class
//

#ifndef TXTFILE_H_
#define TXTFILE_H_

#include <string>
#include <vector>

#include "bctypes.h"
#include "memstream.h"

//------------------------------------------------------------------------------

class TXTFile
{
public:
	TXTFile( std::string filepath );
	~TXTFile();

	const std::vector<std::string> GetLines()
	{
		return m_lines;
	}


private:
	std::string m_filepath;

	std::vector<std::string> m_lines;
};

#endif // ORGFILE_H_


