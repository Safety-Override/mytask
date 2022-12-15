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
    nlohmann::json RegisterNewUser(const std::string& aUserName) {
        size_t newUserId = profiles.size();
        profiles[newUserId].userName = aUserName;
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Registration;
        rep[Message] = std::to_string(newUserId);
        return rep;
    }

    nlohmann::json GetError(const std::string aMessage) {
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Error;
        rep[Message] = aMessage;
        return rep;
    }

    nlohmann::json GetHello(const std::string& aUserId) {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Hello;
        rep[Message] = "Hello, " + GetUserName(aUserId) + "!\n";
        return rep;
    }
    
    nlohmann::json GetActiveRequests(const std::string& aUserId)
    {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
        nlohmann::json rep;
        const size_t id = std::stoi(aUserId);
        rep[MsgType] = MessageTypes::ActiveRequests;
        std::vector<std::pair<long long, long long>> vSellRequests;
        std::vector<std::pair<long long, long long>> vBuyRequests;
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
        return rep;
    }

    nlohmann::json GetDeals(const std::string& aUserId)
    {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
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
        return rep;
    }

    nlohmann::json GetBalance(const std::string& aUserId)
    {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Balance;
        const size_t id = std::stoi(aUserId);
        rep[Dollars] = profiles[id].dollars;
        rep[Rubles] = profiles[id].rubles;
        return rep;
    }

    nlohmann::json ProcessSellRequest(const std::string& aUserId, long long price_, long long volume_)
    {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
        if (price_ <= 0 || volume_ <= 0) {
            return GetError("Incorrect price or volume.\n");
        }
        const size_t id = std::stoi(aUserId);
        ProcessSell(id, price_, volume_);
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Sell;
        rep[Message] = "Processed.\n";
        return rep;
    }

    nlohmann::json ProcessBuyRequest(const std::string& aUserId, long long price_, long long volume_)
    {
        if (!UserExists(aUserId)) {
            return GetError("Error : User doesn't exist.\n");
        }
        if (price_ <= 0 || volume_ <= 0) {
            return GetError("Incorrect price or volume.\n");
        }
        const size_t id = std::stoi(aUserId);
        ProcessBuy(id, price_, volume_);
        nlohmann::json rep;
        rep[MsgType] = MessageTypes::Buy;
        rep[Message] = "Processed.\n";
        return rep;
    }
private:
    struct profileType {
        long long dollars = 0;
        long long rubles = 0;
        std::string userName;
    };
    std::unordered_map<size_t, profileType> profiles;
    struct logType {
        long long price;
        long long volume;
        size_t sellerId;
        size_t buyerId;
    };
    std::vector<logType> logs;
    struct sellReq{
        long long price;
        size_t timeStep;
        mutable long long volume;
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
        long long price;
        size_t timeStep;
        mutable long long volume;
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

    void ProcessTransaction(long long price, long long volume, size_t sellerId, size_t buyerId) {
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

    void ProcessSell(size_t id, long long price, long long volume) {
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

    void ProcessBuy(size_t id, long long price, long long volume) {
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

    bool UserExists(const std::string& aUserId) {
        const size_t id = std::stoi(aUserId);
        return profiles.find(id) != profiles.end();
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

            nlohmann::json j = nlohmann::json::parse(data_);
            MessageTypes reqType = static_cast<MessageTypes>(j[MsgType]);
            nlohmann::json rep;
            switch(reqType) {
                case MessageTypes::Registration:
                    rep = GetCore().RegisterNewUser(j[Message]);
                    break;
                case MessageTypes::Hello:
                    rep = GetCore().GetHello(j[UserId]);
                    break;
                case MessageTypes::ActiveRequests:
                    rep = GetCore().GetActiveRequests(j[UserId]);
                    break;
                case MessageTypes::Deals:
                    rep = GetCore().GetDeals(j[UserId]);
                    break;
                case MessageTypes::Balance:
                    rep = GetCore().GetBalance(j[UserId]);
                    break;
                case MessageTypes::Sell:
                    rep = GetCore().ProcessSellRequest(j[UserId], j[Price], j[Volume]);
                    break;
                case MessageTypes::Buy:
                    rep = GetCore().ProcessBuyRequest(j[UserId], j[Price], j[Volume]);
                    break;
                default:
                {
                    rep = GetCore().GetError("Error! Unknown request.\n");
                    break;
                }
            }
            std::string reply = rep.dump();
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
