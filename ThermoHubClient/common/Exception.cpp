#include "Exception.h"
#include <algorithm>

namespace ThermoHubClient {
namespace internal {
/// Creates the location string from the file name and line number.
/// The result of this function on the file `/home/user/gitThermoFun/ThermoFun/src/Substance.cpp`
/// will be `ThermoFun/src/Substance.cpp`.
/// @param file The full path to the file
/// @param line The line number
std::string location(const std::string& file, int line)
{
    //    std::string str = "ThermoFun/";
    //    auto pos = std::find_end(file.begin(), file.end(), str.begin(), str.end()) - file.begin();
    std::stringstream ss;
    if (file.size() > 45)
        ss << "..."<< file.substr(file.size() - 45, 45) << ":" << line;
    else
        ss << file << ":" << line;
    return ss.str();
}

std::string message(const Exception& exception, const std::string& /*file*/, int /*line*/)
{
    std::string error = exception.error.str();
    std::string reason = exception.reason.str();
    std::string loc = location(exception.file, exception.line);
    unsigned length = std::max(error.size(), std::max(reason.size(), loc.size())) + 16;
    std::string bar(length, '*');
    std::stringstream message;
    message << std::endl;
    message << bar << std::endl;
    message << "*** Error: " << error << std::endl;
    message << "*** Reason: " << reason << std::endl;
    message << "*** Location: " << loc << std::endl;
    message << bar << std::endl;
    message << std::endl;
    return message.str();
}
}

auto hubError(const std::string& error, const std::string& reason, const int& line, const std::string& file) -> void
{
    Exception exception;
    exception.error << error;
    exception.reason << reason;
    exception.line = line;
    exception.file = file;
    RaiseError(exception);
}

auto hubErrorIf(bool condition, const std::string& error, const std::string& reason, const int& line, const std::string& file) -> void
{
    if (condition)
        hubError(error, reason, line, file);
}

} // namespace ThermoHubClient
