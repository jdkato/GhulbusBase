#include <gbBase/Assert.hpp>

#ifndef GHULBUS_CONFIG_BASE_BARE_BUILD
#include <gbBase/Exception.hpp>

#include <boost/exception/all.hpp>
#endif

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <thread>

namespace GHULBUS_BASE_NAMESPACE
{
namespace
{
Assert::Handler g_AssertionHandler(&Assert::failAbort);

std::ostream& operator<<(std::ostream& os, Assert::HandlerParameters const& handler_param)
{
    os << handler_param.file << "(" << handler_param.line << "): Assertion failed "
       << " in function " << handler_param.function << ": " << handler_param.condition;
    if (handler_param.message) {
        os << " - " << handler_param.message;
    }
    return os;
}

std::string toString(Assert::HandlerParameters const& handler_param)
{
    std::stringstream sstr;
    sstr << handler_param;
    return sstr.str();
}
}

namespace Assert
{
void setAssertionHandler(Handler handler) noexcept
{
    auto& static_handler = g_AssertionHandler;
    static_handler = handler;
}

Handler getAssertionHandler() noexcept
{
    auto const& static_handler = g_AssertionHandler;
    return static_handler;
}

void assertionFailed(HandlerParameters const& param)
{
    getAssertionHandler()(param);
    std::abort();
}

void failAbort(HandlerParameters const& param)
{
    // keep the error message in a string for easy inspection by the debugger
    std::string const str = toString(param);
    std::cerr << str << std::endl;
    std::abort();
}

void failHalt(HandlerParameters const& param)
{
    // keep the error message in a string for easy inspection by the debugger
    std::string const str = toString(param);
    std::cerr << str << std::endl;
    for(;;) { std::this_thread::sleep_for(std::chrono::hours(1)); }
}

#ifndef GHULBUS_CONFIG_BASE_BARE_BUILD
void failThrow(HandlerParameters const& param)
{
    boost::throw_exception(boost::enable_error_info(Exceptions::AssertFailed()) <<
                           boost::throw_file(param.file) <<
                           boost::throw_line(param.line) <<
                           boost::throw_function(param.function) <<
                           Exception_Info::description(std::string(param.condition) +
                               (param.message ? " - " : "") + (param.message ? param.message : "")));
}
#endif
}
}
