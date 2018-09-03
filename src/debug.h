#pragma once
#include <string>
#include <vector>
#include <array>

namespace debug {
  struct Error {
	enum Severity {
	  Warning,
	  Trivial,
	  Fatal
	};
	const static std::array<const std::string, 3> severityStrings;
	Error() :msg(""), severity(Warning) {}
	Error(std::string& pMsg, Severity& pSev)
	  :msg(pMsg), severity(pSev) {}
	Severity severity;
	std::string msg = "";
  };

  void pushError(std::string pMessage, Error::Severity errorSeverity = Error::Severity::Warning);
  void printErrors();
  void togglePrintInfo();
  void printInfo();
  extern bool shouldPrintInfo;
  extern std::vector<Error> errorBuffer;
}
