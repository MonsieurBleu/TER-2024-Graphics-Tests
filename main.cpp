#include <Launcher.hpp>
#include <Game.hpp>

/**
 * To be executed by the launcher, the Game class needs :
 * 
 *  - Constructor of type (void)[GLFWwindow*].
 * 
 *  - init method of type (any)[params ...] with 
 *    launchgame call of type (**Game, string, params).
 * 
 *  - mainloop method of type (any)[void].
 */

int main_old()
{
    Game *game = nullptr;
    std::string winname =  "Vulpine Engine Game Demo";
    int ret = launchGame(&game, winname, 5);
    if(game) delete game;
    return ret; 
}

#include <VulpineFastParser.hpp>

int main()
{
    VulpineTextBuff test("../notes/Example.vulpineModel.txt");
    // VulpineTextBuff test("../notes/ParserBenchmark.txt");

    // std::cout 
    // << test.data 
    // << "\n====================END OF FILE====================\n" 
    // << test.getSize() << "\n";

    uft8 *tmp = nullptr;

    BenchTimer bench;
    bench.start();
    do
    {
        tmp = test.read();
        if(tmp)
        std::cout << tmp << "\n==================================\n";
    }
    while(tmp);
    bench.end();
    // std::cout << bench << "\n";
}