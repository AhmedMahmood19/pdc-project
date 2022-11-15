# # Compile programs
g++ ../serial/sortcsv.cpp -o ../serial/sortcsv.out
g++ -fopenmp  ../parallel/sortcsv.cpp -o ../parallel/sortcsv.out

# #setup headers for csv
echo "\"Number of Records\",\"Total Time (in s)\"">../graph/serial-log.csv
echo "\"Number of Records\",\"Total Time (in s)\"">../graph/parallel-log.csv

for f in ../datasets/*studentrecords.csv
do
    Var=$(./../serial/sortcsv.out 3 $f)
    echo $Var >> ../graph/serial-log.csv

    Var=$(./../parallel/sortcsv.out 3 $f)
    echo $Var >> ../graph/parallel-log.csv
done
python3 graph.py