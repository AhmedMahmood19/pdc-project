# Project structure:
* ## datasets/ contains already created datasets of various sizes
* ## graph/ contains the logs created by runall.sh and the graph created from the logs
* ## parallel/ contains the parallel program
* ## serial/ contains the serial program
* ## scripts/ contains runall.sh(used to run all programs and generate a graph), generatecsv.py(used to generate a dataset), graph.py(used to generate a graph from log files)

# Instructions:
* ## Open the terminal in the cloned folder and run these commands only the first time
    ```
    $ python3 -m venv my_env
    $ source my_env/bin/activate
    $ pip3 install -r requirements.txt
    ```
* ## If you want to generate a CSV file with N(replace this with a positive integer) records then run the following commands:
    ```
    $ cd scripts
    $ python3 generatecsv.py N
    ```
* ## You can run the serial program by editing serial/sortcsv.cpp and setting the path to your input file in "FILEPATH" on line 12 and run the following commands:
    (To check the newly sorted file go to scripts/sortedserial.csv)
    ```
    $ cd serial
    $ g++ sortcsv.cpp -o sortcsv.out
    $ ./sortcsv.out
    ```

* ## You can run the parallel program by editing parallel/sortcsv.cpp and setting the path to your input file in "FILEPATH" on line 13 and run the following commands:
    (To check the newly sorted file go to scripts/sortedparallel.csv)
    ```
    $ cd parallel
    $ g++ -fopenmp sortcsv.cpp -o sortcsv.out
    $ ./sortcsv.out
    ```

* ## You can run both serial and parallel programs on all the stored datasets to create a graph by running the following commands:
    ```
    $ cd scripts
    $ bash runall.sh
    ```