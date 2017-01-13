#!/bin/bash
# Script that automatically check all the generation files, take the best genome from each one, and produce a graph.png from it.
# @note: The generation files are files containing the genome of each individual of a population, at a certain generation.
# @author: Brian Delhaisse

# Check all the generation files, and copy the best genome found in a genome<<generation>> file.
echo "Producing genome files from the gen files..."
for i in `ls gen_*`
do
   # nb contains the nb of lines in the file + 1
   nb=`wc -l $i`
   nb=`echo $nb | cut -d ' ' -f 1`
   let "nb=$nb+2"

   # var contains the line nb where the word Organism appears, and the last line
   var=`grep -n Organism $i | cut -d : -f 1`" "$nb
   var=($var)

   # var1 contains the fitness values
   var1=`grep -n Organism $i | cut -d ' ' -f 5`
   
   max=0.0
   let "indexMax=0"
   let "index=0"

   for j in $var1
   do
      # It seems that Bash does not handle floating point numbers.
      if (( $(echo "$max < $j" | bc -l) )); then
         max=$j
         let "indexMax=$index"
      fi

      # increment index
      let "index=$index+1"
   done

   let "end=${var[indexMax+1]}-2"
   let "delta=$end-${var[indexMax]}+1"

   file=`echo genome${i:4}`
   echo "Producing $file"
   head -n $end $i | tail -n $delta > $file
done

# Produce a graph.png from each genome<<generation>> files.
echo ""
echo "Producing graphs from the genome files..."
for i in `ls genome*`
do
   file=`echo graph${i:6}`

   echo "Producing $file.png"
   ./parser23 $i $file
   dot -Tpng $file.gv -o $file.png
   rm $file.gv
done
