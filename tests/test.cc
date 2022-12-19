#include <gtest/gtest.h>
#include "../ServerFunctions.cpp"
#include "../ClientFunctions.cpp"

// Demonstrate some basic assertions.
TEST(ClientTest, HelloTest) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);

}


TEST(ClientTest, RegistrationRequestTest) {
  std::string name = "1000_*(*$&$?///}{[TestUser00____";
  std::string login = "2000_*(*$&$?///}{[TestUser00____";
  std::string password = "3000_*(*$&$?///}{[TestUser00____";
  nlohmann::json j = RegistrationRequest(name, login, password);
  ASSERT_EQ(j[UserName], "1000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[Login], "2000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[Password], "3000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[MsgType], MessageTypes::Registration);
}

TEST(ClientTest, LoginWithPasswordRequestTest) {
  std::string login = "1000_*(*$&$?///}{[TestUser00____";
  std::string password = "2000_*(*$&$?///}{[TestUser00____";
  nlohmann::json j = LoginWithPasswordRequest(login, password);
  ASSERT_EQ(j[Login], "1000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[Password], "2000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[MsgType], MessageTypes::Authorization);
}

TEST(ClientTest, HelloRequestTest) {
nlohmann::json j = HelloRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Hello);
}

TEST(ClientTest, ActiveRequestsRequestTest) {
nlohmann::json j = ActiveRequestsRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::ActiveRequests);
}

TEST(ClientTest, DealsRequestTest) {
nlohmann::json j = DealsRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Deals);
}

TEST(ClientTest, BalanceRequestTest) {
  nlohmann::json j = BalanceRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Balance);
}

TEST(ClientTest, SellRequestTest) {
  nlohmann::json j = SellRequest(7, 3);
  ASSERT_EQ(j[MsgType], MessageTypes::Sell);
  ASSERT_EQ(j[Price], 7);
  ASSERT_EQ(j[Volume], 3);
}

TEST(ClientTest, BuyRequestTest) {
  nlohmann::json j = BuyRequest(7, 3);
  ASSERT_EQ(j[MsgType], MessageTypes::Buy);
  ASSERT_EQ(j[Price], 7);
  ASSERT_EQ(j[Volume], 3);
}

TEST(ClientTest, QuotationsRequestTest) {
  nlohmann::json j = QuotationsRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Quotations);
}

TEST(ClientTest, DeleteRequestTest) {
  nlohmann::json j = DeleteRequest(7, 3, true);
  ASSERT_EQ(j[MsgType], MessageTypes::Delete);
  ASSERT_EQ(j[ReqType], MessageTypes::Sell);
  ASSERT_EQ(j[Price], 7);
  ASSERT_EQ(j[Volume], 3);
  j = DeleteRequest(6, 2, false);
  ASSERT_EQ(j[MsgType], MessageTypes::Delete);
  ASSERT_EQ(j[ReqType], MessageTypes::Buy);
  ASSERT_EQ(j[Price], 6);
  ASSERT_EQ(j[Volume], 2);
}

TEST(ClientTest, ErrorResponseTest) {
  nlohmann::json j;
  std::string errorMsg = "Custom Error.\n";
  j[Message] = "Custom Error.\n";
  ASSERT_EQ(ErrorResponse(j), errorMsg);
}

TEST(ClientTest, RegistrationResponseTest) {
  nlohmann::json j;
  j[UserId] = std::to_string(0);
  std::string rep = "0";
  ASSERT_EQ(RegistrationResponse(j), rep);
}

TEST(ClientTest, LoginWithPasswordResponseTest) {
  nlohmann::json j;
  j[UserId] = std::to_string(0);
  std::string rep = "0";
  ASSERT_EQ(LoginWithPasswordResponse(j), rep);
}

TEST(ClientTest, HelloResponseTest) {
  nlohmann::json j;
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  j[Message] = "Hello, " + name + "\n";
  ASSERT_EQ(HelloResponse(j), "Hello, " + name + "\n");
}

TEST(ClientTest, ActiveRequestsResponseTest) {
  nlohmann::json j;
  std::vector<std::pair<long long, long long>> vSellRequests;
  std::vector<std::pair<long long, long long>> vBuyRequests;
  for (long long i = 0; i < 3; ++i) {
          vSellRequests.push_back({i + 1, i + 2});
  }
  for  (long long i = 0; i < 3; ++i) {
          vBuyRequests.push_back({i + 1, i + 3});
  }
  j[SellRequests] = vSellRequests;
  j[BuyRequests] = vBuyRequests;
  std::string ans = "Your Sell Requests :\nPrice: 1 Volume: 2\nPrice: 2 Volume: 3\nPrice: 3 Volume: 4\nYour Buy Requests :\nPrice: 1 Volume: 3\nPrice: 2 Volume: 4\nPrice: 3 Volume: 5\n";
  ASSERT_EQ(ActiveRequestsResponse(j), ans);
}

TEST(ClientTest, DealsResponseTest) {
  nlohmann::json j;
  std::vector<ns::Deal> vDealsList;
  for (long long i = 0; i < 2; ++i) {
    vDealsList.push_back({i + 2, i + 7, "TestUser_" + std::to_string(i + 4)});
  }
  j[SellRequests][DealsList] = vDealsList;
  vDealsList.clear();
  for (long long i = 0; i < 2; ++i) {
    vDealsList.push_back({i + 4, i + 9, "TestUser_" + std::to_string(i + 15)});
  }
  j[BuyRequests][DealsList] = vDealsList;

  std::string ans = "Your Deals :\nYou sold to TestUser_4. Price: 2 Volume: 7\nYou sold to TestUser_5. Price: 3 Volume: 8\nYou bought from TestUser_15. Price: 4 Volume: 9\nYou bought from TestUser_16. Price: 5 Volume: 10\n";
  ASSERT_EQ(DealsResponse(j), ans);
}

TEST(ClientTest, BalanceResponseTest) {
  nlohmann::json j;
  j[Dollars] = 9001;
  j[Rubles] = 444;
  std::string ans = "Your Balance : Dollars : 9001\nRubles : 444\n";
  ASSERT_EQ(BalanceResponse(j), ans);
}

TEST(ClientTest, SellResponseTest) {
  nlohmann::json j;
  j[Message] = "Processed.\n";
  std::string ans = "Processed.\n";
  ASSERT_EQ(SellResponse(j), ans);
}

TEST(ClientTest, BuyResponseTest) {
  nlohmann::json j;
  j[Message] = "Processed.\n";
  std::string ans = "Processed.\n";
  ASSERT_EQ(BuyResponse(j), ans);
}

TEST(ClientTest, QuotationsResponseTest) {
  nlohmann::json j;
  long long buyPrice = 123;
  long long sellPrice = 321;
  j[SellRequests][Price] = buyPrice;//sell offer price == buy price
  j[BuyRequests][Price] = sellPrice;
  std::string reply = "Buy price : " + std::to_string(buyPrice) + "\n";
  reply += "Sell price : " + std::to_string(sellPrice) + "\n";
  ASSERT_EQ(QuotationsResponse(j), reply);

  buyPrice = -1;
  sellPrice = 321;
  j[SellRequests][Price] = buyPrice;//sell offer price == buy price
  j[BuyRequests][Price] = sellPrice;
  reply = "No sell offers\n";
  reply += "Sell price : " + std::to_string(sellPrice) + "\n";
  ASSERT_EQ(QuotationsResponse(j), reply);

  buyPrice = 123;
  sellPrice = -1;
  j[SellRequests][Price] = buyPrice;//sell offer price == buy price
  j[BuyRequests][Price] = sellPrice;
  reply = "Buy price : " + std::to_string(buyPrice) + "\n";
  reply += "No buy offers\n";
  ASSERT_EQ(QuotationsResponse(j), reply);

  buyPrice = -1;
  sellPrice = -1;
  j[SellRequests][Price] = buyPrice;//sell offer price == buy price
  j[BuyRequests][Price] = sellPrice;
  reply = "No sell offers\n";
  reply += "No buy offers\n";
  ASSERT_EQ(QuotationsResponse(j), reply);
}

TEST(ClientTest, DeleteResponseTest) {
  nlohmann::json j;
  j[Message] = "Request deleted.\n";
  std::string ans = j[Message];
  ASSERT_EQ(DeleteResponse(j), ans);
}

//////////////////////////////////////////////////////////

struct RegisterNewUserTestResult {
  std::string name1;
  std::string name2;
  nlohmann::json reply;
};

struct RegisterNewUserWithAuthTestResult {
  std::string password;
  std::string id;
  nlohmann::json reply;
};

struct LoginWithPasswordTestResult {
  std::string password;
  std::string id;
  std::vector<nlohmann::json> reply;
};

struct GetErrorTestResult {
  nlohmann::json reply;
};

struct GetHelloTestResult {
  std::vector<nlohmann::json> reply;
};

struct GetActiveRequestsTestResult {
  std::vector<nlohmann::json> reply;
};

struct GetDealsTestResult {
  std::vector<nlohmann::json> reply;
};

struct GetBalanceTestResult {
  std::vector<nlohmann::json> reply;
};

struct fakeRequest {
  long long price;
  size_t timeStep;
  long long volume;
  size_t userId;
};

struct ProcessSellRequestTestResult {
  std::vector<nlohmann::json> reply;
  std::vector<fakeRequest> BuyQueueState;
  std::vector<fakeRequest> SellQueueState;
};

struct ProcessBuyRequestTestResult {
  std::vector<nlohmann::json> reply;
  std::vector<fakeRequest> BuyQueueState;
  std::vector<fakeRequest> SellQueueState;
};

struct ProcessQuotationsRequestTestResult {
  std::vector<nlohmann::json> reply;
  long long sellPrice;
  long long buyPrice;
};

struct ProcessDeleteRequestTestResult {
  std::vector<nlohmann::json> reply;
  std::vector<bool> foundBeforeDeletion;
  std::vector<bool> foundAfterDeletion;
};

struct fakeLogType {
  long long price;
  long long volume;
  size_t sellerId;
  size_t buyerId;
};


class CoreTest : public ::testing::Test
{
protected:
    RegisterNewUserWithAuthTestResult RegisterNewUserWithAuthTestFunction(const std::string& aUserName_, const std::string& login_, const std::string& password_) {
      RegisterNewUserWithAuthTestResult result;
      result.reply = core.RegisterNewUserWithAuth(aUserName_, login_, password_);
      result.id = std::to_string(core.authData[login_].userId);
      result.password = core.authData[login_].password;
      return result;
    }

    RegisterNewUserTestResult RegisterNewUserTestFunction(std::string name) { 
      RegisterNewUserTestResult result;
      nlohmann::json reply = core.RegisterNewUser(name);
      result.name1 = core.profiles[0].userName;
      std::string strId = reply[UserId];
      result.name2 = core.GetUserName(strId);
      result.reply = reply;
      return result;
    }

    LoginWithPasswordTestResult LoginWithPasswordTestFunction(std::string login, std::string password) { 
      LoginWithPasswordTestResult result;
      result.reply.push_back(core.LoginWithPassword(login, password));
      std::string rname = "user11";
      core.authData[login].password = password;
      core.authData[login].userId = 5;
      result.id = std::to_string(core.authData[login].userId);
      result.password = core.authData[login].password;
      result.reply.push_back(core.LoginWithPassword(login, password));
      return result;
    }

    GetErrorTestResult GetErrorTestFunction(std::string aMessage) { 
      GetErrorTestResult result;
      result.reply = core.GetError(aMessage);
      return result;
    }

    GetHelloTestResult GetHelloTestFunction(std::string name) { 
      GetHelloTestResult result;
      std::string id = "0";
      result.reply.push_back(core.GetHello(id));
      core.RegisterNewUser(name);
      result.reply.push_back(core.GetHello(id));
      return result;
    }

    GetActiveRequestsTestResult GetActiveRequestsTestFunction(std::vector<std::pair<long long, long long>> vBuyRequests,
        std::vector<std::pair<long long, long long>> vSellRequests) {
      std::string name = "0000_*(*$&$?///}{[TestUser00____";
      GetActiveRequestsTestResult result;
      std::string id = "0";
      result.reply.push_back(core.GetActiveRequests(id));
      core.RegisterNewUser(name);
      size_t timestep = 0;
      for (auto elem : vBuyRequests) {
        core.buyQueue.insert({elem.first, 0, elem.second, 0});//price, timestep, volume, user id//
      }
      for (auto elem : vSellRequests) {
        core.sellQueue.insert({elem.first, 0, elem.second, 0});
      }
      result.reply.push_back(core.GetActiveRequests(id));
      return result;
    }

    GetDealsTestResult GetDealsTestFunction(std::vector<fakeLogType> myLogs, std::string names[4]) {
      std::string ids[4];
      for (int i = 0; i < 4; ++i) {
        ids[i] = core.RegisterNewUser(names[i])[UserId];
      }
      GetDealsTestResult result;
      core.logs.resize(myLogs.size());
      for (size_t i = 0; i < myLogs.size(); ++i) {
        core.logs[i].price = myLogs[i].price;
        core.logs[i].volume = myLogs[i].volume;
        core.logs[i].sellerId = myLogs[i].sellerId;
        core.logs[i].buyerId = myLogs[i].buyerId;
      }
      for (int i = 0; i < 4; ++i) {
        result.reply.push_back(core.GetDeals(ids[i]));
      }
      result.reply.push_back(core.GetDeals("4"));
      return result;
    }

    GetBalanceTestResult GetBalanceTestFunction(long long dollars, long long rubles) {
      GetBalanceTestResult result;
      std::string name = "theName";
      std::string id = "0";
      result.reply.push_back(core.GetBalance(id));
      id = core.RegisterNewUser(name)[UserId];
      result.reply.push_back(core.GetBalance(id));
      core.profiles[std::stoi(id)].dollars = dollars;
      core.profiles[std::stoi(id)].rubles = rubles;
      result.reply.push_back(core.GetBalance(id));
      return result;
    }

    ProcessSellRequestTestResult ProcessSellRequestTestFunction(long long price_, long long volume_, std::vector<fakeRequest> vBuyQueue) {
      std::string name1 = "sellerman";
      std::string id1 = "0";
      std::string name2 = "buyerman";
      std::string id2 = "1";
      id1 = core.RegisterNewUser(name1)[UserId];
      id2 = core.RegisterNewUser(name2)[UserId];
      core.buyQueue.clear();
      core.sellQueue.clear();
      for (auto elem : vBuyQueue) {
        Core::buyReq temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = std::stoi(id2);//elem.userId;
        temp.timeStep = elem.timeStep;
        core.buyQueue.insert(temp);
      }
      core.curTimeStep = vBuyQueue[vBuyQueue.size() - 1].timeStep + 1;
      ProcessSellRequestTestResult result;
      result.reply.push_back(core.ProcessSellRequest(id1, price_, volume_));
      result.reply.push_back(core.GetBalance(id1));
      for (auto elem : core.buyQueue) {
        fakeRequest temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = elem.userId;
        temp.timeStep = elem.timeStep;
        result.BuyQueueState.push_back(temp);
      }
      for (auto elem : core.sellQueue) {
        fakeRequest temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = elem.userId;
        temp.timeStep = elem.timeStep;
        result.SellQueueState.push_back(temp);
      }
      return result;
    }

    ProcessBuyRequestTestResult ProcessBuyRequestTestFunction(long long price_, long long volume_, std::vector<fakeRequest> vSellQueue) {
      std::string name1 = "buyerman";
      std::string id1 = "0";
      std::string name2 = "sellerman";
      std::string id2 = "1";
      id1 = core.RegisterNewUser(name1)[UserId];
      id2 = core.RegisterNewUser(name2)[UserId];
      core.buyQueue.clear();
      core.sellQueue.clear();
      for (auto elem : vSellQueue) {
        Core::sellReq temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = std::stoi(id2);//elem.userId;
        temp.timeStep = elem.timeStep;
        core.sellQueue.insert(temp);
      }
      core.curTimeStep = vSellQueue[vSellQueue.size() - 1].timeStep + 1;
      ProcessBuyRequestTestResult result;
      result.reply.push_back(core.ProcessBuyRequest(id1, price_, volume_));
      result.reply.push_back(core.GetBalance(id1));
      for (auto elem : core.buyQueue) {
        fakeRequest temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = elem.userId;
        temp.timeStep = elem.timeStep;
        result.BuyQueueState.push_back(temp);
      }
      for (auto elem : core.sellQueue) {
        fakeRequest temp;
        temp.price = elem.price;
        temp.volume = elem.volume;
        temp.userId = elem.userId;
        temp.timeStep = elem.timeStep;
        result.SellQueueState.push_back(temp);
      }
      return result;
    }

    ProcessQuotationsRequestTestResult ProcessQuotationsRequestTestFunction() {
      ProcessQuotationsRequestTestResult result;
      std::string name1 = "buyerman";
      std::string strid = "0";
      strid = core.RegisterNewUser(name1)[UserId];
      size_t id = 0;
      result.reply.push_back(core.ProcessQuotationsRequest(strid));
      Core::sellReq temp1;
      temp1.price = 5;
      temp1.timeStep = 0;
      temp1.userId = 0;
      core.sellQueue.insert(temp1);
      result.reply.push_back(core.ProcessQuotationsRequest(strid));
      Core::buyReq temp2;
      temp2.price = 2;
      temp2.timeStep = 1;
      temp2.userId = 0;
      core.buyQueue.insert(temp2);
      result.reply.push_back(core.ProcessQuotationsRequest(strid));
      return result;
    }

    ProcessDeleteRequestTestResult ProcessDeleteRequestTestFunction() {
      ProcessDeleteRequestTestResult result;
      std::string name1 = "buyerman";
      std::string strid = "0";
      strid = core.RegisterNewUser(name1)[UserId];
      result.foundBeforeDeletion.push_back(!core.sellQueue.empty());
      result.reply.push_back(core.ProcessDeleteRequest(strid, 5, 2, MessageTypes::Sell));
      result.foundAfterDeletion.push_back(!core.sellQueue.empty());
      Core::sellReq temp1;
      temp1.price = 5;
      temp1.timeStep = 0;
      temp1.userId = 0;
      temp1.volume = 2;
      core.sellQueue.insert(temp1);
      result.foundBeforeDeletion.push_back(!core.sellQueue.empty());
      result.reply.push_back(core.ProcessDeleteRequest(strid, 5, 2, MessageTypes::Sell));
      result.foundAfterDeletion.push_back(!core.sellQueue.empty());
      Core::buyReq temp2;
      temp2.price = 2;
      temp2.timeStep = 1;
      temp2.userId = 0;
      temp2.volume = 1;
      core.buyQueue.insert(temp2);
      result.foundBeforeDeletion.push_back(!core.buyQueue.empty());
      result.reply.push_back(core.ProcessDeleteRequest(strid, 2, 1, MessageTypes::Buy));
      result.foundAfterDeletion.push_back(!core.buyQueue.empty());
      return result;
    }
private:
    Core core;
};

TEST_F(CoreTest, RegisterNewUserWithAuthTest) {
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  std::string login = "0000_*(*$&$?///}{[TestUser00____";
  std::string password = "0000_*(*$&$?///}{[TestUser00____";
  RegisterNewUserWithAuthTestResult result = RegisterNewUserWithAuthTestFunction(name, login, password);
  ASSERT_EQ(result.id, result.reply[UserId]);
  ASSERT_EQ(result.password, password);
  ASSERT_EQ(result.reply[MsgType], MessageTypes::Registration);
  ASSERT_EQ(result.reply[UserId], "0");
}

TEST_F(CoreTest, RegisterNewUserTest) {
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  RegisterNewUserTestResult result = RegisterNewUserTestFunction(name);
  ASSERT_EQ(result.name1, name);
  ASSERT_EQ(result.name2, name);
  ASSERT_EQ(result.reply[MsgType], MessageTypes::Registration);
  ASSERT_EQ(result.reply[UserId], "0");
}

TEST_F(CoreTest, LoginWithPasswordTest) {
  std::string login = "01000_*(*$&$?///}{[TestUser00____";
  std::string password = "02000_*(*$&$?///}{[TestUser00____";
  LoginWithPasswordTestResult result = LoginWithPasswordTestFunction(login, password);
  ASSERT_EQ(result.id, result.reply[1][UserId]);
  ASSERT_EQ(result.password, password);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Error);
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::Authorization);
  ASSERT_EQ(result.reply[1][UserId], "5");
}

TEST_F(CoreTest, GetErrorTest) {
  std::string message = "Error. No error found.\n";
  GetErrorTestResult result = GetErrorTestFunction(message);
  ASSERT_EQ(result.reply[MsgType], MessageTypes::Error);
  ASSERT_EQ(result.reply[Message], message);
}

TEST_F(CoreTest, GetHelloTest) {
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  GetHelloTestResult result = GetHelloTestFunction(name);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Error);
  ASSERT_EQ(result.reply[0][Message], "Error : User doesn't exist.\n");
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::Hello);
  ASSERT_EQ(result.reply[1][Message], "Hello, " + name + "!\n");
}

TEST_F(CoreTest, GetActiveRequestsTest) {
  std::vector<std::pair<long long, long long>> vBuyRequests;
  std::vector<std::pair<long long, long long>> vSellRequests;
  vBuyRequests.push_back({6, 4});
  vSellRequests.push_back({7, 3});
  vSellRequests.push_back({8, 3});
  GetActiveRequestsTestResult result = GetActiveRequestsTestFunction(vBuyRequests, vSellRequests);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Error);
  ASSERT_EQ(result.reply[0][Message], "Error : User doesn't exist.\n");
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::ActiveRequests);
  ASSERT_EQ(result.reply[1][BuyRequests][0][0], 6);
  ASSERT_EQ(result.reply[1][BuyRequests][0][1], 4);
  ASSERT_EQ(result.reply[1][SellRequests][0][0], 7);
  ASSERT_EQ(result.reply[1][SellRequests][0][1], 3);
  ASSERT_EQ(result.reply[1][SellRequests][1][0], 8);
  ASSERT_EQ(result.reply[1][SellRequests][1][1], 3);
}


TEST_F(CoreTest, GetDealsTest) {
  std::vector<fakeLogType> myLogs;
  myLogs.push_back({2, 5, 0, 1});
  myLogs.push_back({2, 5, 1, 2});
  myLogs.push_back({2, 5, 2, 0});
  myLogs.push_back({50, 5, 2, 1});
  myLogs.push_back({2, 75, 0, 1});
  std::string names[4] = {"bob", "bot", "tot", "tob"};
  GetDealsTestResult result = GetDealsTestFunction(myLogs, names);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Deals);
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::Deals);
  ASSERT_EQ(result.reply[2][MsgType], MessageTypes::Deals);
  ASSERT_EQ(result.reply[3][MsgType], MessageTypes::Deals);
  ASSERT_EQ(result.reply[4][MsgType], MessageTypes::Error);


  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][0]).userName, names[1]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][1]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][1]).volume, 75);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][SellRequests][DealsList][1]).userName, names[1]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][BuyRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][BuyRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[0][BuyRequests][DealsList][0]).userName, names[2]);


  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][SellRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][SellRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][SellRequests][DealsList][0]).userName, names[2]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][0]).userName, names[0]);
  
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][1]).price, 50);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][1]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][1]).userName, names[2]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][2]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][2]).volume, 75);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[1][BuyRequests][DealsList][2]).userName, names[0]);
  

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][0]).userName, names[0]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][1]).price, 50);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][1]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][SellRequests][DealsList][1]).userName, names[1]);

  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][BuyRequests][DealsList][0]).price, 2);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][BuyRequests][DealsList][0]).volume, 5);
  ASSERT_EQ(static_cast<ns::Deal>(result.reply[2][BuyRequests][DealsList][0]).userName, names[1]);


  ASSERT_EQ(result.reply[3][BuyRequests][DealsList].size(), 0);
  ASSERT_EQ(result.reply[3][SellRequests][DealsList].size(), 0);

}

TEST_F(CoreTest, GetBalanceTest) {
  GetBalanceTestResult result = GetBalanceTestFunction(99999, 88888);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Error);
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::Balance);
  ASSERT_EQ(result.reply[1][Rubles], 0);
  ASSERT_EQ(result.reply[1][Dollars], 0);
  ASSERT_EQ(result.reply[2][MsgType], MessageTypes::Balance);
  ASSERT_EQ(result.reply[2][Rubles], 88888);
  ASSERT_EQ(result.reply[2][Dollars], 99999);
}

TEST_F(CoreTest, ProcessSellRequestTest) {
  std::vector<fakeRequest> vBuyQueue;
  vBuyQueue.push_back({4, 1, 4, 1});//price, timestep, volume, id.
  vBuyQueue.push_back({3, 0, 4, 1});
  vBuyQueue.push_back({3, 2, 4, 1});
  ProcessSellRequestTestResult result = ProcessSellRequestTestFunction(5, 2, vBuyQueue);
  ASSERT_EQ(result.reply[1][Rubles], 0);
  ASSERT_EQ(result.reply[1][Dollars], 0);
  ASSERT_EQ(result.BuyQueueState.size(), vBuyQueue.size());
  ASSERT_EQ(result.BuyQueueState[0].price, 4);
  ASSERT_EQ(result.BuyQueueState[0].volume, 4);
  ASSERT_EQ(result.BuyQueueState[1].price, 3);
  ASSERT_EQ(result.BuyQueueState[1].volume, 4);
  ASSERT_EQ(result.BuyQueueState[2].price, 3);
  ASSERT_EQ(result.BuyQueueState[2].volume, 4);
  ASSERT_EQ(result.SellQueueState.size(), 1);
  ASSERT_EQ(result.SellQueueState[0].price, 5);
  ASSERT_EQ(result.SellQueueState[0].volume, 2);

  result = ProcessSellRequestTestFunction(4, 2, vBuyQueue);
  ASSERT_EQ(result.reply[1][Rubles], 8);
  ASSERT_EQ(result.reply[1][Dollars], -2);
  ASSERT_EQ(result.BuyQueueState.size(), vBuyQueue.size());
  ASSERT_EQ(result.BuyQueueState[0].price, 4);
  ASSERT_EQ(result.BuyQueueState[0].volume, 2);
  ASSERT_EQ(result.BuyQueueState[1].price, 3);
  ASSERT_EQ(result.BuyQueueState[1].volume, 4);
  ASSERT_EQ(result.BuyQueueState[2].price, 3);
  ASSERT_EQ(result.BuyQueueState[2].volume, 4);
  ASSERT_EQ(result.SellQueueState.size(), 0);

  result = ProcessSellRequestTestFunction(3, 13, vBuyQueue);
  ASSERT_EQ(result.BuyQueueState.size(), 0);
  ASSERT_EQ(result.SellQueueState.size(), 1);
  ASSERT_EQ(result.SellQueueState[0].price, 3);
  ASSERT_EQ(result.SellQueueState[0].volume, 1);
  ASSERT_EQ(result.reply[1][Rubles], 40);
  ASSERT_EQ(result.reply[1][Dollars], -12);
}

TEST_F(CoreTest, ProcessBuyRequestTest) {
  std::vector<fakeRequest> vSellQueue;
  vSellQueue.push_back({4, 1, 4, 1});//price, timestep, volume, id.
  vSellQueue.push_back({3, 0, 4, 1});
  vSellQueue.push_back({3, 2, 4, 1});
  ProcessBuyRequestTestResult result = ProcessBuyRequestTestFunction(1, 2, vSellQueue);
  ASSERT_EQ(result.reply[1][Rubles], 0);
  ASSERT_EQ(result.reply[1][Dollars], 0);
  ASSERT_EQ(result.SellQueueState.size(), vSellQueue.size());
  ASSERT_EQ(result.SellQueueState[0].price, 3);
  ASSERT_EQ(result.SellQueueState[0].volume, 4);
  ASSERT_EQ(result.SellQueueState[1].price, 3);
  ASSERT_EQ(result.SellQueueState[1].volume, 4);
  ASSERT_EQ(result.SellQueueState[2].price, 4);
  ASSERT_EQ(result.SellQueueState[2].volume, 4);
  ASSERT_EQ(result.BuyQueueState.size(), 1);
  ASSERT_EQ(result.BuyQueueState[0].price, 1);
  ASSERT_EQ(result.BuyQueueState[0].volume, 2);

  result = ProcessBuyRequestTestFunction(3, 2, vSellQueue);
  ASSERT_EQ(result.reply[1][Rubles], -6);
  ASSERT_EQ(result.reply[1][Dollars], 2);
  ASSERT_EQ(result.SellQueueState.size(), vSellQueue.size());
  ASSERT_EQ(result.SellQueueState[0].price, 3);
  ASSERT_EQ(result.SellQueueState[0].volume, 2);
  ASSERT_EQ(result.SellQueueState[1].price, 3);
  ASSERT_EQ(result.SellQueueState[1].volume, 4);
  ASSERT_EQ(result.SellQueueState[2].price, 4);
  ASSERT_EQ(result.SellQueueState[2].volume, 4);
  ASSERT_EQ(result.BuyQueueState.size(), 0);

  result = ProcessBuyRequestTestFunction(5, 13, vSellQueue);
  ASSERT_EQ(result.SellQueueState.size(), 0);
  ASSERT_EQ(result.BuyQueueState.size(), 1);
  ASSERT_EQ(result.BuyQueueState[0].price, 5);
  ASSERT_EQ(result.BuyQueueState[0].volume, 1);
  ASSERT_EQ(result.reply[1][Rubles], -40);
  ASSERT_EQ(result.reply[1][Dollars], 12);
}

TEST_F(CoreTest, ProcessQuotationsRequestTest) {
  ProcessQuotationsRequestTestResult result = ProcessQuotationsRequestTestFunction();
  
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Quotations);
  ASSERT_EQ(result.reply[0][SellRequests][Price], -1);
  ASSERT_EQ(result.reply[0][BuyRequests][Price], -1);
  ASSERT_EQ(result.reply[1][SellRequests][Price], 5);
  ASSERT_EQ(result.reply[1][BuyRequests][Price], -1);
  ASSERT_EQ(result.reply[2][SellRequests][Price], 5);
  ASSERT_EQ(result.reply[2][BuyRequests][Price], 2);
}

TEST_F(CoreTest, ProcessDeleteRequestTest) {
  ProcessDeleteRequestTestResult result = ProcessDeleteRequestTestFunction();
  
  ASSERT_EQ(result.foundBeforeDeletion[0], false);
  ASSERT_EQ(result.reply[0][MsgType], MessageTypes::Error);
  ASSERT_EQ(result.foundAfterDeletion[0], false);

  ASSERT_EQ(result.foundBeforeDeletion[1], true);
  ASSERT_EQ(result.reply[1][MsgType], MessageTypes::Delete);
  ASSERT_EQ(result.foundAfterDeletion[1], false);

  ASSERT_EQ(result.foundBeforeDeletion[2], true);
  ASSERT_EQ(result.reply[2][MsgType], MessageTypes::Delete);
  ASSERT_EQ(result.foundAfterDeletion[2], false);
}