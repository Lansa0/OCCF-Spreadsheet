# OCCF-Spreadsheet
## Code Status
**Major Bug ⚠️**

Columns further than (Z) may not function properly
***
## Documentation
```cpp
Constructor s(OCCF& OCCF)
```
***
```cpp
bool isVerified()
```
returns verified status
***
```cpp
int getRows()
```
returns number of rows
***
```cpp
int getColumns()
```
returns number of columns
***
```cpp
int getCells()
```
returns number of cells
***
```cpp
int getCellSize()
```
returns size of each cell represented in number of spaces
***
```cpp
void setCellSize(int input_size)
```
set size of every cell

**Parameter:**
- `int` InputSize : The specified size of all cells represented in space characters.
  - Range : [2-100]
***
```cpp
void draw()
```
output spreadsheet to the terminal
***
```cpp
void draw(std::ofstream& output_file)
```
output spreadsheet to given file

**Parameter:**
- `std::ofstream&` output_file : The file where the spreadsheet will draw onto
***
```cpp
void draw(const char* output_file_path)
```
output spreadsheet to given file using file's path

**Parameter:**
- `const char*` output_file_path : The file path to where the spreadsheet will draw onto.
***
```cpp
void LiveEditMode(const char* output_file_path,const char* save_file_path)
```
terminal input based real-time editing of the spreadsheet data

**Parameters:**
- `const char*` output_file_path : The file path to where the spreadsheet will draw onto.
- `const char*` save_file_path : The file path where changes to the spreadsheet will save itself to.
  - Default Value "nullptr" : A prompt will ask to provide the file path in the terminal when saving.
***
## Naming Convention
### Variables
- `PascalCase` : standard variables

- `UPPERCASE_AND_UNDERSCORES` : const variables

- `lowercase_and_underscores` : variables derived from function parameters
  - const function parameters remain lowercase
- `_PrefixUnderscore` : variables used only once, mostly by the line 1-3 lines below
  - const rules apply
### Functions
Begin with fully lowercase verb. PascalCase for trailing words.
> [!NOTE]
> Depending on the situation and context of the code, rules are subject to change or nullification
