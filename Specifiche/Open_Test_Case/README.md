# Open_Test_Case


This folder contains all the test cases provided along with the specification to allow you to check the project timelines during the implementation phase.

- archivio_test_aperti_*n*.zip contains 111 input files and their respective expected outputs.
- extra_test_case.zip contains an extra test case.
- test_gen_2023.zip contains generators to create other test cases.

## How to run with test case

1. run the project by providing as input the test case and as output a txt file on which the result of the execution will be saved
    ```bash
    ./EXECUTABLE < INPUT_FILE_NAME.txt > OUTPUT_FILE_NAME.txt
    ```
2. compare the contents of the output file with the expected output file
    ```bash
    diff ./EXPECTED_OUTPUT.txt ./OUTPUT_FILE_NAME.txt
    ```

## What was used to debug

1. GNU Project Debugger
    ```bash
    gdb ./EXECUTABLE --tui
    ```
2. Address SANitizer - checks for memory access errors at compile time
    ```bash
    gcc -Wall -Werror -O2 -g3 main.c -o EXECUTABLE -fsanitize=address
    ```
3. Valgrind
   1. Memcheck - checks for memory access errors at run time
       ```bash
      valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes ./EXECUTABLE
      ```
   3. Callgrind - tracking machine time per function
      ```bash
      valgrind --tool=callgrind --callgrind-out-file=OUTPUT_FILE_NAME.txt ./EXECUTABLE
      kcachegrind OUTPUT_FILE_NAME.txt
      ```
   5. Massif - tracking memory quantity per function
      ```bash
      valgrind --tool=massif --massif-out-file=OUTPUT_FILE_NAME.txt ./EXECUTABLE
      massif-visualizer OUTPUT_FILE_NAME.txt
      ```
