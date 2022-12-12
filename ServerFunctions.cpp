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
    bool UserExists(const std::string& aUserId) {
        const size_t id = std::stoi(aUserId);
        return profiles.find(id) != profiles.end();
    }

    std::string RegisterNewUser(const std::string& aUserName) {
        size_t newUserId = profiles.size();
        profiles[newUserId].userName = aUserName;
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Registration;
        rep[Message] = std::to_string(newUserId);
        return rep.dump();
    }


    std::string GetHello(const std::string& aUserId) {
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Hello;
        rep[Message] = "Hello, " + GetUserName(aUserId) + "!\n";
        return rep.dump();
    }
    
    std::string GetActiveRequests(const std::string& aUserId)
    {
        nlohmann::json rep;
        const size_t id = std::stoi(aUserId);
        rep[MsgType] = MessageTypes::ActiveRequests;
        std::vector<std::pair<int, int>> vSellRequests;
        std::vector<std::pair<int, int>> vBuyRequests;
        for (auto elem : sellQueue) {
            if (id == elem.userId) {
                vSellRequests.push_back({elem.price, elem.volume});
            }
        }
        for (auto elem : buyQueue) {
            if (id == elem.userId) {
                vBuyRequests.push_back({elem.price, elem.volume});
            }
        }
        rep[SellRequests] = vSellRequests;
        rep[BuyRequests] = vBuyRequests;
        return rep.dump();
    }

    std::string GetDeals(const std::string& aUserId)
    {
        nlohmann::json rep;
        const size_t id = std::stoi(aUserId);
        rep[MsgType] = MessageTypes::Deals;
        std::vector<ns::Deal> vDealsList;
        for (auto elem : logs) {
            if (id == elem.sellerId) {
                vDealsList.push_back({elem.price, elem.volume, GetUserName(std::to_string(elem.buyerId))});
                
            }
        }
        rep[SellRequests][DealsList] = vDealsList;
        vDealsList.clear();
        for (auto elem : logs) {
            if (id == elem.buyerId) {
                vDealsList.push_back({elem.price, elem.volume, GetUserName(std::to_string(elem.sellerId))});
            }
        }
        rep[BuyRequests][DealsList] = vDealsList;
        return rep.dump();
    }

    std::string GetBalance(const std::string& aUserId)
    {
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Balance;
        const size_t id = std::stoi(aUserId);
        rep[Dollars] = profiles[id].dollars;
        rep[Rubles] = profiles[id].rubles;
        return rep.dump();
    }

    std::string ProcessSellRequest(const std::string& aUserId, const std::string& message)
    {
        auto j = nlohmann::json::parse(message);
        const size_t id = std::stoi(aUserId);
        long long price_ = j[Price];
        long long volume_ = j[Volume];
        ProcessSell(id, price_, volume_);
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Sell;
        rep[Message] = "Processed\n";
        return rep.dump();
    }

    std::string ProcessBuyRequest(const std::string& aUserId, const std::string& message)
    {
        auto j = nlohmann::json::parse(message);
        const size_t id = std::stoi(aUserId);
        long long price_ = j[Price];
        long long volume_ = j[Volume];
        ProcessBuy(id, price_, volume_);
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Buy;
        rep[Message] = "Processed\n";
        return rep.dump();
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

    std::string GetUserName(const std::string& aUserId)
    {
        return profiles.find(std::stoi(aUserId))->second.userName;
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
            auto reqType = j[MsgType];
            std::string reply = "";
            nlohmann::json rep;
            if (reqType == MessageTypes::Registration)
            {
                reply = GetCore().RegisterNewUser(j[Message]);
            } else if (!GetCore().UserExists(j[UserId])) {
                rep[MsgType] = MessageTypes::Error;
                rep[Message] = "Error : User doesn't exist.\n";
                reply = rep.dump();
            } else if (reqType == MessageTypes::Hello)
            {
                reply = GetCore().GetHello(j[UserId]);
            }
            else if (reqType == MessageTypes::ActiveRequests)
            {
                reply = GetCore().GetActiveRequests(j[UserId]);
            }
            else if (reqType == MessageTypes::Deals)
            {
                reply = GetCore().GetDeals(j[UserId]);
            }
            else if (reqType == MessageTypes::Balance)
            {
                reply = GetCore().GetBalance(j[UserId]);
            }
            else if (reqType == MessageTypes::Sell)
            {
                reply = GetCore().ProcessSellRequest(j[UserId], j[Message]);
            }
            else if (reqType == MessageTypes::Buy)
            {
                reply = GetCore().ProcessBuyRequest(j[UserId], j[Message]);
            } else {
                nlohmann::json rep;
                rep[MsgType] = MessageTypes::Error;
                rep[Message] = "Error! Unknown request";
                reply = rep.dump();
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
