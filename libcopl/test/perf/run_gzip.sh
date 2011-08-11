#!/bin/bash

# setup files (run before)
#mkdir cantrbry
#cd cantrbry
#wget http://corpus.canterbury.ac.nz/resources/cantrbry.tar.gz
#tar -xf cantrbry.tar.gz
#gzip *
#tar -xf cantrbry.tar.gz
#rm cantrbry.tar.gz
#cd ..

# iterate through files
for i in $(ls cantrbry/*.gz); do
	./gzip_decomp_perf $i ${i%.gz};
done

