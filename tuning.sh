#!/bin/bash

# "tuning_Btree.csv"ファイルを削除する
rm -f tuning_Btree.csv

# "tuning_Btree.csv"ファイルを新規作成する
touch tuning_Btree.csv

# "tuning_Btree.csv"ファイルの1行目に "p_ins,p_sub,p_del,p,t,l,score,time" という文字列を書き込む
echo "p_ins,p_sub,p_del,p,t,l,score,time" >> tuning_Btree.csv

gcc -g -lm sample/tuning_Btree.c -o tuning_Btree -O2 -Wall
for i in {3..30}
do
    ./tuning_Btree "all/tuning/data/0" "tuning_Btree.csv" "$i"
done