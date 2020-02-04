#include "../cfg.hpp"
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

#include "../../config.h"


namespace vkmod::cfg
{
	void init()
	{
		struct stat info;
		if ( stat( CONFIG_DIR ), &info )
		{
			std::cout << "CONFIG_DIR exists" << std::endl;
			return;
		}
		if (mkdir(CONFIG_DIR))
		{
			std::cout << "Failed to Create CONFIG_DIR" << std::endl;
			exit(1);
		}
		
	}
}
