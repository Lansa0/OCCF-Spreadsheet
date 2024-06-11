#include "Spreadsheet.hpp"

// Static Helpers
static bool verifyOCCF(OCCF& Data,int& Rows,int& Columns)
{
    std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    uint8_t HighestRowNumber = 0,HighestColumnNumber = 0;
    for (const auto& RowData : Data)
    {
    
        try
        {
            size_t pos;
            const int Value = std::stoi(RowData.first,&pos);

            if (pos != RowData.first.length()){return false;}
            if (Value < 0 || Value > 999){return false;}

            HighestRowNumber = (Value > HighestRowNumber) ? Value : HighestRowNumber;
        }
        catch(const std::invalid_argument&){return false;}
        catch(const std::out_of_range&){return false;}
        
        int ColumnCount = 0;
        for (const auto& ColumnData : *RowData.second)
        {
            if (!std::isalpha(ColumnData.first[0]) && ColumnData.first.length() == 1){return false;}

            ColumnCount = Alphabet.find(ColumnData.first[0]) + 1;
            HighestColumnNumber = (ColumnCount > HighestColumnNumber) ? ColumnCount : HighestColumnNumber;
        }
    }
    Rows = HighestRowNumber;
    Columns = HighestColumnNumber;
    return true;
}
static void createBorders(const int CellSize,const int Columns,std::string& Outter,std::string& Inner)
{
    std::string OutterBorder = "\n+===+";
    std::string InnerBorder = "\n+---+";

    for (int _ = 0; _ < Columns; _++)
    {
        for (int _ = 0; _ < CellSize; _++)
        {
            OutterBorder += '=';
            InnerBorder += '-';
        }
        OutterBorder += '+';
        InnerBorder += '+';
    }
    Outter = OutterBorder;
    Inner = InnerBorder;
}
static void createCellRows(OCCF& SheetData,std::vector<std::string>& RowVector,const int CellSize,const int TotalColumns)
{
    const int Center = (CellSize)/2;
    const std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    // Top Row 
    std::string TopRow = "\n|///|";
    for (int i1 = 0; i1 < TotalColumns; i1++)
    {
        for (int i2 = 0; i2 < CellSize; i2++)
        {
            if (i2 == Center){TopRow += Alphabet[i1];}
            else{TopRow += ' ';}
        }
        TopRow += '|';
    }
    RowVector[0] = TopRow;

    const int RowVectorSize = RowVector.size();
    for (int RowIndex = 1; RowIndex < RowVectorSize; RowIndex++)
    {
        std::string RowString = "\n|";
 
        // Hard coding was easier
        if (RowIndex <= 9){RowString += " " + std::to_string(RowIndex) + " |";}
        else if (RowIndex <= 99 ){RowString += std::to_string(RowIndex) + " |";}
        else {RowString += std::to_string(RowIndex) + "|";}

        if (SheetData.find(std::to_string(RowIndex)) != SheetData.end()) // Row Does Exist
        {
            uint8_t ColumnCount = 0;
            for (const auto& Column : SheetData[RowIndex])
            {
                const char ColumnLetter = Column.first[0];
                if (Alphabet[ColumnCount] != ColumnLetter)
                {
                    const uint8_t EmptyCells = Alphabet.find(Column.first[0]);
                    const uint8_t Difference = EmptyCells - ColumnCount;

                    for (int i1 = 0; i1 < Difference; i1++)
                    {
                        ColumnCount++;
                        for (int i2 = 0; i2 < CellSize; i2++){RowString += ' ';}
                        RowString += '|';
                    }
                }

                const std::string ColumnValue = *Column.second;
                int ColumnValueIndex = 0;
                const int ColumnValueLength = ColumnValue.length();
                const int RelativeCenter = (ColumnValueLength < CellSize) ? Center - (ColumnValueLength/2) : 0;
        
                for (int i = 0; i < CellSize; i++)
                {
                    if ((i >= RelativeCenter) && (ColumnValueIndex < ColumnValueLength))
                    {
                        RowString += ColumnValue[ColumnValueIndex];
                        ColumnValueIndex++;
                    }
                    else
                    {RowString += ' ';}
                }
                RowString += '|';
                ColumnCount++;
            }
            if (ColumnCount < TotalColumns)
            {
                const uint8_t Difference = TotalColumns - ColumnCount;
                for (int _ = 0; _ < Difference; _++)
                {
                    for (int  _ = 0; _ < CellSize; _++){RowString += ' ';}
                    RowString += '|';
                }
            }
        }
        else 
        {
            for (int _ = 0; _ < TotalColumns; _++)
            {
                for (int _ = 0; _ < CellSize; _++){RowString += ' ';}
                RowString += '|';
            }
        }
        RowVector[RowIndex] = RowString;
    }
}
static void getInput(int& Input,const char* Message)
{
    while (true)
    {
        int input;

        std::cout << Message;
        std::cin >> input;

        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX,'\n');
            std::cout << "Invalid Input" << std::endl;
        }
        else
        {
            Input = input;
            std::cin.ignore(INT_MAX,'\n');
            return;
        }
    }
}
static void getInput(char& Input,const char* Message)
{
    while (true)
    {
        char input;

        std::cout << Message;
        std::cin >> input;

        if (std::cin.fail() || !std::isalpha(input))
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX,'\n');
            std::cout << "Invalid Input" << std::endl;
        }
        else
        {
            Input = input;
            std::cin.ignore(INT_MAX,'\n');
            return;
        }
    }
}
static void getInput(std::string& Input,const char* Message)
{
    while (true)
    {
        std::cout << Message;

        std::string input;
        if (getline(std::cin,input))
        {
            Input = input;
            return;
        }
        else
        {
            // eof, fail, bad
        }
    }
}
static bool parseValue(std::string Value,OCCF& Data,const int Row,const char Column)
{
    bool IsString=false,IsInt=false;

    bool IsBool=false,SearchBool;
    uint8_t BoolIndex,ValueLength = Value.length();

    bool TypeFind=true;

    std::string BankValue;
    unsigned short CharNumber = 0;
    for (const char c : Value)
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
                if (IsString){Data[Row][Column] = BankValue;}
                else if (IsInt){Data[Row][Column] = int(std::stoi(BankValue));}
                else {Data[Row][Column] = double(std::stod(BankValue));}
                return true;
            }
            else if ((c == '!' && !IsString) || (c == '#' && IsString))
            {return false;}
            else if (IsBool) 
            {
                std::string Check = (SearchBool) ? "true" : "false";
                bool CheckStatus = Check[BoolIndex] == c;
                bool EndOfLine = ValueLength == CharNumber;

                if ((CheckStatus && !EndOfLine) || (EndOfLine && c == 'e') ){BoolIndex++;}
                else {return false;}

                if (SearchBool && BoolIndex == 4)
                {
                    Data[Row][Column] = true;
                    return true;
                }
                else if (!SearchBool && BoolIndex == 5)
                {
                    Data[Row][Column] = false;
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
    int rows,columns;
    if (verifyOCCF(OCCF,rows,columns))
    {
        std::cout 
        << "Spreadsheet Data Succesfully Verified"
        << "\nRows : " << rows
        << "\nColumns : " << columns
        << "\nCells :" << rows*columns << std::endl;

        Verified = true;
        SheetData = &OCCF;
        Rows = rows;
        Columns = columns;
    }
    else
    {std::cerr << "\nWarning : Spreadsheet Data Failed to Verify\n";}
}

int Spreadsheet::getCellSize()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}
    return CellSize;
}
void Spreadsheet::setCellSize(int InputSize)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet Data Unverified");}

    if (InputSize <= 0)
    {
        InputSize = 1;
        std::cerr << "\nWarning : Cannot set Cell Size to 0 or lower\n";
    }
    else if (InputSize > 50)
    {
        InputSize = 50;
        std::cerr << "\nWarning : Cannot set Cell Size above 50\n";
    }
    CellSize = InputSize;
}

void Spreadsheet::draw()
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    std::string OutterBorder,InnerBorder;
    std::vector<std::string> CellRows(Rows+1);

    createBorders(CellSize,Columns,OutterBorder,InnerBorder);
    createCellRows(*SheetData,CellRows,CellSize,Columns);

    std::cout << OutterBorder;
    const int RowSize = CellRows.size();
    for (int i = 0; i < RowSize; i++)
    {
        std::cout << CellRows[i];
        if (i < (RowSize - 1)){std::cout << InnerBorder;}
    }
    std::cout << OutterBorder;
}
void Spreadsheet::draw(const char* OutputFilePath)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    std::string OutterBorder,InnerBorder;
    std::vector<std::string> CellRows(Rows+1);

    createBorders(CellSize,Columns,OutterBorder,InnerBorder);
    createCellRows(*SheetData,CellRows,CellSize,Columns);

    std::ofstream OutputFile(OutputFilePath);
    if (!OutputFile.is_open())
    {
        std::cerr << "File failed to open";
        throw;
    }

    OutputFile << OutterBorder;
    const int RowSize = CellRows.size();
    for (int i = 0; i < RowSize; i++)
    {
        OutputFile << CellRows[i];
        if (i < (RowSize - 1)){OutputFile << InnerBorder;}
    }
    OutputFile << OutterBorder;
    OutputFile.close();
}

void Spreadsheet::LiveEditMode(const char* OutputFilePath,const char* SaveFilePath)
{
    if (!Verified){throw VerifyError("ERROR : Spreadsheet data unverified");}

    draw(OutputFilePath);
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
                    std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    
                    size_t t = Alphabet.find(CellColumnInput);
                    if (t != std::string::npos)
                    {
                        Columns = (t+1 > Columns) ? t+1 : Columns;
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

                draw(OutputFilePath);

                std::cout << "\nCell (" << CellColumnInput << IntInput <<") Value Set\n";
                break;

            case 2: // Modify Cell Size

                getInput(IntInput,"Enter Cell Size : ");
                CellSize = IntInput;
                draw(OutputFilePath);

                std::cout << "\nCell Size Set\n"; 
                break;

            case 3: // Save 

                if (SaveFilePath == nullptr)
                {
                    getInput(StringInput,"Enter File Path : ");
                    SheetData->dump(StringInput.c_str());
                }
                else{SheetData->dump(SaveFilePath);}
                
                std::cout << "\nSave Succesful\n";
                break;

            // case 4: // Redraw 
            //     break;

            default:return;  
        }
    }
}