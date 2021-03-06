#pragma once
#include <string>
#include <vector>
#include <array>

namespace debug
{
  struct Error
  {
	enum Severity {
	  Warning,
	  Trivial,
	  Fatal
	};
	const static std::array<const std::string, 3> severityStrings;
	Error()
	  : msg("")
	  , severity(Warning)
	{}
	Error(std::string& pMsg, Severity& pSev)
	  : msg(pMsg)
	  , severity(pSev)
	{}
	Severity severity;
	std::string msg = "";
  };

  void setError(std::string pMessage, Error::Severity errorSeverity = Error::Severity::Warning);
  void error(std::string pMessage);
  void fatal(std::string pMessage);
  void warning(std::string pMessage);
  void printErrors();
  extern std::vector<Error> errorBuffer;
}
