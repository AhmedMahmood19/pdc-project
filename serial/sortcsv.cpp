#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <cstdlib>
using namespace std;

#define FILEPATH "../20Kstudentrecords.csv"

int getNoOfRows(fstream &inputfile)
{
    int n = 0;
    string buff;
    while (!inputfile.eof())
    {
        getline(inputfile, buff);
        n++;
    }
    inputfile.clear();                 // clears error flags, needed when we want the fstream in its original condition
    inputfile.seekg(0, inputfile.beg); // sets fstream back to the beginning
    return n - 2;                      // We want to ignore the header and the last blank line
}

int chooseAttr(string headerRow, int &numOfAttrs, int argv=-1)
{
    // create a stream using the header row
    stringstream ss(headerRow);
    int i = 0, option;
    cout << ":::Choose the attribute to sort by, enter the corresponding option number:::\n\nOption No. : Attribute\n\n";
    // Checks if an error was raised(usually EOF)
    while (ss.good())
    {
        string attribute;
        getline(ss, attribute, ','); // gets each comma separated attribute from the stream
        cout << i << "          : " << attribute << endl;
        i++;
    }
    cout << endl;
    numOfAttrs = i; // stores the number of columns/attrs
    // by default argv=-1, indicating we are not using command line args
    if(argv==-1)
    {
        cin >> option;
        //.good() Checks if an error was raised
        while (option < 0 || option >= i || !cin.good())
        {
            std::cin.clear();                                         // clears error flags
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignores all characters entered uptil when user pressed enter
            cout << "Invalid option! Try Again\n";
            cin >> option;
        }
    }
    // If using command line args for automation then the option would be the argv we passed
    else
    {
        option=argv;
        cout<<option<<" chosen due to command line argument\n";
    }
    return option;
}

void getColumn(fstream &inputfile, vector<pair<string, int>> &column, int &chosenAttrNum, int &numOfAttrs, int &numOfRecs)
{
    string line, value;
    int currentRecordNum = 1;
    // while we haven't read all the records
    while (currentRecordNum <= numOfRecs)
    {
        // iterates till we reach the chosen attribute
        for (int i = 0; i <= chosenAttrNum; i++)
        {
            // If we stopped at/chose the last attribute value of a record we get the value till \n instead of ',' since there are no commas after it
            if (i == numOfAttrs - 1)
            {
                // We save that value and break out of the loop
                getline(inputfile, value);
                break;
            }
            // keep reading comma separated values, then stops at and only retains the value we needed
            getline(inputfile, value, ',');
        }
        // Insert the attribute value and record number into the vector
        column.emplace_back(value, currentRecordNum);
        // If we did not choose the last attribute, it means we stopped the getline loop in the middle of a line
        // ,so we skip the rest of this line and continue with the next line
        if (chosenAttrNum != numOfAttrs - 1)
        {
            getline(inputfile, value);
        }
        currentRecordNum++;
    }
}
void merge(vector<pair<string, int>> &column, int left, int mid, int right, string dtype)
{
    int leftSubN = (mid - left) + 1, rightSubN = (right - mid);
    vector<pair<string, int>> leftSub, rightSub;
    leftSub.reserve(leftSubN);
    rightSub.reserve(rightSubN);
    for (int i = 0; i < leftSubN; i++)
    {
        leftSub.emplace_back(column[left + i]);
    }
    for (int i = 0; i < rightSubN; i++)
    {
        rightSub.emplace_back(column[mid + 1 + i]);
    }
    int lefti = 0, righti = 0, i = left;
    // While there are elements left in both sub vectors, we will compare to get the smaller one
    while (lefti < leftSubN && righti < rightSubN)
    {
        // The following section checks the type of the attribute before comparing attribute values
        bool LeftLessThanRight;
        if ((dtype == "string"))
        {
            LeftLessThanRight = leftSub[lefti].first <= rightSub[righti].first;
        }
        else if ((dtype == "int"))
        {
            LeftLessThanRight = atoi(leftSub[lefti].first.c_str()) <= atoi(rightSub[righti].first.c_str());
        }
        else if ((dtype == "float"))
        {
            LeftLessThanRight = atof(leftSub[lefti].first.c_str()) <= atof(rightSub[righti].first.c_str());
        }
        // If Element in Left sub vector is smaller than copy that first otherwise copy the right one
        if (LeftLessThanRight)
        {
            column[i] = leftSub[lefti];
            lefti++;
        }
        else
        {
            column[i] = rightSub[righti];
            righti++;
        }
        i++;
    }
    // While there are elements left in only one of the subvectors then we copy them over without comparing
    while (lefti < leftSubN)
    {
        column[i] = leftSub[lefti];
        lefti++;
        i++;
    }
    while (righti < rightSubN)
    {
        column[i] = rightSub[righti];
        righti++;
        i++;
    }
}
void mergesort(vector<pair<string, int>> &column, int left, int right, string dtype)
{
    if (right > left)
    {
        int mid = (right + left) / 2;
        mergesort(column, left, mid, dtype);
        mergesort(column, mid + 1, right, dtype);
        merge(column, left, mid, right, dtype);
    }
}

void storeSortedFile(vector<pair<string, int>> &column, int &numOfRecs)
{
    string buff;
    fstream original(FILEPATH, ios::in), sorted("sorted.csv", ios::out);
    // First we copy the header row
    getline(original, buff);
    sorted << buff << endl;
    original.clear();                // clears error flags, needed when we want the fstream in its original condition
    original.seekg(0, original.beg); // sets fstream back to the beginning
    // Next we go through the sorted column, pick the lines in order and copy them to the new file
    for (int i = 0; i < numOfRecs; i++)
    {
        // We start getting lines from 0 so we can account for the header row
        for (int j = 0; j <= column[i].second; j++)
        {
            getline(original, buff);
        }
        sorted << buff << endl;
        original.clear();                // clears error flags, needed when we want the fstream in its original condition
        original.seekg(0, original.beg); // sets fstream back to the beginning
    }
    original.close();
    sorted.close();
}

int main(int argc, char *argv[])
{
    string buff;
    // Note: We will cast string to other datatypes according to the option, when comparing
    vector<pair<string, int>> column; // for each record, it stores the value of the chosen attribute and its line no., so it stores a whole column
    int chosenAttrNum, numOfAttrs, numOfRecs;
    fstream inputfile(FILEPATH, ios::in);
    if (!inputfile)
    {
        cout << "File could not be opened!\n";
        return 0;
    }
    // gets total no. of rows so we can reserve memory for the vector to make it efficient
    numOfRecs = getNoOfRows(inputfile);
    column.reserve(numOfRecs);
    // get header row to display options to the user
    getline(inputfile, buff);
    // If command line arguments are passed
    if (argc>1)
    {
        // Use the command line argument as the option
        chosenAttrNum = chooseAttr(buff, numOfAttrs, atoi(argv[1]));
    }
    else
    {
        // get the column number of the attribute to sort by from the user, this function also stores the number of attributes
        chosenAttrNum = chooseAttr(buff, numOfAttrs);
    }
    // We will now be reading from row 1 onwards (since we already read the header row when we used getline previously)
    // and storing all the attribute values/column, to sort the records by
    getColumn(inputfile, column, chosenAttrNum, numOfAttrs, numOfRecs);
    // We don't need to read the file anymore
    inputfile.close();
    if (chosenAttrNum == 6)
    {
        mergesort(column, 0, numOfRecs - 1, "int");
    }
    else if (chosenAttrNum == 7)
    {
        mergesort(column, 0, numOfRecs - 1, "float");
    }
    else
    {
        mergesort(column, 0, numOfRecs - 1, "string");
    }
    storeSortedFile(column, numOfRecs);
    return 0;
}