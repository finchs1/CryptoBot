#include "coin.h"
#include <vector>
#include <fstream>

class coin_mgr
{
    public:
        coin_mgr()
        {
            auto curl_code = curl_global_init(CURL_GLOBAL_ALL);
            if (curl_code != 0)
            {
                std::cout << "CURL has failed initialization";
                exit(1);
            }
            handle = curl_easy_init();
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Fedora; Linux x86_64; rv:99.0) Gecko/20100101 Firefox/99.0");
            curl_easy_setopt(handle, CURLOPT_ACCEPT_ENCODING, "");

            //coin_json = simdjson::padded_string::load("list.json");
            //coin_list = parser.iterate(coin_json);

            std::ifstream coin_json_file("list.json");

            file_buf << coin_json_file.rdbuf();

            coin_json_file.close();

            coin_json = json::parse(file_buf.str());
        }

        bool create_coin(const std::string& t)
        {
            if (does_coin_exist(t)) return true;

            coin new_coin = coin(t);
            new_coin.update();
            
            if (new_coin.get_name() == "INVALID")
                return false;

            if (coins.size() == 50)
                coins.pop_back();

            coins.push_back(new_coin);

            return true;
        }

        void update_all_coins()
        {
            for (auto& c : coins)
                c.update();
        }

        void update_coin(const std::string& ticker)
        {
            for (auto& c : coins)
                if (!c.get_ticker().compare(ticker))
                    c.update();
        }

        float get_price(const std::string& ticker)
        {
            for (auto& c : coins)
                if (!c.get_ticker().compare(ticker))
                    return c.get_price();

            return 0;
        }

        float get_fee(const std::string& ticker)
        {
            for (auto& c : coins)
                if (!c.get_ticker().compare(ticker))
                    return c.get_fee();

            return 0;
        }

        bool does_coin_exist(const std::string& ticker)
        {
            for (auto& c : coins)
                if (c.get_ticker() == ticker)
                    return true;

            return false;
        }

        void tick()
        {
            update_all_coins();
            /*auto cur_time = time(nullptr);

            
            if (cur_time > time_to_update)
            {
                update_all_coins();
                time_to_update = cur_time + update_time;
            }
            */
        }

    private:
        std::vector<coin> coins;
        //time_t time_to_update;
        std::stringstream file_buf;
        //const int update_time = 600;
};