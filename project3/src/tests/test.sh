#!/bin/bash

# Runs all tests in the given directory
# The exit code is the number of failed tests, so 0 on success, non-zero for
# failure

directory=""
latex_format=""

# check for usage
if [ $# -eq 0 ]
then
   echo "Usage: $0 test_directory test_directory_2"
   exit 1
fi

# get all directories, and possibly a latex format file
while (( "$#" ))
do
   if [[ -d "$1" ]]
   then
      # directory to process
      directory="$directory $1"
   else
      echo "Invalid directory: $1"
      exit 1
   fi

   shift
done

#bold=`tput smso`
#nbold=`tput rmso`
bold=`tput bold`
nbold=`tput sgr0`

# the number of required tests that have failed
required_failures=0

# variables for grading - tests
name="STUDENT"
score=0
possible=0
required_tests=""
optional_tests=""

for test in $(find $directory -type f -executable)
do
   # test description
   test_description="$(cat $test.c | head -n 1 | cut -d " " -f 2-)"

   # see if test is optional
   # grep will return 0 if the string REQUIRED is found; anything else is
   # optional
   grep -q REQUIRED <<< $test_description
   optional=$?
   if [[ $optional -eq 0 ]]
   then
      status="REQUIRED"
   else
      status="OPTIONAL"
   fi

   # get the number of points for this test
   points="$(echo $test_description | cut -d '-' -f 2 | cut -d ':' -f 1)"

   # remove the required/optional and points value from the description
   test_description="$(echo $test_description | cut -d ' ' -f 2-)"

   # run the test
   output=$($test 2>&1)

   rc="$?"
   if [ $rc -ne 0 ]
   then
      # failure

      echo "${bold}$status TEST FAILED (status code $rc):${nbold} $test"
      echo "Output was:"
      echo $output
      echo

      if [[ $optional -eq 0 ]]
      then
         # required
         required_failures=$(($required_failures+1))
         possible=$(($possible+$points))

         required_tests="$required_tests\n$test_description & 0 & $points \\\\\\\\ \\\\hline"
      else
         # optional
         optional_tests="$optional_tests\n$test_description & 0 & $points \\\\\\\\ \\\\hline"
      fi
   else
      # success
      echo "${bold}$status TEST SUCCESS:${nbold} $test"
      score=$(($score+$points))
      possible=$(($possible+$points))

      if [[ $optional -eq 0 ]]
      then
         # required
         required_tests="$required_tests\n$test_description & $points & $points \\\\\\\\ \\\\hline"
      else
         # optional
         optional_tests="$optional_tests\n$test_description & $points & $points \\\\\\\\ \\\\hline"
      fi
   fi
done

# calculate the student's score for the test portion
test_percentage=$(echo "scale=3;100*$score/$possible" | bc)

echo "The above test results => grade of $test_percentage % for the test section."
echo "Remember, not all of the tests are provided with the project description."

exit $required_failures
