#include <cstdlib>
#include <iostream>
#include <set>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include "Common.hpp"

using boost::asio::ip::tcp;

class Core
{
public:
    std::string RegisterNewUser(const std::string& aUserName)
    {
        size_t newUserId = profiles.size();
        profiles[newUserId].userName = aUserName;

        return std::to_string(newUserId);
    }

    std::string GetUserName(const std::string& aUserId)
    {
        const auto userIt = profiles.find(std::stoi(aUserId));
        if (userIt == profiles.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second.userName;
        }
    }

    std::string GetActiveRequests(const std::string& aUserId)
    {
        const auto id = std::stoi(aUserId);
        std::string reply = "Your Sell Requests : \n";
        for (auto elem : sellQueue) {
            if (id == elem.userId) {
                reply += "Price: " + std::to_string(elem.price) + " Volume: " + std::to_string(elem.volume) + "\n";
            }
        }
        reply += "Your Buy Requests : \n";
        for (auto elem : buyQueue) {
            if (id == elem.userId) {
                reply += "Price: " + std::to_string(elem.price) + " Volume: " + std::to_string(elem.volume) + "\n";
            }
        }
        return reply;
    }

    std::string GetDeals(const std::string& aUserId)
    {
        const auto id = std::stoi(aUserId);
        std::string reply = "Your Deals : \n";
        for (auto elem : logs) {
            if (id == elem.sellerId) {
                reply += "You sold to " + GetUserName(std::to_string(elem.buyerId)) + ". Price: " + std::to_string(elem.price) + " Volume: " + std::to_string(elem.volume) + "\n";
            }
            if (id == elem.buyerId) {
                reply += "You bought from " + GetUserName(std::to_string(elem.sellerId)) + ". Price: " + std::to_string(elem.price) + " Volume: " + std::to_string(elem.volume) + "\n";
            }
        }
        return reply;
    }

    std::string GetBalance(const std::string& aUserId)
    {
        const size_t id = std::stoi(aUserId);
        std::string reply = "Your Balance : Dollars : " + std::to_string(profiles[id].dollars) + "\n"
                            + "Rubles : " + std::to_string(profiles[id].rubles) + "\n";
        return reply;
    }

    std::string ProcessSellRequest(const std::string& aUserId, const std::string& message)
    {
        const size_t id = std::stoi(aUserId);
        int price = std::stoi(message.substr(0, message.find(' ')));
        int volume = std::stoi(message.substr(message.find(' ')));
        ProcessSell(id, price, volume);
        std::string reply = "Processed\n";
        return reply;
    }

    std::string ProcessBuyRequest(const std::string& aUserId, const std::string& message)
    {
        const size_t id = std::stoi(aUserId);
        int price = std::stoi(message.substr(0, message.find(' ')));
        int volume = std::stoi(message.substr(message.find(' ')));
        ProcessBuy(id, price, volume);
        std::string reply = "Processed\n";
        return reply;
    }
private:
    struct profileType {
        long long dollars = 0;
        long long rubles = 0;
        std::string userName;
    };
    std::unordered_map<size_t, profileType> profiles;
    struct logType {
        size_t price;
        size_t volume;
        size_t sellerId;
        size_t buyerId;
    };
    std::vector<logType> logs;
    struct sellReq{
        size_t price;
        size_t timeStep;
        mutable size_t volume;
        size_t userId;
        bool operator<(const sellReq r)const
        {
            if (price == r.price) {
                return timeStep < r.timeStep;
            }
            return price < r.price;
        };
    };
    struct buyReq{
        size_t price;
        size_t timeStep;
        mutable size_t volume;
        size_t userId;
        bool operator<(const buyReq r)const
        {
            if (price == r.price) {
                return timeStep < r.timeStep;
            }
            return price > r.price;
        };
    };
    std::set<sellReq> sellQueue;
    std::set<buyReq> buyQueue;
    size_t curTimeStep = 0;

    void ProcessTransaction(size_t price, size_t volume, size_t sellerId, size_t buyerId) {
        logType temp;
        temp.price = price;
        temp.volume = volume;
        temp.sellerId = sellerId;
        temp.buyerId = buyerId;
        logs.push_back(temp);
        profiles[sellerId].dollars -= volume;
        profiles[sellerId].rubles += price * volume;
        profiles[buyerId].dollars += volume;
        profiles[buyerId].rubles -= price * volume;
    }

    void ProcessSell(size_t id, int price, int volume) {
        size_t curPrice;
        size_t curVolume;
        while (!buyQueue.empty() && volume) {
            curPrice = buyQueue.begin()->price;
            curVolume = buyQueue.begin()->volume;
            if (price <= curPrice) {
                if (volume < curVolume) {
                    ProcessTransaction(curPrice, volume, id, buyQueue.begin()->userId);
                    buyQueue.begin()->volume = curVolume - volume;
                    volume = 0;
                } else if (volume == curVolume) {
                    ProcessTransaction(curPrice, volume, id, buyQueue.begin()->userId);
                    buyQueue.erase(buyQueue.begin());
                    volume = 0;
                } else {
                    ProcessTransaction(curPrice, curVolume, id, buyQueue.begin()->userId);
                    volume -= curVolume;
                    buyQueue.erase(buyQueue.begin());
                }
            } else {
                sellReq temp;
                temp.price = price;
                temp.volume = volume;
                temp.timeStep = curTimeStep++;
                temp.userId = id;
                sellQueue.insert(temp);
                volume = 0;
            }
        }
        if (volume) {
            sellReq temp2;
            temp2.price = price;
            temp2.volume = volume;
            temp2.timeStep = curTimeStep++;
            temp2.userId = id;
            sellQueue.insert(temp2);
        }
    }

    void ProcessBuy(size_t id, int price, int volume) {
        size_t curPrice;
        size_t curVolume;
        while (!sellQueue.empty() && volume) {
            curPrice = sellQueue.begin()->price;
            curVolume = sellQueue.begin()->volume;
            if (price >= curPrice) {
                if (volume < curVolume) {
                    ProcessTransaction(curPrice, volume, sellQueue.begin()->userId, id);
                    sellQueue.begin()->volume = curVolume - volume;
                    volume = 0;
                } else if (volume == curVolume) {
                    ProcessTransaction(curPrice, volume, sellQueue.begin()->userId, id);
                    sellQueue.erase(sellQueue.begin());
                    volume = 0;
                } else {
                    ProcessTransaction(curPrice, curVolume, sellQueue.begin()->userId, id);
                    volume -= curVolume;
                    sellQueue.erase(sellQueue.begin());
                }
            } else {
                buyReq temp;
                temp.price = price;
                temp.volume = volume;
                temp.timeStep = curTimeStep++;
                temp.userId = id;
                buyQueue.insert(temp);
                volume = 0;
            }
        }
        if (volume) {
            buyReq temp2;
            temp2.price = price;
            temp2.volume = volume;
            temp2.timeStep = curTimeStep++;
            temp2.userId = id;
            buyQueue.insert(temp2);
        }
    }
};

Core& GetCore()
{
    static Core core;
    return core;
}

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            data_[bytes_transferred] = '\0';

            auto j = nlohmann::json::parse(data_);
            auto reqType = j["ReqType"];

            std::string reply = "Error! Unknown request type";
            if (reqType == Requests::Registration)
            {
                reply = GetCore().RegisterNewUser(j["Message"]);
            }
            else if (reqType == Requests::Hello)
            {
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            }
            else if (reqType == Requests::ActiveRequests)
            {
                reply = GetCore().GetActiveRequests(j["UserId"]);
            }
            else if (reqType == Requests::Deals)
            {
                reply = GetCore().GetDeals(j["UserId"]);
            }
            else if (reqType == Requests::Balance)
            {
                reply = GetCore().GetBalance(j["UserId"]);
            }
            else if (reqType == Requests::Sell)
            {
                reply = GetCore().ProcessSellRequest(j["UserId"], j["Message"]);
            }
            else if (reqType == Requests::Buy)
            {
                reply = GetCore().ProcessBuyRequest(j["UserId"], j["Message"]);
            }
            
            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};
