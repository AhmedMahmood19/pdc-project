# Steps:
## 1. Open the terminal in the cloned folder and run these commands only the first time
```
$ python3 -m venv my_env
$ source my_env/bin/activate
$ pip3 install -r requirements.txt
```
## 2. Whenever you want to generate a CSV file filled with dummy data then run this file. To change the number of records in the file, go to the python file and modify the loop range at line 6
```
$ python3 createcsv.py
```
## 3. To sort the file go to the serial directory and run the following commands
```
$ g++ sortcsv.cpp -o sortcsv.out
$ ./sortcsv.out
```
## 4. To check the new sorted file open the sorted.csv file in the serial directory