for i in $(find *.c); do gcc $i -O3 -pthread -o $(echo $i | awk '{split($1,array,".");print(array[1])}'); done
