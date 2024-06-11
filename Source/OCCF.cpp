#include "OCCF.hpp"

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) UTIL (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

std::string trim(const std::string& line)
{
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = line.find_first_not_of(WhiteSpace);
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(start, end - start + 1);
}
#define PARSING_ERROR(line_num,line)("\nERROR : Parsing Failure\nLINE : "+std::to_string(line_num)+" , \""+trim(line)+"\"\nWHAT : ")

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) OCCF (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

// Misc.
OCCF::~OCCF(){for(auto& pair : Data){delete pair.second;}}
OCCF::OCCF(){}

OCCF::_CONTAINER& OCCF::operator[] (const std::string Provision)
{
    if (Data.find(Provision) == Data.end()) 
    {Data[Provision] = new OCCF::_CONTAINER;}
    return *Data[Provision];
}
OCCF::_CONTAINER& OCCF::operator[] (const char provision)
{
    std::string Provision(1,provision);
    if (Data.find(Provision) == Data.end()) 
    {Data[Provision] = new OCCF::_CONTAINER;}
    return *Data[Provision];
}
OCCF::_CONTAINER& OCCF::operator[] (const int _index)
{
    if (_index < 0){throw OCCF_ERROR("\nERROR : Index failure\nWHAT : Unable to index negative values");}
    std::string index = std::to_string(_index);
    if (Data.find(index) == Data.end()) 
    {Data[index] = new OCCF::_CONTAINER;}
    return *Data[index];
}

std::map<std::string,OCCF::_CONTAINER*>::const_iterator OCCF::find(std::string key)const{return Data.find(key);}
std::map<std::string,OCCF::_CONTAINER*>::const_iterator OCCF::begin()const{return Data.begin();}
std::map<std::string,OCCF::_CONTAINER*>::const_iterator OCCF::end()const{return Data.end();}

void OCCF::erase(const char* Key){this->Data.erase(Key);}

// Parse Helpers
std::ifstream OCCF::_getFile(const char* PathToFile)
{
    std::ifstream File(PathToFile);
    {
        if (!File.is_open())
        {throw OCCF_ERROR("\nERROR : File Failure\nFILE : "+std::string(PathToFile)+"\nWHAT : File failed to open");}
    }
    return File;
}
void OCCF::_parse(std::ifstream& File)
{
    this->Data.clear();

    enum class States
    {
        //Character States
        STANDBY,
        CONTAINER_START,
        CONTAINER_END,
        KEY_START,
        VALUE_STANDBY, 
        VALUE_START,

        COMMENT_START,
        COMMENT_MIDDLE,
        COMMENT_LINE,

        COMMENT_BLOCK,
        COMMENT_BLOCK_MIDDLE,
        COMMENT_BLOCK_END,

        //Section States
        CONTAINER_SEARCH,
        KEY_SEARCH,
        VALUE_SEARCH,
    };

    States SectionState = States::CONTAINER_SEARCH;
    States CachedState; // Might not be needed 

    std::string BankContainer,BankKey,BankValue;

    bool CommentBlock = false;
    int LineNumber = 0;

    while (File)
    {
        std::string Line;
        getline(File,Line);

        States CharState = (CommentBlock) ? States::COMMENT_BLOCK : States::STANDBY;
        
        bool IsString;
        bool IsInt = true;
        
        bool IsBool = false;
        bool SearchBool;
        int IndexBool;
        const unsigned int LineLength = Line.length();
        unsigned int CharNumber = 0;

        LineNumber++;
        int BankedLineNumber;

        for (const char& c : Line)
        {
            CharNumber++;
            switch (CharState)
            {
                case States::STANDBY:
                     if (c == '.'){CharState = States::COMMENT_START;}
                else if (c == '-' && SectionState == States::CONTAINER_SEARCH){CharState = States::CONTAINER_START;}
                else if (c == '?' && SectionState == States::KEY_SEARCH){CharState = States::KEY_START;}
                else if (c == '<' && SectionState == States::KEY_SEARCH){CharState = States::CONTAINER_END;}
                else if (std::string(" \t\v\r\n").find(c) == std::string::npos){throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "General syntax error"));}
                    break;

                case States::CONTAINER_START:
                    if (c == '>')
                    {
                        if (BankContainer.length() == 0)
                        {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Cannot have empty container Names"));}
                        else if (this->Data.find(BankContainer) != this->Data.end())
                        {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Cannot have duplicate container Names"));}

                        SectionState = States::KEY_SEARCH;
                        CharState = States::STANDBY;
                    }
                    else
                    {BankContainer += c;}
                    break;

                case States::KEY_START:
                    if (c == '?')
                    {
                        if (BankKey.length() == 0)
                        {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Cannot have empty key names"));}
                        else if ((*this)[BankContainer].find(BankKey) != (*this)[BankContainer].end())
                        {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Cannot have duplicate key names"));}

                        CharState = States::VALUE_STANDBY;
                        SectionState = States::VALUE_SEARCH;
                    }
                    else if (c != '#')
                    {BankKey += c;}
                    else 
                    {throw OCCF::OCCF_ERROR("Error : Don't Do That");}
                    break;

                case States::VALUE_STANDBY:
                    if ((c == '!' || c == '#') && SectionState == States::VALUE_SEARCH)
                    {
                        IsString = (c == '#') ? false : true;
                        CharState = States::VALUE_START;
                    }
                    else if (c == 't' || c == 'f')
                    {
                        BankedLineNumber = LineNumber;
                        SearchBool = (c == 't') ? true : false;
                        IsBool = true;
                        IndexBool = 1;
                        CharState = States::VALUE_START;
                    }
                    else if (c == '.')
                    {
                        CachedState = CharState;
                        CharState = States::COMMENT_START;
                    }
                    else if (!(c == ' '|| c == '!' || c == '#'))
                    {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Invalid value declaration"));}
                    break;

                case States::VALUE_START:
                    if ((c == '!' && IsString) || (c == '#' && !IsString))
                    {
                        if (IsString){(*this)[BankContainer][BankKey] = BankValue;}
                        else if (IsInt){(*this)[BankContainer][BankKey] = int(std::stoi(BankValue));}
                        else {(*this)[BankContainer][BankKey] = double(std::stod(BankValue));}

                        BankValue = "";
                        BankKey = "";
                        CharState = States::STANDBY;
                        SectionState = States::KEY_SEARCH;

                        IsInt = true;
                    }
                    else if ((c == '!' && !IsString) || (c == '#' && IsString))
                    {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Invalid value declaration"));}
                    else if (IsBool)
                    {
                        
                        std::string Check = (SearchBool) ? "true" : "false";
                        bool CheckStatus = Check[IndexBool] == c;
                        bool EndOfLine = LineLength == CharNumber;

                        if ((CheckStatus && !EndOfLine) || (EndOfLine && c == 'e') ){IndexBool++;}
                        else{throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "bool \"" + Check + "\" failed to parse (Check Spelling)"));}

                        if (SearchBool && IndexBool == 4)
                        {
                            (*this)[BankContainer][BankKey] = true;
                            CharState = States::STANDBY;
                            SectionState = States::KEY_SEARCH;
                            IsBool = false;
                            BankKey = "";
                        }
                        else if (!SearchBool && IndexBool == 5)
                        {
                            (*this)[BankContainer][BankKey] = false;
                            CharState = States::STANDBY;
                            SectionState = States::KEY_SEARCH;
                            IsBool = false;
                            BankKey = "";
                        }
                    }
                    else
                    {
                        if (!IsString)
                        {
                            if (!(isdigit(c) || c =='.')){throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Number values cannot contain letters or symbols"));}
                            if (c == '.' && !IsInt){throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Invalid double"));}
                            else if (c == '.'){IsInt = false;}
                        }
                        BankValue += c;
                    }
                    break;

                case States::CONTAINER_END:
                    if (c == '-')
                    {
                        SectionState = States::CONTAINER_SEARCH;
                        CharState = States::STANDBY;

                        BankContainer = "";
                    }
                    else if (c != '-')
                    {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Container was not closed correctly"));}
                    else
                    {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "idk bro"));}
                    break;

                case States::COMMENT_START:
                    if (c == '.'){CharState = States::COMMENT_MIDDLE;}
                    else {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Comment Line Improperly Declared (...)"));}
                    break;

                case States::COMMENT_MIDDLE:
                    if (c == '.'){CharState = States::COMMENT_LINE;}
                    else if (c =='/')
                    {
                        CommentBlock = true;
                        CharState = States::COMMENT_BLOCK;
                    }
                    else {throw OCCF::OCCF_ERROR(std::string(PARSING_ERROR(LineNumber,Line) + "Comment Line Improperly Declared (...)"));}
                    break;

                case States::COMMENT_LINE:
                    // Do Nothing
                    break;

                case States::COMMENT_BLOCK:
                    if (c == '\\'){CharState = States::COMMENT_BLOCK_MIDDLE;}
                    break;

                case States::COMMENT_BLOCK_MIDDLE:
                    if (c == '.'){CharState = States::COMMENT_BLOCK_END;}
                    else {CharState = States::COMMENT_BLOCK;}
                    break;

                case States::COMMENT_BLOCK_END:
                    if (c == '.')
                    {
                        CommentBlock = false;
                        CharState = States::STANDBY;
                    }
                    else {CharState = States::COMMENT_BLOCK;}
                    break;

                default:
                    break;
            }
        }
    }

    if (SectionState != States::CONTAINER_SEARCH)
    {throw OCCF::OCCF_ERROR(std::string("ERROR : Parsing Failure\nWHAT : Container not Closed properly"));}

};

// Parse Methods
OCCF::OCCF(const char* PathToFile) // Constructor
{
    std::ifstream File = _getFile(PathToFile);
    _parse(File);
}
void OCCF::parse(const char* PathToFile) // Member Function
{
    std::ifstream File = _getFile(PathToFile);
    _parse(File);
}
void OCCF::parse(std::ifstream& File)
{
    this->_parse(File);
}
std::ifstream& operator>> (std::ifstream& File,OCCF& _) // Extraction Overload
{
    _._parse(File);
    return File;
}

// Output Data To Text File
void OCCF::dump(const char* PathToFile)
{
    std::ofstream File(PathToFile);
    if (!File.is_open())   
    {throw OCCF_ERROR("\nERROR : File Failure\nFILE : "+std::string(PathToFile)+"\nWHAT : File failed to open");}
    
    bool FirstLine = true;
    for (const auto& Inventory : Data)
    {
        if (!FirstLine){File << "\n\n";}
        FirstLine = false;

        File << '-' << Inventory.first << ">\n";
        for (const auto& pair : *Inventory.second)
        {
            const char* Affix;
            switch (pair.second->t)
            {
                case type::INT: Affix = "#"; break;
                case type::DECIMAL: Affix = "#"; break;
                case type::STRING: Affix = "!"; break;
                case type::BOOL: Affix = ""; break;
                default: throw OCCF_ERROR("\nERROR : idk\n");
            }
            File << "\t?" << pair.first << "? " << Affix << *pair.second << Affix << '\n';
        }
        File << "<-";
    }
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) VALUE (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::_VALUE::_VALUE():s(new std::string("")), t(type::STRING){}
OCCF::_VALUE::_VALUE(int _i) : i(_i),t(type::INT){}
OCCF::_VALUE::_VALUE(double _d) : d(_d),t(type::DECIMAL){}
OCCF::_VALUE::_VALUE(float _d) : d(_d),t(type::DECIMAL){}
OCCF::_VALUE::_VALUE(std::string _s) : s(new std::string(_s)), t(type::STRING){} // CPP String Class
OCCF::_VALUE::_VALUE(const char* _s) : s(new std::string(_s)), t(type::STRING){} // C-Style String 
OCCF::_VALUE::_VALUE(bool _b) : b(_b), t(type::BOOL){}
OCCF::_VALUE::~_VALUE(){}

OCCF::_VALUE::operator int() const
{
    switch (t)
    {
        case type::INT: return i;
        case type::DECIMAL: return int(d);
        case type::STRING:
            if ((s->find_first_not_of("0123456789")) == std::string::npos)
            {return std::stoi(*s);}
            else
            {return int(s->size());}
        case type::BOOL: return int(b);
        default: throw OCCF_ERROR("Error : Unable to convert to type int");
    }
}
OCCF::_VALUE::operator double() const
{
    switch (t)
    {
        case type::INT : return double(i);
        case type::DECIMAL: return double(d);
        case type::STRING: return double(s->size());// REDO THIS SIMILAR TO INT
        case type::BOOL: return double(b);
        default: throw OCCF_ERROR("Error : Unable to convert type to double");
    }
}
OCCF::_VALUE::operator float() const
{
    switch (t)
    {
        case type::INT : return float(i);
        case type::DECIMAL: return float(d);
        case type::STRING: return float(s->size());
        case type::BOOL: return float(b);
        default: throw OCCF_ERROR("Error : Unable to convert type to float");
    }
}
OCCF::_VALUE::operator std::string() const
{
    switch (t)
    {
        case type::INT: return std::to_string(i);
        case type::DECIMAL: return std::to_string(d);
        case type::STRING: return *s;
        case type::BOOL: return b ? "true" : "false";
        default: throw OCCF_ERROR("Error : Unable to convert to type std::string");
    }
}
OCCF::_VALUE::operator const char*() const
{
    switch (t)
    {
        case type::INT: return _VALUE(std::to_string(i).c_str());
        case type::DECIMAL: return _VALUE(std::to_string(d).c_str());
        case type::STRING: return s->c_str();
        case type::BOOL: return b ? "true" : "false";
        default: throw OCCF_ERROR("Error : Unable to convert to type const char*");
    }
}
OCCF::_VALUE::operator bool() const
{
    std::string cache;
    switch (t)
    {
        case type::INT: return i;
        case type::DECIMAL: return d;
        case type::STRING: // Check if string is strictly "true" or "false" **
            if (s->size() > 5){throw OCCF_ERROR("Error : Unable to convert string to type bool");} 
            for (const char& c : *s)
            {cache += c;}
            if (cache == "true"){return true;}
            else if (cache == "false"){return false;}
            throw OCCF_ERROR("Error : Unable to convert string to type bool");
        case type::BOOL: return b;
        default: throw OCCF_ERROR("Error : Unable to convert to type bool");
    }
}

std::ostream& operator<<(std::ostream& output,const OCCF::_VALUE& self)
{
    switch (self.t)
    {
    case OCCF::type::INT: return output << self.i;
    case OCCF::type::DECIMAL: return output << self.d;
    case OCCF::type::STRING: return output << *self.s;
    case OCCF::type::BOOL: return (self.b) ? output << "true" : output << "false" ;
    default: return output;
    }
}

const bool OCCF::_VALUE::isInt(){return t == type::INT;}
const bool OCCF::_VALUE::isDouble(){return t == type::DECIMAL;}
const bool OCCF::_VALUE::isString(){return t == type::STRING;}
const bool OCCF::_VALUE::isBool(){return t == type::BOOL;}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) SECTION (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::_CONTAINER::~_CONTAINER()
{for(const auto& pair:Container){
    if (pair.second->t == OCCF::type::STRING)
    {delete pair.second->s;pair.second->s = nullptr;}
    delete pair.second;}
}

OCCF::_VALUE& OCCF::_CONTAINER::operator[](const int _index)
{
    if (_index < 0){throw OCCF_ERROR("\nERROR : Index failure\nWHAT : Unable to index negative values");}

    std::string index = std::to_string(_index);
    if (Container.find(index) == Container.end()){Container[index] = new _VALUE;}
    return *Container[index];
}
OCCF::_VALUE& OCCF::_CONTAINER::operator[](const char _index)
{
    std::string index(1,_index);
    if (Container.find(index) == Container.end()){Container[index] = new _VALUE;}
    return *Container[index];
}
OCCF::_VALUE& OCCF::_CONTAINER::operator[](const std::string index)
{
    if (Container.find(index) == Container.end()){Container[index] = new _VALUE;}
    return *Container[index];
}

std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::find(std::string key)const{return Container.find(key);}
std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::begin()const{return Container.begin();}
std::map<std::string,OCCF::_VALUE*>::const_iterator OCCF::_CONTAINER::end()const{return Container.end();}

void OCCF::_CONTAINER::erase(const char* Key){this->Container.erase(Key);}
void OCCF::_CONTAINER::erase(const int key)
{
    std::string Key = std::to_string(key);
    this->Container.erase(Key);
}

//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡) OCCF ERROR (｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)
//(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)(｡♥‿♥｡)

OCCF::OCCF_ERROR::OCCF_ERROR(const char* message) : mess(message){}
OCCF::OCCF_ERROR::OCCF_ERROR(std::string message) : mess(message.c_str()){}
const char* OCCF::OCCF_ERROR::what() const noexcept {return mess;}