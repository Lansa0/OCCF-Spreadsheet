#include "Spreadsheet.hpp"

int main()
{
    const char* DataFilePath = "example/Data.txt";
    const char* SpreadsheetFilePath = "example/Spreadsheet.txt";

    OCCF Data(DataFilePath);
    Spreadsheet spreadsheet(Data);

    if (spreadsheet.isVerified())
    {
        spreadsheet.setCellSize(5);
        spreadsheet.LiveEditMode(SpreadsheetFilePath,DataFilePath);
    }
}