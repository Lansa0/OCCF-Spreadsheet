# OCCF-Spreadsheet

## Documentation
```cpp
Constructor s(OCCF& OCCF)
```
***
```cpp
int getCellSize()
```
Retuns size of each cell represented in number of spaces
***
```cpp
void setCellSize(int IntputSize)
```
Set size of each cell
### Parameter: 
- `int` InputSize : The specified size of all cells represented in space characters.
  - Range : [1-50]
***
```cpp
void draw()
```
Output spreadsheet to the terminal
***
```cpp
void draw(const char* OutputFilePath)
```
Output spreadsheet to given file
### Parameter:
- `const char*` OutputFilePath : The file path to where the spreadsheet will draw onto.
***
```cpp
void LiveEditMode(const char* OutputFilePath,const char* SaveFilePath)
```
Terminal input based real-time editing of the spreadsheet data
### Parameters:
- `const char*` OutputFilePath : The file path to where the spreadsheet will draw onto.
- `const char*` SaveFilePath : The file path where changes to the spreadsheet will save itself to.
  - Default Value "nullptr" : A prompt will ask to provide the file path in the terminal when saving.
***
***
## Naming Convention
### Variables
- `PascalCase` : standard variables

- `UPPERCASE_AND_UNDERSCORES` : constant variables

- `lowercase_and_underscores` : variables derived from function parameters
  - Constant function parameters remain lowercase
### Functions
Begin with fully lowercase verb. PascalCase for trailing words.
> [!NOTE]
> Depending on the situation and context of the code, rules are subject to change or nullification
