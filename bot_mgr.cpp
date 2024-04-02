#include "bot_mgr.h"
#include "coin_mgr.h"

dpp::cluster bot("****************");//replace with discord bot token

bot_mgr::bot_mgr()
{
    bot.on_log(dpp::utility::cout_logger());
    auto mgr = coin_mgr();
    
    /* The interaction create event is fired when someone issues your commands */
    bot.on_interaction_create([this, &mgr](const dpp::interaction_create_t & event)
    {
        auto cmd_name = event.command.get_command_name();

        if (!cmd_name.compare("gas")) this->gas(event, mgr);

        else 
        {
            if (cmd_name == std::string("price"))
                cmd_name = std::get<std::string>(event.get_parameter("crypto"));

            std::transform(cmd_name.begin(), cmd_name.end(), cmd_name.begin(), toupper);
            this->coin(event, cmd_name, mgr);
        }
    });
 
    bot.on_ready([this, &mgr](const dpp::ready_t & event) {
        if (dpp::run_once<struct register_bot_commands>())
        {
            const uint64_t time_to_tick = 300;

            dpp::timer_callback_t callback = [&mgr](dpp::timer)
            {
                mgr.tick();
            };

            if (!setup)
                bot.start_timer(callback, time_to_tick);

            this->setup_cmds();
        }
    });

    bot.start(false);
}

void bot_mgr::setup_cmds()
{
    if (!setup)
    {
        /* Create a new global command on ready event */
        dpp::slashcommand btc("btc", "Get the current Bitcoin price", bot.me.id);
        dpp::slashcommand eth("eth", "Get the current Ethereum price", bot.me.id);
        dpp::slashcommand gas("gas", "Get the current Ethereum gas price", bot.me.id);
        dpp::slashcommand shib("shib", "Get the current Shiba Inu price", bot.me.id);
        dpp::slashcommand price("price", "Get the current price of a cryptocurrency", bot.me.id);

        price.add_option(dpp::command_option(dpp::co_string, "crypto", "Cryptocurrency", true));

        /* Register the command */
        //bot.global_command_create(btc);//Only need to register once
        //bot.global_command_create(eth);
        //bot.global_command_create(gas);
        //bot.global_command_create(shib);
        //bot.global_command_create(price);

        setup = true;
    }

}

void bot_mgr::gas(const dpp::interaction_create_t& e, coin_mgr& mgr)
{   
    auto eth_ticker = std::string("eth");

    if (!mgr.create_coin(eth_ticker)) 
        e.reply("Error getting gas");

    auto fee = mgr.get_fee(eth_ticker);
    auto price = mgr.get_price(eth_ticker);

    e.reply(fmt::format(std::locale("en_US.UTF-8"), "Low gas price = {} gwei = ${:.2Lf}", fee, fee * 0.000021 * price));
}

void bot_mgr::coin(const dpp::interaction_create_t& e, const std::string& coin_ticker, coin_mgr& mgr)
{
    auto ticker_lower = coin_ticker;
    std::transform(coin_ticker.begin(), coin_ticker.end(), ticker_lower.begin(), tolower);

    auto coin_valid = mgr.create_coin(ticker_lower);
    if (!coin_valid) e.reply("Coin is invalid");

    else
    {
        auto price = mgr.get_price(ticker_lower);
        std::string fmt_string;
        if (price < 0.1f)
            fmt_string = fmt::format(std::locale("en_US.UTF-8"), "1 {} = ${:.9Lf}", coin_ticker, price);
        else
            fmt_string = fmt::format(std::locale("en_US.UTF-8"), "1 {} = ${:.2Lf}", coin_ticker, price);

        e.reply(fmt_string);
    }

}
