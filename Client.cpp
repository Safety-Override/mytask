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

        std::cout << "Enter 1 to register, enter 2 to login.\n";
        int choice;
        std::cin >> choice;
        std::string my_id = "";
        nlohmann::json request;
        nlohmann::json response;
        if (choice == 1) {
            std::cout << "Hello! Enter your name: ";
            std::string my_name;
            std::cin >> my_name;
            std::cout << "Enter new login: ";
            std::string my_login;
            std::cin >> my_login;
            std::cout << "Enter new password: ";
            std::string my_password;
            std::cin >> my_password;

            request = RegistrationRequest(my_name, my_login, my_password);
            SendMessage(s, "0", request);
            response = nlohmann::json::parse(ReadMessage(s));
            if (response[MsgType] == MessageTypes::Error) {
                std::cout << ErrorResponse(response);
                exit(1);
            }
            my_id = RegistrationResponse(response);
        } else if (choice == 2) {
            std::cout << "Enter new login: ";
            std::string my_login;
            std::cin >> my_login;
            std::cout << "Enter new password: ";
            std::string my_password;
            std::cin >> my_password;

            request = LoginWithPasswordRequest(my_login, my_password);
            SendMessage(s, "0", request);
            response = nlohmann::json::parse(ReadMessage(s));
            if (response[MsgType] == MessageTypes::Error) {
                std::cout << ErrorResponse(response);
                exit(2);
            }
            my_id = LoginWithPasswordResponse(response);
        }

        while (true)
        {
            std::cout << "Menu:\n"
                         "1) Hello\n"
                         "2) Active requests\n"
                         "3) All deals\n"
                         "4) Balance\n"
                         "5) Create Sell request\n"
                         "6) Create Buy request\n"
                         "7) Quotations\n"
                         "8) Delete request\n"
                         "9) Exit\n"
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
                    request = QuotationsRequest();
                    break;
                }
                case 8:
                {
                    std::cout << "Enter price :\n";
                    long long price_;
                    std::cin >> price_;
                    std::cout << "Enter volume :\n";
                    long long volume_;
                    std::cin >> volume_;
                    std::cout << "Enter request type : \"sell\" or \"buy\" :\n";
                    std::string reqType;
                    std::cin >> reqType;
                    while (reqType != "sell" && reqType != "buy") {
                        std::cout << "Enter request type : \"sell\" or \"buy\" :\n";
                        std::cin >> reqType;
                    }
                    request = DeleteRequest(price_, volume_, reqType == "sell");
                    break;
                }
                case 9:
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
            auto resType = static_cast<MessageTypes>(response[MsgType]);
            switch(resType) {
                case MessageTypes::Error:
                    std::cout << ErrorResponse(response);
                    break;
                case MessageTypes::Hello:
                    std::cout << HelloResponse(response);
                    break;
                case MessageTypes::ActiveRequests:
                    std::cout << ActiveRequestsResponse(response);
                    break;
                case MessageTypes::Deals:
                    std::cout << DealsResponse(response);
                    break;
                case MessageTypes::Balance:
                    std::cout << BalanceResponse(response);
                    break;
                case MessageTypes::Sell:
                    std::cout << SellResponse(response);
                    break;
                case MessageTypes::Buy:
                    std::cout << BuyResponse(response);
                    break;
                case MessageTypes::Quotations:
                    std::cout << QuotationsResponse(response);
                    break;
                case MessageTypes::Delete:
                    std::cout << DeleteResponse(response);
                    break;
                default:
                {
                    std::cout << "Unknown error\n";
                    break;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}