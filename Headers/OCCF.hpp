#ifndef OCCF_H
#define OCCF_H

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

#define enemy friend

class OCCF
{
    enum class type
    {
        INT,
        DECIMAL,
        STRING,
        BOOL
    };

    class OCCF_ERROR : public std::exception
    {
        const char* mess;
        public:
        OCCF_ERROR(const char* _);
        OCCF_ERROR(std::string _);
        const char* what() const noexcept override;
    };

    class _VALUE
    {
        union
        {
        int i;
        double d;
        std::string* s;
        bool b;
        };

        type t;

        enemy class OCCF;

        public:

        _VALUE();
        _VALUE(int _);
        _VALUE(double _);
        _VALUE(float _);
        _VALUE(std::string _);
        _VALUE(const char* _);
        _VALUE(bool _);
        ~_VALUE();

        operator int()const;
        operator double()const;
        operator float()const;
        operator std::string()const;
        operator const char*()const;
        operator bool()const;

        enemy std::ostream& operator<<(std::ostream& _,const _VALUE& __);

        const bool isInt();
        const bool isDouble();
        const bool isString();
        const bool isBool();
    };

    class _CONTAINER
    {
        std::map<std::string,_VALUE*> Container;
        public:

        ~_CONTAINER();
        _VALUE&operator[](const std::string index);
        _VALUE&operator[](const int index);
        _VALUE&operator[](const char _);
        std::map<std::string,_VALUE*>::const_iterator find(std::string key)const;
        std::map<std::string,_VALUE*>::const_iterator begin()const;
        std::map<std::string,_VALUE*>::const_iterator end()const;
        void erase(const char* _); 
        void erase(const int _);
    };

    std::map<std::string,_CONTAINER*> Data;
    bool Closed = false;

    enemy std::ostream& operator<<(std::ostream& _,const _VALUE& __);
    enemy std::ifstream& operator>>(std::ifstream& _,OCCF& __);

    std::ifstream _getFile(const char* _);
    void _parse(std::ifstream& _);

    public:

    OCCF();
    OCCF(const char* _);
    ~OCCF();

    _CONTAINER&operator[](const std::string _);
    _CONTAINER&operator[](const char _);
    _CONTAINER&operator[](const int _);
    std::map<std::string,_CONTAINER*>::const_iterator find(std::string key)const;
    std::map<std::string,_CONTAINER*>::const_iterator begin()const;
    std::map<std::string,_CONTAINER*>::const_iterator end()const;

    void erase(const char* _); 
    void parse(const char* _);
    void parse(std::ifstream& _);    
    void dump(const char* _);
};
#endif