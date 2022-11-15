#include <iostream>
#include <omp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <limits>
#include <cstdlib>
#include <iomanip>
#include <chrono>
using namespace std;

#define STOREPATH "../scripts/sortedparallel.csv"
string FILEPATH = "../datasets/12500studentrecords.csv";

int getNoOfRows(ifstream &originalfile, vector<streampos> &pos)
{
    int n = 0;
    string buff;
    while (!originalfile.eof())
    {
        pos.emplace_back(originalfile.tellg());  //stores position of each row for quick random access
        getline(originalfile, buff);
        n++;
    }
    originalfile.clear();                 // clears error flags, needed when we want the filestream in its original condition
    originalfile.seekg(0, originalfile.beg); // sets filestream back to the beginning
    return n - 2;                      // We want to ignore the header and the last blank line
}

int chooseAttr(string headerRow, int &numOfAttrs, int argv = -1)
{
    // create a stream using the header row
    stringstream ss(headerRow);
    int i = 0, option;
    cerr << ":::Choose the attribute to sort by, enter the corresponding option number:::\n\nOption No. : Attribute\n\n";
    // If ss reached EOF, then ss.good() would be false
    while (ss.good())
    {
        string attribute;
        // gets each comma separated attribute from the stream and prints it
        getline(ss, attribute, ',');
        cerr << i << "          : " << attribute << endl;
        i++;
    }
    cerr << endl;
    // stores the number of columns/attrs
    numOfAttrs = i;
    // by default argv=-1, indicating we are not using command line args
    if (argv == -1)
    {
        cin >> option;
        //.good() Checks if an error was raised eg: user enters a character
        while (option < 0 || option >= i || !cin.good())
        {
            std::cin.clear();                                         // clears error flags
            std::cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ignores all characters entered uptil when user pressed enter
            cerr << "Invalid option! Try Again\n";
            cin >> option;
        }
    }
    // If using command line args for automation then the option would be the argv we passed
    else
    {
        option = argv;
        cerr << option << " chosen due to command line argument\n";
    }
    return option;
}

void getColumn(ifstream &originalfile, vector<pair<string, int>> &column, int &chosenAttrNum, int &numOfAttrs, int &numOfRecs)
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
                getline(originalfile, value);
                break;
            }
            // keep reading comma separated values, then stops at and only retains the value we needed
            getline(originalfile, value, ',');
        }
        // Insert the attribute value and record number into the vector
        column.emplace_back(value, currentRecordNum);
        // If we did not choose the last attribute, it means we stopped the getline loop in the middle of a line
        // ,so we skip the rest of this line and continue with the next line
        if (chosenAttrNum != numOfAttrs - 1)
        {
            getline(originalfile, value);
        }
        currentRecordNum++;
    }
}
void merge(vector<pair<string, int>> &column, int left, int mid, int right, string dtype)
{
    // create left and right subvectors
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
        // The following section checks the datatype of the attribute before comparing attribute values
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
        // If Element in Left sub vector is smaller, copy that first otherwise copy the right one
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
    // While there are elements left in only one of the sub vectors then we copy them over without comparing
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
void mergeSort(vector<pair<string, int>> &column, int left, int right, string dtype)
{
    //If subarray can be further divided
    if (right > left){
        // Find the mid index and the size of the current subvector
        int mid = (right + left) / 2;
        int n=(right - left) + 1;
        // The vector "column" is shared by all threads, create a task only if the n is large enough
        #pragma omp task shared(column) if (n > 10)
        {
            // cout<<"I am thread"<<omp_get_thread_num();
            mergeSort(column, left, mid, dtype);
        }
        #pragma omp task shared(column) if (n > 10)
            mergeSort(column, mid+1, right, dtype);
        // Once the single thread has created all the threads, it will wait for all the tasks to be completed before continuing
        // After which, only the single thread runs the merge function
        #pragma omp taskwait
            merge(column, left, mid, right, dtype);
    }
}

void storeSortedFile(vector<pair<string, int>> &column, int &numOfRecs, vector<streampos> &pos)
{
    string buff;
    ifstream original(FILEPATH);
    ofstream sorted(STOREPATH);
    
    // First we copy the header row from the original file to the the sorted file
    getline(original, buff);
    sorted << buff << "\n";
    // Iterate through the sorted column to get the correct order of the row numbers
    for (auto i:column)
    {
        // We use the row number to move the ifstream to the correct row
        original.seekg(pos[i.second], original.beg);

        getline(original, buff);
        original.clear();
        sorted<<buff<<"\n";
    }
    original.close();
    sorted.close();
}

int main(int argc, char *argv[])
{
    // If ran through shell script we would start the timer instantly and use argv as FILEPATH
    // otherwise timer would be started again after user picks an option
    auto start = chrono::high_resolution_clock::now();
    if (argc==3)
    {
        FILEPATH=argv[2];
    }
    
    // for each record, it stores the value of the chosen attribute and its line no., so it stores a whole column
    // Note: While comparing, we will cast string to other datatypes according to the option
    vector<pair<string, int>> column;
    //stores position of each row for quick random access
    vector<streampos> pos;          
    string buff;
    int chosenAttrNum, numOfAttrs, numOfRecs;
    // Open the original unsorted file
    ifstream originalfile(FILEPATH);
    if (!originalfile)
    {
        cerr << "File could not be opened!\n";
        return 0;
    }
    // gets total no. of rows also stores the positions of rows for efficiency
    numOfRecs = getNoOfRows(originalfile,pos);
    // Reserve space in vector to avoid reallocation overhead
    column.reserve(numOfRecs);
    // get the header row to display options to the user
    getline(originalfile, buff);
    
    if (argc == 3)
    {
        // If ran through shell script we would use the command line argument as the option
        chosenAttrNum = chooseAttr(buff, numOfAttrs, atoi(argv[1]));
    }
    else if (argc==1)
    {
        // If ran through command line we would prompt for option and then start timer
        chosenAttrNum = chooseAttr(buff, numOfAttrs);
        start = chrono::high_resolution_clock::now();
    }
    // We will now read from row 1 onwards (since we already read the header row when we used getline previously)
    // We will also store all the chosen attribute's values(a whole column), to sort the records by
    getColumn(originalfile, column, chosenAttrNum, numOfAttrs, numOfRecs);
    // We don't need the file for now so we can close it
    originalfile.close();
    // Now we will sort the column
    // Explicitly disable dynamic teams
    omp_set_dynamic(0);
    //Only 4 threads will exist once we enter the parallel region and only one of them creates tasks
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp single
            if (chosenAttrNum == 6)
            {
                mergeSort(column, 0, numOfRecs - 1, "int");
            }
            else if (chosenAttrNum == 7)
            {
                mergeSort(column, 0, numOfRecs - 1, "float");
            }
            else
            {
                mergeSort(column, 0, numOfRecs - 1, "string");
            }
    }
    storeSortedFile(column, numOfRecs, pos);
    
    auto end = chrono::high_resolution_clock::now();
    double timetaken = (chrono::duration_cast<chrono::nanoseconds>(end - start).count()) * 1e-9;
    
    if (argc==3)
    {
        // If ran through shell script then this would be stored in the log file
        cout << numOfRecs << " , " <<fixed << timetaken << setprecision(9);
    }
    else if (argc==1)
    {
        // If ran through command line, this would be printed
        cout << "Time Taken: " <<fixed << timetaken << setprecision(9);
    }
    return 0;
}