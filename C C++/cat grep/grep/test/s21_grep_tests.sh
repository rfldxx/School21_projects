#!/ bin / bash
PATTERN="test/pattern.txt"
RES=""
regex1=")\$"
regex2="^int"
GREEN="\033[32m"
RED="\033[31m"
NORMAL="\033[0m"
DATA_TEST="test/test1.txt test/test2.txt test/test3.txt test/test4.txt test/test5.txt"

#делаем ли проверку на лики
FL=0
if [[ "$FL" -eq 1 ]]
then
echo "\nTEST WITH leaks -atExit -- ...\n"
fi

SUCCESS=0
FAIL=0

echo "TASK: with one flags"

for filename in $DATA_TEST
do
for flag in -i -v -c -l -n
do
for pattern in persian CAT int $regex1 $regex2 $regex3
do
TEST="$flag $pattern $filename"

#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
done
done
done

echo "TASK: with two flags"

for filename in $DATA_TEST
do
for flag1 in -i -v -c -l -n
do
for flag2 in -i -v -c -l -n
do
for pattern in persian CAT int $regex1 $regex2 $regex3
do
if [ $flag1 != $flag2 ]
then
TEST="$flag1 $flag2 $pattern $filename"
#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
fi
done
done
done
done

echo "TASK: with -e flag"

for filename in $DATA_TEST
do
for flag1 in -i -v -c -l -n
do
for pattern1 in persian CAT int $regex1 $regex2 $regex3
do
TEST="$flag1 -e $pattern1 $filename"
#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
done
done
done

echo "TASK: with -f flag"
for filename in $DATA_TEST
do
for flag1 in -i -v -c -l -n
do
TEST="$flag1 -f $PATTERN $filename"
#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
done
done

echo "TASK: with two files"
for filename1 in $DATA_TEST
do
for filename2 in $DATA_TEST
do
if [ $filename1 != $filename2 ]
then
TEST="$flag1 -f $PATTERN $filename1 $filename2"
#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
fi
done
done


for filename1 in $DATA_TEST
do
for filename2 in $DATA_TEST
do
for flag1 in -i -v -c -l -n
do
for pattern in persian CAT int $regex1 $regex2 $regex3
do
if [ $filename1 != $filename2 ]
then
TEST="$flag1 $flag $pattern $filename1 $filename2"
#echo "$TEST"

if [[ "$FL" -eq 1 ]]
then
leaks -atExit -- ./s21_grep "$TEST" > s21_grep.txt
grep -e LEAK s21_grep.txt
fi

./s21_grep $TEST > s21_grep.txt
grep $TEST > grep.txt
RES="$(diff -s s21_grep.txt grep.txt)"
if [ "$RES" == "Files s21_grep.txt and grep.txt are identical" ];
then
#echo "$GREEN SUCCESS $NORMAL"
((SUCCESS ++))
else
echo "$TEST"
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
#echo "________________________\n"
rm s21_grep.txt grep.txt
fi
done
done
done
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
