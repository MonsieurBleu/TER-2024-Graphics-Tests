#pragma once 

#include <memory>
#include <fstream>

typedef unsigned char uft8;

/*
    Simple buffer to load a full uft8 file and parse Vulpine Format text based formats
*/
class VulpineTextBuff
{
    private :

        size_t size = 0;
        size_t readhead = 0;

        bool eof = false;
        bool stringMode = false;
        bool commentsMode = false;

        bool readBreakChar();

    public :
        VulpineTextBuff(const char *filename);
        ~VulpineTextBuff();

        void loadFromFile(const char *filename);
        uft8 *data = nullptr;

        size_t getSize(){return size;};
        size_t getReadHead(){return size;};

        uft8* read();
};

