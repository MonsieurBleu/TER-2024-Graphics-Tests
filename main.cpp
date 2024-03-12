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

int main()
{
    Game *game = nullptr;
    std::string winname =  "Vulpine Engine Game Demo";
    int ret = launchGame(&game, winname, 5);
    if(game) delete game;
    return ret; 
}

#include <VulpineFastParser.hpp>

int main_old()
{
    // VulpineTextBuff test("../notes/Example.vulpineModel.txt");
    // VulpineTextBuff test("../notes/loaderTest.txt");

    // uft8 *tmp = nullptr;

    // BenchTimer bench;
    // bench.start();
    // do
    // {
    //     tmp = test.read();
    //     if(tmp)
    //     std::cout << tmp << "\n==================================\n";
    // }
    // while(tmp);S
    // bench.end();

    // std::cout << bench << "\n";

    // char str[] = "189";
    // std::cout << fromStr<int>(str) << "\n"; 

    // loader<int, int> l; 

    // std::cout << l.create(test) << "\n";
    // std::cout << l.create(test) << "\n";
    // std::cout << l.create(test) << "\n";
    // std::cout << l.create(test) << "\n";
    // std::cout << l.create(test) << "\n";

    // loader<ShaderProgram, std::string, std::string> l;
    // l.create();

    return EXIT_SUCCESS;
}