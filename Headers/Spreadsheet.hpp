#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "OCCF.hpp"

class VerifyError : public std::exception
{
    std::string Message;
    public:
    VerifyError(const std::string _);
    const char* what() const noexcept override;
};

class Spreadsheet
{
    OCCF* SheetData;
    bool Verified = false;
    uint8_t CellSize = 9;
    uint8_t Columns;
    uint16_t Rows;

    public:
    Spreadsheet(OCCF& OCCFData);

    void setCellSize(int Size);
    int getCellSize();

    // Output to terminal
    void draw(); 
    // Output to file
    void draw(const char* OutputFilePath); 

    // Edit the spreadsheet via terminal commands and view your edits on runtime in a file
    void LiveEditMode(const char* OutputFilePath);
};
#endif