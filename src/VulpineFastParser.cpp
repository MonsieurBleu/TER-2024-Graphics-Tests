#include <VulpineFastParser.hpp>

#include <limits>

#include <Utils.hpp>

VulpineTextBuff::VulpineTextBuff(const char *filename)
{
    loadFromFile(filename);
}

VulpineTextBuff::~VulpineTextBuff()
{
    if(data) delete [] data;
}

void VulpineTextBuff::loadFromFile(const char *filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    if(!file.good())
    {
        std::cerr 
        << TERMINAL_ERROR    << "Error : VulpineTextBuff::loadFromFile can't open file "
        << TERMINAL_FILENAME << filename 
        << TERMINAL_ERROR    << "\n\tThe buffer will be empty.\n"
        << TERMINAL_RESET ;
        return;
    }

    /* 
        Get the full file size from fstream.
            see : https://stackoverflow.com/a/37808094
        
        TODO : add this to an utility function
    */
    file.ignore( std::numeric_limits<std::streamsize>::max() );
    size = file.gcount();
    file.clear();
    file.seekg( 0, std::ios_base::beg );

    data = new uft8[size+1];
    file.read((char *)data, size);
    file.close();
    data[size] = '\0';
}

bool VulpineTextBuff::readBreakChar()
{
    if(readhead >= size)
    {
        eof = true;
        return false;
    } 
    
    if(stringMode)
        return !(stringMode = data[readhead] != '"');

    if(commentsMode)
    {
        commentsMode = data[readhead] != '*';
        return true;
    }

    switch(data[readhead])
    {
        case '\0' : 
        case '\n' : 
        case '\t' : 
        case ' ' :
        case 13 :
            return true;
        break;

        case '*' :
            commentsMode = true;
            return true;
        break;

        case '"' :
            stringMode = true;
            return true;
        break;

        default :
            return false;
        break;
    }
}

uft8* VulpineTextBuff::read()
{
    
    while(readBreakChar()) readhead++;

    if(eof) return nullptr;
    
    uft8 *beg = data + readhead;

    while(!readBreakChar() && !eof)
        readhead++;

    data[readhead] = '\0';

    return beg;
}


template<typename T>
T fromStr(const char * ptr);

template<>
int fromStr(const char *ptr)
{
    return atoi(ptr);
}

template<>
float fromStr(const char *ptr)
{
    return atof(ptr);
}

template<>
const char* fromStr(const char *ptr)
{
    return ptr;
}

template<>
std::string fromStr(const char *ptr)
{
    return std::string(ptr);
}