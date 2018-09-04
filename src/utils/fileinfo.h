#pragma once
#include <string>
#include "path.h"

namespace path
{
  class Fileinfo
  {
	Fileinfo()
	{}
	Fileinfo(std::string pPath)
	  :path(pPath)
	{}
	std::string path;

	bool isAbsolute() const
	{
	  return path::isAbsolute(path);
	}
	bool isRelative() const
	{
	  return path::isRelative(path);
	}

	std::string extension() const
	{
	  return path::getExtension(path);
	}

	std::string path() const
	{
	  return path::getPath(path);
	}

	std::string name() const
	{
	  return path::getFilename(path);
	}
  };
}
