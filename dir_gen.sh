#!/bin/bash
mkdir victim
cd victim
mkdir level1
touch file1.txt
touch file2.txt
cd level1
touch file3.txt
mkdir level2
cd level2
touch file4.txt
ln ../../file1.txt lnfile.txt