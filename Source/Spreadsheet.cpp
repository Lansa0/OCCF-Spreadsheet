#include "Spreadsheet.hpp"

// Static Helpers
static bool verifyOCCF(OCCF& data,unsigned int& rows,unsigned int& columns)
{
    const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    unsigned int HighestRowNumber = 0,HighestColumnNumber = 0;
    for (const auto& RowData : data)
    {
        try
        {
            size_t Pos;
            const int VALUE = std::stoi(RowData.first,&Pos);

            if (Pos != RowData.first.length()){return false;}
            if (VALUE < 1 || VALUE > 999){return false;}

            HighestRowNumber = (VALUE > HighestRowNumber) ? VALUE : HighestRowNumber;
        }
        catch(const std::invalid_argument&){return false;}
        catch(const std::out_of_range&){return false;}
        
        unsigned int ColumnCount = 0;
        for (const auto& ColumnData : *RowData.second)
        {
            if (!std::isalpha(ColumnData.first[0]) && ColumnData.first.length() == 1){return false;}

            ColumnCount = ALPHABET.find(ColumnData.first[0]) + 1;
            HighestColumnNumber = (ColumnCount > HighestColumnNumber) ? ColumnCount : HighestColumnNumber;
        }
    }
    rows = HighestRowNumber;
    columns = HighestColumnNumber;
    return true;
}
static void createBorders(const int cell_size,const int columns,std::string& outter_border,std::string& inner_border)
{
    std::string OutterBorder = "\n+===+";
    std::string InnerBorder = "\n+---+";

    for (int _ = 0; _ < columns; _++)
    {
        for (int _ = 0; _ < cell_size; _++)
        {
            OutterBorder += '=';
            InnerBorder += '-';
        }
        OutterBorder += '+';
        InnerBorder += '+';
    }
    outter_border = OutterBorder;
    inner_border = InnerBorder;
}
static void createCellRows(OCCF& sheet_data,std::vector<std::string>& row_list,const int cell_size,const int total_columns)
{
    const unsigned int CENTER = (cell_size)/2;
    const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Top Row 
    std::string TopRow = "\n|///|";
    for (int i1 = 0; i1 < total_columns; i1++)
    {
        for (int i2 = 0; i2 < cell_size; i2++)
        {
            if (i2 == CENTER){TopRow += ALPHABET[i1];}
            else{TopRow += ' ';}
        }
        TopRow += '|';
    }
    row_list[0] = TopRow;

    const unsigned int ROW_VECTOR_SIZE = row_list.size();
    for (int RowIndex = 1; RowIndex < ROW_VECTOR_SIZE; RowIndex++)
    {
        std::string RowString = "\n|";
 
        // Hard coding was easier
        if (RowIndex <= 9){RowString += " " + std::to_string(RowIndex) + " |";}
        else if (RowIndex <= 99 ){RowString += std::to_string(RowIndex) + " |";}
        else {RowString += std::to_string(RowIndex) + "|";}

        if (sheet_data.find(std::to_string(RowIndex)) != sheet_data.end()) // Row Does Exist
        {
            int ColumnCount = 0;
            for (const auto& Column : sheet_data[RowIndex])
            {
                const char COLUMN_LETTER = Column.first[0];
                if (ALPHABET[ColumnCount] != COLUMN_LETTER)
                {
                    const unsigned int EMPTY_CELLS = ALPHABET.find(Column.first[0]);
                    const unsigned int DIFFERENCE = EMPTY_CELLS - ColumnCount;

                    for (int _ = 0; _ < DIFFERENCE; _++)
                    {
                        ColumnCount++;
                        for (int _ = 0; _ < cell_size; _++){RowString += ' ';}
                        RowString += '|';
                    }
                }

                unsigned int ColumnValueIndex = 0;
                const std::string COLUMN_VALUE = *Column.second;
                const unsigned int COLUMN_VALUE_LENGTH = COLUMN_VALUE.length();
                const unsigned int RELATIVE_CENTER = (COLUMN_VALUE_LENGTH < cell_size) ? CENTER - (COLUMN_VALUE_LENGTH/2) : 0;
                for (int i = 0; i < cell_size; i++)
                {
                    if ((i >= RELATIVE_CENTER) && (ColumnValueIndex < COLUMN_VALUE_LENGTH))
                    {
                        RowString += COLUMN_VALUE[ColumnValueIndex];
                        ColumnValueIndex++;
                    }
                    else
                    {RowString += ' ';}
                }
                RowString += '|';
                ColumnCount++;
            }
            if (ColumnCount < total_columns)
            {
                const unsigned int DIFFERENCE = total_columns - ColumnCount;
                for (int _ = 0; _ < DIFFERENCE; _++)
                {
                    for (int  _ = 0; _ < cell_size; _++){RowString += ' ';}
                    RowString += '|';
                }
            }
        }
        else 
        {
            for (int _ = 0; _ < total_columns; _++)
            {
                for (int _ = 0; _ < cell_size; _++){RowString += ' ';}
                RowString += '|';
            }
        }
        row_list[RowIndex] = RowString;
    }
}
static void getInput(int& input,const char* message)
{
    while (true)
    {
        int Input;

        std::cout << message;
        std::cin >> Input;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX,'\n');
            std::cout << "Invalid Input" << std::endl;
        }
        else
        {
            input = Input;
            std::cin.ignore(INT_MAX,'\n');
            return;
        }
    }
}
static void getInput(char& input,const char* message)
{
    while (true)
    {
        char Input;

        std::cout << message;
        std::cin >> Input;

        if (std::cin.fail() || !std::isalpha(Input))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX,'\n');
            std::cout << "Invalid Input" << std::endl;
        }
        else
        {
            input = Input;
            std::cin.ignore(INT_MAX,'\n');
            return;
        }
    }
}
static void getInput(std::string& input,const char* message)
{
    while (true)
    {
        std::cout << message;

        std::string Input;
        if (getline(std::cin,Input))
        {
            input = Input;
            return;
        }
        else
        {
            // eof, fail, bad
        }
    }
}
static bool parseValue(std::string value,OCCF& data,const int row,const char column)
{
    bool IsString=false,IsInt=false;
    bool IsBool=false,SearchBool;
    unsigned int BoolIndex;

    const unsigned int VALUE_LENGTH = value.length();
    bool TypeFind=true;

    std::string BankValue;
    unsigned int CharNumber = 0;
    for (const char c : value)
    {
        CharNumber++;
        if (TypeFind)
        {
            TypeFind = false;
            if (c == '!'){IsString = true;}
        else if (c == '#'){IsInt = true;}
        else if (c == 't' || c == 'f')
        {
            IsBool = true;
            SearchBool = (c == 't') ? true : false;
            BoolIndex = 1;
        }
            else {return false;}       
        }
        else
        {
            if ((c == '!' && IsString) || (c == '#' && !IsString))
            {
                if (IsString){data[row][column] = BankValue;}
                else if (IsInt){data[row][column] = int(std::stoi(BankValue));}
                else {data[row][column] = double(std::stod(BankValue));}
                return true;
            }
            else if ((c == '!' && !IsString) || (c == '#' && IsString))
            {return false;}
            else if (IsBool) 
            {
                const std::string CHECK = (SearchBool) ? "true" : "false";
                const bool CHECK_STATUS = CHECK[BoolIndex] == c;
                const bool END_OF_LINE = (VALUE_LENGTH == CharNumber);

                if ((CHECK_STATUS && !END_OF_LINE) || (END_OF_LINE && c == 'e') ){BoolIndex++;}
                else {return false;}

                if (SearchBool && BoolIndex == 4)
                {
                    data[row][column] = true;
                    return true;
                }
                else if (!SearchBool && BoolIndex == 5)
                {
                    data[row][column] = false;
                    return true;
                }
            }
            else 
            {
                if (!IsString)
                {
                    if (!(isdigit(c) || c =='.')){return false;}
                    else if (c == '.' && !IsInt){return false;}
                    else if (c == '.'){IsInt = false;}
                }
                BankValue += c;
            }
        }
    }
    return false;
}

// Verify Error
VerifyError::VerifyError(std::string message):Message(message){}
const char* VerifyError::what() const noexcept {return Message.c_str();}

// Spreadsheet 
Spreadsheet::Spreadsheet(OCCF& OCCF)
{
    unsigned int _Rows,_Columns;
    if (verifyOCCF(OCCF,_Rows,_Columns))
    {
        std::cout 
        << "Spreadsheet Data Succesfully Verified"
        << "\nRows : " << _Rows
        << "\nColumns : " << _Columns
        << "\nCells :" << _Rows*_Columns << std::endl;

        Verified = true;
        SheetData = &OCCF;
        Rows = _Rows;
        Columns = _Columns;
    }
    else
    {std::cerr << "\nWarning : Spreadsheet Data Failed to Verify\n";}
}

int Spreadsheet::getCellSize()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return CellSize;
}
void Spreadsheet::setCellSize(int input_size)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet Data Unverified");}

    if (input_size <= 0)
    {
        input_size = 1;
        std::cerr << "\nWarning : Cannot set Cell Size to 0 or lower\n";
    }
    else if (input_size > 50)
    {
        input_size = 50;
        std::cerr << "\nWarning : Cannot set Cell Size above 50\n";
    }
    CellSize = input_size;
}

void Spreadsheet::draw()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    std::string OutterBorder,InnerBorder;
    std::vector<std::string> CellRows(Rows+1);

    createBorders(CellSize,Columns,OutterBorder,InnerBorder);
    createCellRows(*SheetData,CellRows,CellSize,Columns);

    std::cout << OutterBorder;
    const unsigned int RowSize = CellRows.size();
    for (int i = 0; i < RowSize; i++)
    {
        std::cout << CellRows[i];
        if (i < (RowSize - 1)){std::cout << InnerBorder;}
    }
    std::cout << OutterBorder;
}
void Spreadsheet::draw(const char* output_file_path)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    std::string OutterBorder,InnerBorder;
    std::vector<std::string> CellRows(Rows+1);

    createBorders(CellSize,Columns,OutterBorder,InnerBorder);
    createCellRows(*SheetData,CellRows,CellSize,Columns);

    std::ofstream OutputFile(output_file_path);
    if (!OutputFile.is_open())
    {
        std::cerr << "File failed to open";
        throw;
    }

    OutputFile << OutterBorder;
    const unsigned int RowSize = CellRows.size();
    for (int i = 0; i < RowSize; i++)
    {
        OutputFile << CellRows[i];
        if (i < (RowSize - 1)){OutputFile << InnerBorder;}
    }
    OutputFile << OutterBorder;
    OutputFile.close();
}

void Spreadsheet::LiveEditMode(const char* output_file_path,const char* save_file_path)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    draw(output_file_path);
    while (true)
    {
        int IntInput;
        char CellColumnInput;
        std::string StringInput;

        std::cout 
        << "\n0. Quit"
        << "\n1. Modify cell"
        << "\n2. Modify cell size"
        << "\n3. Save data to file\n";
        getInput(IntInput,"Intput : ");  

        switch (IntInput)
        {
            case 1: // Modify Cell

                do
                {
                    getInput(IntInput,"Enter Row : ");
                    if (IntInput > 0 && IntInput < 1000)
                    {
                        Rows = (IntInput > Rows) ? IntInput : Rows;
                        break;
                    }
                    std::cout << "Row (" << IntInput << ") is out of range [1-999]\n";
                } while (true);

                do
                {
                    getInput(CellColumnInput,"Enter Column : ");
                    const std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    
                    const size_t COLUMN_INDEX = ALPHABET.find(CellColumnInput);
                    if (COLUMN_INDEX != std::string::npos)
                    {
                        Columns = (COLUMN_INDEX+1 > Columns) ? COLUMN_INDEX+1 : Columns;
                        break;
                    }
                    std::cout << "Column (" << CellColumnInput << ") does not exist\n";
                } while (true);
                
                do
                {
                    getInput(StringInput,"Enter Value : ");
                    if (parseValue(StringInput,*SheetData,int(IntInput),CellColumnInput)){break;}
                    std::cout << "Failed to parse\n";
                } while (true);

                draw(output_file_path);

                std::cout << "\nCell (" << CellColumnInput << IntInput <<") Value Set\n";
                break;

            case 2: // Modify Cell Size

                getInput(IntInput,"Enter Cell Size : ");
                CellSize = IntInput;
                draw(output_file_path);

                std::cout << "\nCell Size Set\n"; 
                break;

            case 3: // Save 

                if (save_file_path == nullptr)
                {
                    getInput(StringInput,"Enter File Path : ");
                    SheetData->dump(StringInput.c_str());
                }
                else{SheetData->dump(save_file_path);}
                
                std::cout << "\nSave Succesful\n";
                break;

            default:return;  
        }
    }
}