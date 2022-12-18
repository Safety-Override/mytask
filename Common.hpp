#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>
#include "json.hpp"

static short port = 5555;

namespace ns {
    struct Deal {
        long long price;
        long long volume;
        std::string userName;
    };

    void to_json(nlohmann::json& j, const Deal& d) {
        j = nlohmann::json{ {"Price", d.price}, {"Volume", d.volume}, {"UserName", d.userName} };
    };

    void from_json(const nlohmann::json& j, Deal& d) {
        j.at("Price").get_to(d.price);
        j.at("Volume").get_to(d.volume);
        j.at("UserName").get_to(d.userName);
    };
}

constexpr char Price[] = "Price";
constexpr char Volume[] = "Volume";

constexpr char Rubles[] = "Rubles";
constexpr char Dollars[] = "Dollars";

constexpr char Login[] = "Login";
constexpr char Password[] = "Password";
constexpr char UserName[] = "UserName";

constexpr char UserId[] = "UserId";
constexpr char DealsList[] = "DealsList";
constexpr char ReqType[] = "ReqType";
constexpr char Message[] = "Message";
constexpr char MsgType[] = "MsgType";
constexpr char SellRequests[] = "SellRequests";
constexpr char BuyRequests[] = "BuyRequests";

enum class MessageTypes
{
    Error, 
    Registration, 
    Authorization,
    Hello, 
    ActiveRequests, 
    Deals, 
    Balance, 
    Sell, 
    Buy,
    Quotations,
    Delete
};

#endif //CLIENSERVERECN_COMMON_HPP
