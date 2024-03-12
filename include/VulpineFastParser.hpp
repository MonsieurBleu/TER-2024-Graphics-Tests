#pragma once 

#include <memory>
#include <fstream>

typedef char uft8;

/*
    Simple buffer to load a full uft8 file and parse Vulpine text based formats
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

#include <string.h>

template<typename T>
T fromStr(const char * ptr);

template<>
int fromStr(const char *ptr);

template<>
float fromStr(const char *ptr);

template<>
const char* fromStr(const char *ptr);

template<>
std::string fromStr(const char *ptr);

#include <memory>

template<typename T, typename... params>
class loader
{
    private : 

    public :

        T val;
        T create(VulpineTextBuff &buff)
        {
            val = T(fromStr<params>(buff.read())...);
            return val;
        };

};
