#!/bin/bash

rm /home/val/Projects/FDF/_REPOSITORY/fdf-cpp/good.log
rm /home/val/Projects/FDF/_REPOSITORY/fdf-cpp/bad.log

cp ../bin/fdftest ../

cd ..

#while IFS= read -r line; do   
#   ./fdftest "$line"
#done < testfiles.txt

#find /media/val/SSD/temp -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
#    #echo ">>> $file"
#    ./fdftest "$file" >> files.txt
#done

#find ./test/images/StanOriginal -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
#    echo ">>> $file"
#    ./fdftest "$file"
#done

#find ./test/images/FROUD_JPEGS -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
#    echo ">>> $file"
#    ./fdftest "$file"    
#done

#find ./test/images/OK -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
#    echo ">>> $file"
#    ./fdftest "$file"    
#done

find ./test/images/Stan -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
    echo ">>> $file"
    ./fdftest "$file"
done

find ./test/images/FROUD -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
    echo ">>> $file"
    ./fdftest "$file"
done

find ./test/images/FROUD_CUST -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
    echo ">>> $file"
    ./fdftest "$file"
done

find ./test/images/OK_modified -name "*.*" -type f -print0 | while IFS= read -r -d '' file; do
    echo ">>> $file"
    ./fdftest "$file"    
done


cd test
