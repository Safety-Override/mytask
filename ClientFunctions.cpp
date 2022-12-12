#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aMessageType,
    const std::string& aMessage = "")
{
    nlohmann::json req;
    req[UserId] = aId;
    req[MsgType] = aMessageType;
    req[Message] = aMessage;
    std::string request = req.dump();
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


std::string RegistrationRequest(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;
    SendMessage(aSocket, "0", MessageTypes::Registration, name);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    return j[Message];
}

std::string HelloRequest(tcp::socket& aSocket, std::string id)
{
    SendMessage(aSocket, id, MessageTypes::Hello);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    return j[Message];
}

std::string ActiveRequestsRequest(tcp::socket& aSocket, std::string id)
{
    SendMessage(aSocket, id, MessageTypes::ActiveRequests);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    if (j[MsgType] == MessageTypes::Error) {
        return j[Message];
    }
    std::string reply = "Your Sell Requests : \n";
    for (auto elem : j[SellRequests]) {
        reply += "Price: " + std::to_string(static_cast<int>(elem[0])) 
        + " Volume: " + std::to_string(static_cast<int>(elem[1])) 
        + "\n";
    }
    reply += "Your Buy Requests : \n";
    for (auto elem : j[BuyRequests])  {
        reply += "Price: " + std::to_string(static_cast<int>(elem[0])) 
        + " Volume: " + std::to_string(static_cast<int>(elem[1])) 
        + "\n";
    }
    return reply;

}

std::string DealsRequest(tcp::socket& aSocket, std::string id)
{
    SendMessage(aSocket, id, MessageTypes::Deals);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    if (j[MsgType] == MessageTypes::Error) {
        return j[Message];
    }
    std::string reply = "Your Deals : \n";
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

std::string BalanceRequest(tcp::socket& aSocket, std::string id)
{
    SendMessage(aSocket, id, MessageTypes::Balance, "");
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    if (j[MsgType] == MessageTypes::Error) {
        return j[Message];
    }
    std::string reply = "Your Balance : Dollars : " + std::to_string(static_cast<long long>(j[Dollars])) + "\n"
                      + "Rubles : " + std::to_string(static_cast<long long>(j[Rubles])) + "\n";
    
    return reply;
}

std::string SellRequest(tcp::socket& aSocket, std::string id)
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Sell;
    long long price_;
    long long volume_;
    std::cout << "Enter price: ";
    std::cin >> price_;
    std::cout << "Enter volume: ";
    std::cin >> volume_;
    req[Price] = price_;
    req[Volume] = volume_;
    std::string thisRequest = req.dump();
    SendMessage(aSocket, id, MessageTypes::Sell, thisRequest);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    return j[Message];
}

std::string BuyRequest(tcp::socket& aSocket, std::string id)
{
    nlohmann::json req;
    req[MsgType] = MessageTypes::Sell;
    long long price_;
    long long volume_;
    std::cout << "Enter price: ";
    std::cin >> price_;
    std::cout << "Enter volume: ";
    std::cin >> volume_;
    req[Price] = price_;
    req[Volume] = volume_;
    std::string thisRequest = req.dump();
    SendMessage(aSocket, id, MessageTypes::Buy, thisRequest);
    auto j = nlohmann::json::parse(ReadMessage(aSocket));
    return j[Message];
}
