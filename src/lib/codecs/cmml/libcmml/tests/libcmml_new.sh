for i in *.cmml
do
../tools/cmml-validate $i > ${i}.correct

done