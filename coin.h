#include <string>
#include <curl/curl.h>
#include <cstring>
#include <iostream>
//#include "simdjson.h"
#include <dpp/nlohmann/json.hpp>
//#include <string_view>

const std::string price_url = "https://api.coingecko.com/api/v3/simple/price?ids=";
const std::string gas_url = "https://ethgasstation.info/api/ethgasAPI.json?api-key=######################################";//Add API Key for ethgasstation
CURL* handle;
curl_slist* list = nullptr;

//simdjson::ondemand::parser parser;
//simdjson::padded_string coin_json;
//simdjson::ondemand::document coin_list;
json coin_json;

char data[100000];
int64_t pos = 0;

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
    memcpy(reinterpret_cast<void*>((uintptr_t)data + pos), buffer, nmemb);
    pos += nmemb;
    return nmemb;
}

auto req_data(const std::string& url)
{
    memset(data, 0, sizeof data);
    pos = 0;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

    return curl_easy_perform(handle);
}

class coin
{
    public:

        coin(const std::string& t) : ticker(t)
        {
            /*
            coin_list.rewind();
            for (simdjson::ondemand::object c : coin_list)
            {
                if (t == c["symbol"])
                {
                    name = c["name"].get_string().value().data();
                    id = c["id"].get_string().value().data();
                    break;
                }
            }
            */

            for (auto& c : coin_json)
            {
                if (t == c["symbol"].get<std::string>())
                {
                    name = c["name"].get<std::string>();
                    id = c["id"].get<std::string>();
                    return;
                }
            }

            name = "INVALID";
        }

        void update()
        {
            if (get_name() == "INVALID") return;
            auto curl_code = req_data(price_url + id + "&vs_currencies=usd");

            if (!std::string(data).empty())
            {
                json j = json::parse(data);

                if (curl_code == CURLE_OK && j.dump() != "{}")
                {
                    price = j[id]["usd"].get<float>();
                    fee = 0;
                }
                else
                {
                    name = "INVALID";
                    price = 0;
                    fee = 0;

                    return;
                }
            }

            //curl_slist_free_all(list);
            //curl_easy_setopt(handle, CURLOPT_HTTPHEADER, 0);

            if (ticker == "eth")
            {
                auto curl_code2 = req_data(gas_url);

                if (!std::string(data).empty())
                {
                    json j = json::parse(data);

                    if (curl_code == CURLE_OK && j.dump() != "{}")
                        fee = j["safeLow"].get<float>() / 10;
                }
            }

            //list = nullptr;
        }

        const std::string& get_ticker()
        {
            return ticker;
        }

        const std::string& get_name()
        {
            return name;
        }

        float get_price()
        {
            return price;
        }

        float get_fee()
        {
            return fee;
        }

    private:
        std::string ticker;
        std::string name;
        std::string id;
        float price;
        float fee;
};
