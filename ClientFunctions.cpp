#include <iostream>
#include <boost/asio.hpp>

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const std::string& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

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

std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

std::string ProcessSellRequest(tcp::socket& aSocket, std::string id)
{
    int price;
    int volume;
    std::cout << "Enter price: ";
    std::cin >> price;
    std::cout << "Enter volume: ";
    std::cin >> volume;

    SendMessage(aSocket, id, Requests::Sell, std::to_string(price) + " " + std::to_string(volume));
    return ReadMessage(aSocket);
}

std::string ProcessBuyRequest(tcp::socket& aSocket, std::string id)
{
    int price;
    int volume;
    std::cout << "Enter price: ";
    std::cin >> price;
    std::cout << "Enter volume: ";
    std::cin >> volume;

    SendMessage(aSocket, id, Requests::Buy, std::to_string(price) + " " + std::to_string(volume));
    return ReadMessage(aSocket);
}
