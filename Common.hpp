#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests
{
    static std::string Registration = "Reg";
    static std::string Hello = "Hel";
    static std::string ActiveRequests = "Act";
    static std::string Deals = "Dea";
    static std::string Balance = "Bal";
    static std::string Sell = "Sel";
    static std::string Buy = "Buy";
}

#endif //CLIENSERVERECN_COMMON_HPP
