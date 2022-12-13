#include <gtest/gtest.h>
#include "../ServerFunctions.cpp"
#include "../ClientFunctions.cpp"

// Demonstrate some basic assertions.
TEST(HelloTest, ClientFunctionsBasicChecks) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);

}


TEST(RegistrationRequestTest, ClientFunctionsBasicChecks) {
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  nlohmann::json j = RegistrationRequest(name);
  ASSERT_EQ(j[Message], "0000_*(*$&$?///}{[TestUser00____");
  ASSERT_EQ(j[MsgType], MessageTypes::Registration);
}

TEST(HelloRequestTest, ClientFunctionsBasicChecks) {
nlohmann::json j = HelloRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Hello);
}

TEST(ActiveRequestsRequestTest, ClientFunctionsBasicChecks) {
nlohmann::json j = ActiveRequestsRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::ActiveRequests);
}

TEST(DealsRequestTest, ClientFunctionsBasicChecks) {
nlohmann::json j = DealsRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Deals);
}

TEST(BalanceRequestTest, ClientFunctionsBasicChecks) {
  nlohmann::json j = BalanceRequest();
  ASSERT_EQ(j[MsgType], MessageTypes::Balance);
}

TEST(SellRequestTest, ClientFunctionsBasicChecks) {
  nlohmann::json j = SellRequest(7, 3);
  ASSERT_EQ(j[MsgType], MessageTypes::Sell);
  ASSERT_EQ(j[Price], 7);
  ASSERT_EQ(j[Volume], 3);
}

TEST(BuyRequestTest, ClientFunctionsBasicChecks) {
  nlohmann::json j = BuyRequest(7, 3);
  ASSERT_EQ(j[MsgType], MessageTypes::Buy);
  ASSERT_EQ(j[Price], 7);
  ASSERT_EQ(j[Volume], 3);
}

TEST(ErrorResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json j;
  std::string errorMsg = "Custom Error.\n";
  j[Message] = "Custom Error.\n";
  ASSERT_EQ(ErrorResponse(j), errorMsg);
}

TEST(RegistrationResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json j;
  j[Message] = std::to_string(0);
  std::string rep = "0";
  ASSERT_EQ(RegistrationResponse(j), rep);
}

TEST(HelloResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json j;
  std::string name = "0000_*(*$&$?///}{[TestUser00____";
  j[Message] = "Hello, " + name + "\n";
  ASSERT_EQ(HelloResponse(j), "Hello, " + name + "\n");
}

TEST(ActiveRequestsResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json rep;
  std::vector<std::pair<long long, long long>> vSellRequests;
  std::vector<std::pair<long long, long long>> vBuyRequests;
  for (long long i = 0; i < 3; ++i) {
          vSellRequests.push_back({i + 1, i + 2});
  }
  for  (long long i = 0; i < 3; ++i) {
          vBuyRequests.push_back({i + 1, i + 3});
  }
  rep[SellRequests] = vSellRequests;
  rep[BuyRequests] = vBuyRequests;
  std::string ans = "Your Sell Requests :\nPrice: 1 Volume: 2\nPrice: 2 Volume: 3\nPrice: 3 Volume: 4\nYour Buy Requests :\nPrice: 1 Volume: 3\nPrice: 2 Volume: 4\nPrice: 3 Volume: 5\n";
  ASSERT_EQ(ActiveRequestsResponse(rep), ans);
}

TEST(DealsResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json rep;
  std::vector<ns::Deal> vDealsList;
  for (long long i = 0; i < 2; ++i) {
    vDealsList.push_back({i + 2, i + 7, "TestUser_" + std::to_string(i + 4)});
  }
  rep[SellRequests][DealsList] = vDealsList;
  vDealsList.clear();
  for (long long i = 0; i < 2; ++i) {
    vDealsList.push_back({i + 4, i + 9, "TestUser_" + std::to_string(i + 15)});
  }
  rep[BuyRequests][DealsList] = vDealsList;

  std::string ans = "Your Deals :\nYou sold to TestUser_4. Price: 2 Volume: 7\nYou sold to TestUser_5. Price: 3 Volume: 8\nYou bought from TestUser_15. Price: 4 Volume: 9\nYou bought from TestUser_16. Price: 5 Volume: 10\n";
  ASSERT_EQ(DealsResponse(rep), ans);
}

TEST(BalanceResponseTest, ClientFunctionsBasicChecks) {
  nlohmann::json rep;
  rep[MsgType] = MessageTypes::Balance;
  rep[Dollars] = profiles[id].dollars;
  rep[Rubles] = profiles[id].rubles;
  
  std::string ans = "Your Deals :\nYou sold to TestUser_4. Price: 2 Volume: 7\nYou sold to TestUser_5. Price: 3 Volume: 8\nYou bought from TestUser_15. Price: 4 Volume: 9\nYou bought from TestUser_16. Price: 5 Volume: 10\n";
  ASSERT_EQ(DealsResponse(rep), ans);
}

//////////////////////////////////////////////////////////

TEST(UserExistsTest, ServerFunctionsBasicChecks) {

}

TEST(RegisterNewUserTest, ServerFunctionsBasicChecks) {

}

TEST(GetHelloTest, ServerFunctionsBasicChecks) {

}

TEST(GetActiveRequestsTest, ServerFunctionsBasicChecks) {

}

TEST(GetDealsTest, ServerFunctionsBasicChecks) {

}

TEST(GetBalanceTest, ServerFunctionsBasicChecks) {

}

TEST(ProcessSellRequestTest, ServerFunctionsBasicChecks) {

}

TEST(ProcessBuyRequestTest, ServerFunctionsBasicChecks) {

}
