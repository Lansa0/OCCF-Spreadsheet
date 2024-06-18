#include "Spreadsheet.hpp"

// Static Helpers
static bool verifyOCCF(OCCF& data,unsigned int& rows,unsigned int& columns)
{
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
            const size_t COLUMN_NAME_LENGTH = ColumnData.first.length();

            if (COLUMN_NAME_LENGTH == 1)
            {
                if (!std::isalpha(ColumnData.first[0]) || !std::isupper(ColumnData.first[0])){return false;}
                ColumnCount = ColumnData.first[0] - ('A' - 1);
            }
            else if (COLUMN_NAME_LENGTH == 2)
            {
                if (
                    !(std::isalpha(ColumnData.first[0]) && std::isupper(ColumnData.first[0]))||
                    !(std::isalpha(ColumnData.first[1]) && std::isupper(ColumnData.first[1]))
                   ){return false;}

                const int _FIRST_LETTER = ColumnData.first[0] - ('A' - 1);
                const int _SECOND_LETTER = ColumnData.first[1] - ('A' - 1);
                ColumnCount = 26 + (_FIRST_LETTER * _SECOND_LETTER);
            }
            else if (COLUMN_NAME_LENGTH >= 3){return false;}
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

    // Top Row 
    std::string TopRow = "\n|///|";
    for (int i1 = 0; i1 < total_columns; i1++)
    {
        for (int i2 = 0; i2 < cell_size; i2++)
        {
            if(i1 < 26)
            {
                if (i2 == CENTER){TopRow += 'A' + i1;}
                else{TopRow += ' ';}
            }
            else
            {
                if (i2 == CENTER-1){TopRow += 'A' + (i1/26) - 1;}
                else if (i2 == CENTER){TopRow += 'A' + (i1%26);}
                else{TopRow += ' ';}
            }
        }
        TopRow += '|';
    }
    row_list[0] = TopRow;

    const unsigned int _ROW_VECTOR_SIZE = row_list.size();
    for (int RowIndex = 1; RowIndex < _ROW_VECTOR_SIZE; RowIndex++)
    {
        std::string RowString = "\n|";
        std::map<int,std::string> CellMap;

        // Hard coding was easier
        if (RowIndex <= 9){RowString += " " + std::to_string(RowIndex) + " |";}
        else if (RowIndex <= 99 ){RowString += std::to_string(RowIndex) + " |";}
        else {RowString += std::to_string(RowIndex) + "|";}

        // Check if Row Exist
        if (sheet_data.find(std::to_string(RowIndex)) != sheet_data.end()) 
        {
            for (const auto& Column : sheet_data[RowIndex])
            {
                const std::string COLUMN_NAME = Column.first;

                unsigned int CellNumber;
                std::string CellString;

                // Get Cell Number for Map Key
                if (COLUMN_NAME.size() == 1)
                {CellNumber = COLUMN_NAME[0] - ('A'-1);}
                else
                {
                    const int _FIRST_LETTER = COLUMN_NAME[0] - ('A'-1);
                    const int _SECOND_LETTER = COLUMN_NAME[1] - ('A'-1);
                    CellNumber = (_FIRST_LETTER * _SECOND_LETTER) + 26;
                }

                // Get Cell String for Map Value
                unsigned int ColumnValueIndex = 0;
                const std::string COLUMN_VALUE = *Column.second;
                const unsigned int COLUMN_VALUE_LENGTH = COLUMN_VALUE.length();
                const unsigned int _RELATIVE_CENTER = (COLUMN_VALUE_LENGTH < cell_size) ? CENTER - (COLUMN_VALUE_LENGTH/2) : 0;
                for (int i = 0; i < cell_size; i++)
                {
                    if ((i >= _RELATIVE_CENTER) && (ColumnValueIndex < COLUMN_VALUE_LENGTH))
                    {
                        CellString += COLUMN_VALUE[ColumnValueIndex];
                        ColumnValueIndex++;
                    }
                    else{CellString += ' ';}             
                }
                CellMap[CellNumber] = CellString + '|';
            }

            // Combine Individual Cells Into One Row
            unsigned int ColumnCount = 0;
            for (const std::pair<int,std::string>& pair : CellMap)
            {      
                if (pair.first > (ColumnCount + 1))
                {
                    const unsigned int _DIFFERNCE = pair.first - ColumnCount;
                    std::cout << pair.first << " : " << *(&ColumnCount) << " : " << _DIFFERNCE << '\n';

                    for (int _ = 1; _ < _DIFFERNCE; _++)
                    {
                        for (int  _ = 0; _ < cell_size; _++){RowString += ' ';}
                        RowString += '|'; 
                    }
                }   
                RowString += pair.second;
                ColumnCount = pair.first;
            }
            
            // Add any missing cells at the end 
            if (ColumnCount < total_columns)
            {
                const unsigned int _DIFFERENCE = total_columns - ColumnCount;
                for (int _ = 0; _ < _DIFFERENCE; _++)
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
static bool parseValue(std::string value,OCCF& data,const int row,const char* column)
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
                const std::string _CHECK = (SearchBool) ? "true" : "false";
                const bool CHECK_STATUS = _CHECK[BoolIndex] == c;
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

bool Spreadsheet::isVerified(){return Verified;}

int Spreadsheet::getRows()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return Rows;
}
int Spreadsheet::getColumns()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return Columns;
}
int Spreadsheet::getCells()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return Rows*Columns;
}
int Spreadsheet::getCellSize()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return CellSize;
}

void Spreadsheet::setCellSize(int input_size)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet Data Unverified");}

    if (input_size < 2)
    {
        input_size = 2;
        std::cerr << "\nWarning : Cannot set Cell Size below 2\n";
    }
    else if (input_size > 100)
    {
        input_size = 100;
        std::cerr << "\nWarning : Cannot set Cell Size above 100\n";
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
void Spreadsheet::draw(std::ofstream& output_file)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    std::string OutterBorder,InnerBorder;
    std::vector<std::string> CellRows(Rows+1);

    createBorders(CellSize,Columns,OutterBorder,InnerBorder);
    createCellRows(*SheetData,CellRows,CellSize,Columns);

    output_file << OutterBorder;
    const unsigned int RowSize = CellRows.size();
    for (int i = 0; i < RowSize; i++)
    {
        output_file << CellRows[i];
        if (i < (RowSize - 1)){output_file << InnerBorder;}
    }
    output_file << OutterBorder;
    output_file.close();
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
        throw std::runtime_error("Failed to open file: " + std::string(output_file_path));
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
        const char* CellColumnInput;
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

                do // Row
                {
                    getInput(IntInput,"Enter Row : ");
                    if (IntInput > 0 && IntInput < 1000)
                    {
                        Rows = (IntInput > Rows) ? IntInput : Rows;
                        break;
                    }
                    std::cout << "\nRow (" << IntInput << ") is out of range [1-999]\n";
                } while (true);

                do // Column
                {
                    getInput(StringInput,"Enter Column : ");
                    const size_t STRING_INPUT_LENGTH = StringInput.length();
                    if (STRING_INPUT_LENGTH == 1)
                    {
                        if (std::isalpha(StringInput[0]) && std::isupper(StringInput[0]))
                        {
                            const int COLUMN_NUMBER = StringInput[0] - ('A'-1);
                            Columns = (COLUMN_NUMBER > Columns) ? COLUMN_NUMBER : Columns;
                            break;
                        }
                    }   
                    else if (STRING_INPUT_LENGTH == 2)
                    {
                        const char FIRST_CHARACTER = StringInput[0];
                        const char SECOND_CHARACTER = StringInput[1];
                        if (
                            std::isalpha(FIRST_CHARACTER) && std::isupper(FIRST_CHARACTER) &&
                            std::isalpha(SECOND_CHARACTER) && std::isupper(SECOND_CHARACTER)
                           )
                        {
                            const int COLUMN_NUMBER = 26 + ((FIRST_CHARACTER - ('A'-1))*(SECOND_CHARACTER - ('A'-1)));
                            Columns = (COLUMN_NUMBER > Columns) ? COLUMN_NUMBER : Columns;
                            break;
                        }
                    }
                    std::cout << "\nColumn (" << StringInput << ") does not exist\n";
                } while (true);
                CellColumnInput = std::string(StringInput).c_str();

                do // Value
                {
                    getInput(StringInput,"Enter Value : ");
                    if (parseValue(StringInput,*SheetData,IntInput,CellColumnInput)){break;}
                    std::cout << "\nFailed to parse\n";
                } while (true);

                draw(output_file_path);
                std::cout << "\nCell (" << CellColumnInput << IntInput << ") Value Set\n";

                break;

            case 2: // Modify Cell Size

                do
                {
                    getInput(IntInput,"Enter Cell Size : ");
                    if (IntInput < 2 || IntInput > 100){std::cout << "\nCell Size (" << IntInput << ") is out of range [2-100]\n";}
                    else{break;}
                } while (true);

                CellSize = IntInput;
                draw(output_file_path);
                std::cout << "\nCell Size (" << IntInput << ") Set\n";
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