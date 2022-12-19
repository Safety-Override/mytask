Для сборки необходимо использовать cmake от версии 3.21.

Необходимы библиотеки boost::asio и nlohmann::json.

Подразумевается только локальный запуск итоговых программ. Интерфейс выполнен на английском языке.

После компиляции необходимо запустить сначала файл Server, затем Client.

В интерфейсе консоли реализованы следующие опции: 
"Registration" регистрация клиента у сервера при запуске Client, либо "LoginWithPassword" вариант входа в зарегистрированный аккаунт. "Hello Request" запрос на получение строки формата "Hello $username" от сервера. "Active Requests" запрос всех активных заявок пользователя. "All deals" запрос всех сделок пользователя. "Balance" запрос баланса. "Create Sell request" создание заявки на продажу долларов за рубли. "Create Buy request" создание заявки на покупку долларов за рубли. "Quotations" - получение котировок. "Delete request" - удаление запросов клиента."Exit" выход из программы.

После компиляции есть возможность запустить тесты находящиеся в подпапке tests. Файл test.