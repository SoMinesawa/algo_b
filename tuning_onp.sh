#!/bin/bash


rm -f tuning_onp.csv
touch tuning_onp.csv
echo "p_ins,p_sub,p_del,p_nerr,l,a,score,time" >> tuning_onp.csv

gcc -g -lm sample/tuning_onp.c -o tuning_onp -O2 -Wall
for ((p_ins=0; p_ins<=9; p_ins++))
do
for ((p_sub=0; p_sub<=9; p_sub++))
do
for ((p_del=0; p_del<=9; p_del++))
do
    echo "$p_ins" "$p_sub" "$p_del"
for ((l=13; l>=10; l--))
do
a=0.22
a_end=0.50
a_step=0.01
while [[ $(echo "$a <= $a_end" | bc) -eq 1 ]]
do
    echo "$p_ins" "$p_sub" "$p_del" "$l" "$a" 
    ./tuning_onp "all/tuning/data/0" "tuning_onp.csv" "$p_ins" "$p_sub" "$p_del" "$l" "$a"
    a=$(echo "$a + $a_step" | bc)
done
done
done
done
done