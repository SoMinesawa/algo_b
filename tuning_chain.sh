#!/bin/bash

# "tuning_chain.csv"ファイルを削除する
rm -f tuning_chain.csv

# "tuning_chain.csv"ファイルを新規作成する
touch tuning_chain.csv

# "tuning_chain.csv"ファイルの1行目に "p_ins,p_sub,p_del,p,t,l,score,time" という文字列を書き込む
echo "p_ins,p_sub,p_del,p_nerr,l,a,score,time" >> tuning_chain.csv

gcc -g -lm sample/tuning_chain.c -o tuning_chain -O2 -Wall
for ((p_ins=0; p_ins<=9; p_ins++))
do
for ((p_sub=0; p_sub<=9; p_sub++))
do
for ((p_del=0; p_del<=9; p_del++))
do
    echo "$p_ins" "$p_sub" "$p_del"
for ((l=11; l>=7; l--))
do
a=0.20
a_end=0.35
a_step=0.01
while [[ $(echo "$a <= $a_end" | bc) -eq 1 ]]
do
    ./tuning_chain "all/tuning/data/0" "tuning_chain.csv" "$p_ins" "$p_sub" "$p_del" "$l" "$a"
    a=$(echo "$a + $a_step" | bc)
    
done
done
done
done
done