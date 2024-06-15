#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "OCCF.hpp"

class VerifyError : public std::exception
{
private:
    std::string Message;
public:
    VerifyError(const std::string _);
    const char* what() const noexcept override;
};
class Spreadsheet
{
private:
    OCCF* SheetData;
    bool Verified = false;
    uint8_t CellSize = 5;
    uint16_t Rows,Columns;

public:
    Spreadsheet(OCCF& OCCFData);
    
    bool isVerified();

    int getRows();
    int getColumns();
    int getCells();
    int getCellSize();

    void setCellSize(int size);

    void draw(); 
    void draw(std::ofstream& output_file);
    void draw(const char* output_file_path); 

    void LiveEditMode(const char* output_file_path,const char* save_file_path = nullptr);
};
#endif