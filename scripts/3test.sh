# Compile programs
g++ -fopenmp  ../serial/sortcsv.cpp -o ../serial/sortcsv.out
g++ -fopenmp  ../parallel/sortcsv.cpp -o ../parallel/sortcsv.out

#setup headers for csv
echo "\"Number of Records\",\"Total Time (in s)\"">../graph/serial-log.csv
echo "\"Number of Records\",\"Total Time (in s)\"">../graph/parallel-log.csv

size=(100 500 2500 12500 62500 90000 100000)
i=0
for f in *studentrecords.csv
do
    sleep 1
    mv $f "studentrecords.csv"

    Var=$(./../serial/sortcsv.out 3)
    echo "${size[i]},$Var" >> ../graph/serial-log.csv

    Var=$(./../parallel/sortcsv.out 3)
    echo "${size[i]},$Var" >> ../graph/parallel-log.csv
    
    mv "studentrecords.csv" $f
    sleep 1
    ((i++))
done
python3 1graph.py