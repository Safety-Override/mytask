#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    nlohmann::json& aJSONMessage)
{
    aJSONMessage[UserId] = aId;
    std::string request = aJSONMessage.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}


nlohmann::json RegistrationRequest(std::string& name)
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Registration;
    req[Message] = name;
    return req;
}

nlohmann::json HelloRequest()
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Hello;
    return req;
}

nlohmann::json ActiveRequestsRequest()
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::ActiveRequests;
    return req;
}

nlohmann::json DealsRequest()
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Deals;
    return req;
}

nlohmann::json BalanceRequest()
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Balance;
    return req;
}

nlohmann::json SellRequest(long long price_, long long volume_)
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Sell;
    req[Price] = price_;
    req[Volume] = volume_;
    return req;
}

nlohmann::json BuyRequest(long long price_, long long volume_)
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Buy;
    req[Price] = price_;
    req[Volume] = volume_;
    return req;
}

////////////////////////////////////////////////////

std::string ErrorResponse(nlohmann::json& j)
{
    return j[Message];
}

std::string RegistrationResponse(nlohmann::json& j)
{
    return j[Message];
}

std::string HelloResponse(nlohmann::json& j)
{
    return j[Message];
}

std::string ActiveRequestsResponse(nlohmann::json& j)
{
    std::string reply = "Your Sell Requests :\n";
    for (auto elem : j[SellRequests]) {
        reply += "Price: " + std::to_string(static_cast<int>(elem[0])) 
        + " Volume: " + std::to_string(static_cast<int>(elem[1])) 
        + "\n";
    }
    reply += "Your Buy Requests :\n";
    for (auto elem : j[BuyRequests])  {
        reply += "Price: " + std::to_string(static_cast<int>(elem[0])) 
        + " Volume: " + std::to_string(static_cast<int>(elem[1])) 
        + "\n";
    }
    return reply;

}

std::string DealsResponse(nlohmann::json& j)
{
    std::string reply = "Your Deals :\n";
    for (auto elem : j[SellRequests][DealsList]) {
        reply += "You sold to " + static_cast<ns::Deal>(elem).userName + ". Price: " + std::to_string(static_cast<ns::Deal>(elem).price) + " Volume: " + std::to_string(static_cast<ns::Deal>(elem).volume) + "\n";

    }
    for (auto elem :  j[BuyRequests][DealsList]) {
        reply += "You bought from " + static_cast<ns::Deal>(elem).userName
               + ". Price: " + std::to_string(static_cast<ns::Deal>(elem).price)
               + " Volume: " + std::to_string(static_cast<ns::Deal>(elem).volume) + "\n";

    }
    return reply;
}

std::string BalanceResponse(nlohmann::json& j)
{
    std::string reply = "Your Balance : Dollars : " + std::to_string(static_cast<long long>(j[Dollars])) + "\n"
                      + "Rubles : " + std::to_string(static_cast<long long>(j[Rubles])) + "\n";
    
    return reply;
}

std::string SellResponse(nlohmann::json& j)
{
    return j[Message];
}

std::string BuyResponse(nlohmann::json& j)
{
    return j[Message];
}
