echo "Running TEST 9..."
valgrind -s --leak-check=full ./hw3-parallel 3 3 2 0 7 > test9.txt 2>&1
echo "Completed TEST 9."
echo
echo "Running TEST 12..."
valgrind -s --leak-check=full ./hw3-parallel 4 3 1 2 7 > test12.txt 2>&1
echo "Completed TEST 12."
echo
echo "Running TEST 14..."
valgrind -s --leak-check=full ./hw3-parallel 4 4 1 1 13 > test14.txt 2>&1
echo "Completed TEST 14."
echo
echo "Running TEST 16..."
valgrind -s --leak-check=full ./hw3-parallel 4 4 0 1 1 > test16.txt 2>&1
echo "Completed TEST 16."
echo