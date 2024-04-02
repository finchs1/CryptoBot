#include <dpp/dpp.h>
#include <dpp/fmt/format.h>

class coin_mgr;

class bot_mgr
{
    public:
        bot_mgr();
        void setup_cmds();
        void gas(const dpp::interaction_create_t& e, coin_mgr& mgr);
        void coin(const dpp::interaction_create_t& e, const std::string& coin_name, coin_mgr& mgr);
    private:
        bool setup = false;
};