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

// #include <VulpineFastParser.hpp>

// int main()
// {
//     VulpineTextBuff buff("../notes/loaderTest.txt");

//     // Loader2<int, int, int> li;

//     // Loader<int>::loadedAssets["yo"] = 5;
//     // Loader<int>::loadedAssets["yoo"] = 6;
//     // Loader<int>::loadedAssets["yooo"] = 7;

//     // Loader2<int, int, int>().addInfos("yo", buff);
//     // Loader2<int, int>().addInfos("yoo", buff);
//     // Loader2<int>().addInfos("yooo", buff);

//     // std::cout << li.loadedAssets.size() << "\n";
//     // std::cout << li.loadingInfos.size() << "\n";

     


//     return EXIT_SUCCESS;
// }