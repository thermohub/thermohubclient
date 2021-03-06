// C++ includes
#include <stdexcept>
#include <string>
#include <sstream>

namespace ThermoHubClient {


/// Provides a convenient way to initialized an exception with helpful error messages.
struct Exception
{
    /// The error message to be displayed when the exception is raised
    std::stringstream error;

    /// The reason message to be displayed when the exception is raised
    std::stringstream reason;

    /// The line in the cpp file
    int line;

    std::string file;
};

namespace internal {

/// Create a message error from an Exception instance
/// @param exception The exception instance to be raised
/// @param file The name of the file where the exception was raised
/// @param line The number of the line where the exception was raised
/// @return The message error of the exception
/// @see Exception
std::string message(const Exception& exception, const std::string& file, int line);

} // namespace internal

/// Define a macro to raise a runtime exception from an Exception instance.
/// @see Exception
/// @ingroup Common
#define RaiseError(exception) \
    throw std::runtime_error(ThermoHubClient::internal::message(exception, __FILE__, __LINE__));

/// Define a macro to raise a runtime exception from a error string and a reason string.
/// @see Exception
/// @ingroup Common
#define RuntimeError(errorstr, reasonstr) \
    { \
        Reaktoro::Exception exception; \
        exception.error << errorstr; \
        exception.reason << reasonstr; \
        RaiseError(exception); \
    }

/// Define a macro to raise a runtime exception from a error string and a reason string.
/// @see Exception
/// @ingroup Common
#define Assert(condition, errorstr, reasonstr) \
    { \
        if(!(condition)) { \
            Reaktoro::Exception exception; \
            exception.error << errorstr; \
            exception.reason << reasonstr; \
            RaiseError(exception); \
        } \
    }

auto hubError(const std::string& error, const std::string& reason, const int& line, const std::string& file) -> void;

auto hubErrorIf(bool condition, const std::string& error, const std::string& reason, const int& line, const std::string& file) -> void;

} // namespace ThermoHubClient

