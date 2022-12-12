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

        std::string my_id = RegistrationRequest(s);

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
                    std::cout << HelloRequest(s, my_id);
                    break;
                }
                case 2:
                {
                    std::cout << ActiveRequestsRequest(s, my_id);
                    break;
                }
                case 3:
                {
                    std::cout << DealsRequest(s, my_id);
                    break;
                }
                case 4:
                {
                    std::cout << BalanceRequest(s, my_id);
                    break;
                }
                case 5:
                {
                    std::cout << SellRequest(s, my_id);
                    break;
                }
                case 6:
                {
                    std::cout << BuyRequest(s, my_id);
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