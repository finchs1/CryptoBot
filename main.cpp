#include "bot_mgr.h"

/*
void g(dpp::confirmation_callback_t d)
{
    for (auto& b : std::get<dpp::slashcommand_map>(d.value))
        std::cout << b.first << '\n';
}
*/

int main()
{
    bot_mgr instance = bot_mgr();
 
    return 0;
}