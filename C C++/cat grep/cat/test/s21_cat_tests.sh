#!/ bin / bash

FIRST_TEST="test/first.txt"
SECOND_TEST="test/second.txt"
THIRD_TEST="test/third.txt"
DATA_TEST="$FIRST_TEST $SECOND_TEST $THIRD_TEST"
RES=""
GREEN="\033[32m"
RED="\033[31m"
NORMAL="\033[0m"

SUCCESS=0
FAIL=0
echo "------------------------TEST1.---------------------------"
echo "----------------------with one flag----------------------"

for filename in $DATA_TEST
do
for flag in -b -v -e -n -s -t
do
TEST="$flag $filename"
echo "$TEST"
./s21_cat $TEST > s21_cat.txt
cat $TEST > cat.txt
RES="$(diff -s s21_cat.txt cat.txt)"

if [ "$RES" == "Files s21_cat.txt and cat.txt are identical" ];
then
echo "$GREEN SUCCES $NORMAL"
((SUCCESS ++))
else
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
rm s21_cat.txt cat.txt
done
done

echo "------------------------TEST2----------------------------"
echo "----------------------with two flag----------------------"
for filename in $DATA_TEST
do
for flag1 in -b -v -e -n -s -t
do
for flag2 in -b -v -e -n -s -t
do
if [ $flag1 != $flag2 ]
then
TEST="$flag1 $flag2 $filename"
echo "$TEST"
./s21_cat $TEST > s21_cat.txt
cat $TEST > cat.txt
RES="$(diff -s s21_cat.txt cat.txt)"
if [ "$RES" == "Files s21_cat.txt and cat.txt are identical" ]
then
echo "$GREEN SUCCES $NORMAL"
((SUCCESS ++))
else
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
rm s21_cat.txt cat.txt
fi
done
done
done

echo "------------------------TEST3----------------------------"
echo "----------------------with three flag--------------------"
for filename in $DATA_TEST
do
for flag1 in -b -v -e -n -s -t
do
for flag2 in -b -v -e -n -s -t
do
for flag3 in -b -v -e -n -s -t
do
if [ $flag1 != $flag2 ] && [ $flag1 != $flag3 ] && [ $flag2 != $flag3 ]
then
TEST="$flag1 $flag2 $flag3 $filename"
echo "$TEST"
./s21_cat $TEST > s21_cat.txt
cat $TEST > cat.txt
RES="$(diff -s s21_cat.txt cat.txt)"
if [ "$RES" == "Files s21_cat.txt and cat.txt are identical" ]
then
echo "$GREEN SUCCES $NORMAL"
((SUCCESS ++))
else
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
rm s21_cat.txt cat.txt
fi
done
done
done
done

echo "------------------------TEST4-----------------------------"
echo "----------------------with all flags----------------------"
for filename in $DATA_TEST
do
TEST="-b -v -e -n -s -t $filename"
echo "$TEST"
./s21_cat $TEST > s21_cat.txt
cat $TEST > cat.txt
RES="$(diff -s s21_cat.txt cat.txt)"
if [ "$RES" == "Files s21_cat.txt and cat.txt are identical" ]
then
echo "$GREEN SUCCES $NORMAL"
((SUCCESS ++))
else
echo "$RED FAIL $NORMAL"
((FAIL ++))
fi
rm s21_cat.txt cat.txt
done

echo "FAIL: $FAIL"
echo "SUCCESS: $SUCCESS"
