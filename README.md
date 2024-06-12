# OCCF-Spreadsheet

## Documentation

```cpp
Constructor s(OCCF& OCCF)
```
***
```cpp
int getCellSize()
```
retuns size of each cell represented in number of spaces
***
```cpp
void setCellSize(int IntputSize)
```
set size of each cell
### Parameter: 
- int InputSize : The specified size of all cells represented in space characters.
***
```cpp
void draw()
```
outputs spreadsheet to the terminal
***
```cpp
void draw(const char* OutputFilePath)
```
outputs spreadsheet to given file
### Parameter:
- const char* OutputFilePath : The file path to where the spreadsheet will draw onto.
***
```cpp
void LiveEditMode(const char* OutputFilePath,const char* SaveFilePath = nullptr)
```
terminal input based real-time editing of the spreadsheet data
### Parameters:
- const char* OutputFilePath : The file path to where the spreadsheet will draw onto.
- const char* SaveFilePath : The file path where changes to the spreadsheet will save itself to.
  - Default Value "nullptr" : A prompt will ask to provide the file path in the terminal when saving.
