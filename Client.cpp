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

        std::string my_id = ProcessRegistration(s);

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
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    SendMessage(s, my_id, Requests::ActiveRequests, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 3:
                {
                    SendMessage(s, my_id, Requests::Deals, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 4:
                {
                    SendMessage(s, my_id, Requests::Balance, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 5:
                {
                    std::cout << ProcessSellRequest(s, my_id);
                    break;
                }
                case 6:
                {
                    std::cout << ProcessBuyRequest(s, my_id);
                    break;
                }
                case 7:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
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