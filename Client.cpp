#include <iostream>
#include <boost/asio.hpp>

#include "ClientFunctions.cpp"

#include "Common.hpp"
#include "json.hpp"

using boost::asio::ip::tcp;

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        std::cout << "Hello! Enter your name: ";
        std::string my_name;
        std::cin >> my_name;
        nlohmann::json request = RegistrationRequest(my_name);
        SendMessage(s, "0", request);
        nlohmann::json response = nlohmann::json::parse(ReadMessage(s));
        if (response[MsgType] == MessageTypes::Error) {
            std::cout << ErrorResponse(response);
            exit(1);
        }
        std::string my_id = RegistrationResponse(response);
        while (true)
        {
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Active requests\n"
                         "3) All deals\n"
                         "4) Balance\n"
                         "5) Create Sell request\n"
                         "6) Create Buy request\n"
                         "7) Exit\n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    request = HelloRequest();
                    break;
                }
                case 2:
                {
                    request = ActiveRequestsRequest();
                    break;
                }
                case 3:
                {
                    request = DealsRequest();
                    break;
                }
                case 4:
                {
                    request = BalanceRequest();
                    break;
                }
                case 5:
                {
                    long long price_;
                    long long volume_;
                    std::cout << "Enter price: ";
                    std::cin >> price_;
                    std::cout << "Enter volume: ";
                    std::cin >> volume_;
                    request = SellRequest(price_, volume_);
                    break;
                }
                case 6:
                {
                    long long price_;
                    long long volume_;
                    std::cout << "Enter price: ";
                    std::cin >> price_;
                    std::cout << "Enter volume: ";
                    std::cin >> volume_;
                    request = BuyRequest(price_, volume_);
                    break;
                }
                case 7:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n";
                    continue;
                }
            }
            SendMessage(s, my_id, request);
            response = nlohmann::json::parse(ReadMessage(s));
            auto resType = response[MsgType];
            if (resType == MessageTypes::Error) {
                std::cout << ErrorResponse(response);
            } else if (resType == MessageTypes::Hello) {
                std::cout << HelloResponse(response);
            } else if (resType == MessageTypes::ActiveRequests) {
                std::cout << ActiveRequestsResponse(response);
            } else if (resType == MessageTypes::Deals) {
                std::cout << DealsResponse(response);
            } else if (resType == MessageTypes::Balance) {
                std::cout << BalanceResponse(response);
            } else if (resType == MessageTypes::Sell) {
                std::cout << SellResponse(response);
            } else if (resType == MessageTypes::Buy) {
                std::cout << BuyResponse(response);
            } else {
                std::cout << "Unknown error\n";
            }
            
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}