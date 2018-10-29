#pragma once

namespace utils
{
  struct Range
  {
	Range()
	{}
	Range(size_t beg, size_t len)
	  : begin(beg)
	  , length(len)
	{}
	size_t begin;
	size_t length;
  };

}
