#!/bin/bash
# Basic test script for mysh shell

# Colors for test output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color


# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Function to run a test
run_test() {
    local test_name="$1"
    local command="$2"
    local expected_output="$3"
    
    echo "Running test: $test_name"
    
    # Run the command in the shell
    local actual_output=$(echo "$command" | $SHELL_PATH 2>&1)
    
    # Check if the output matches the expected output
    if [[ "$actual_output" =~ $expected_output ]]; then
        echo -e "${GREEN}Test passed${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        echo -e "${RED}Test failed${NC}"
        echo "Expected output to match: $expected_output"
        echo "Actual output: $actual_output"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
    
    echo ""
}

# First verify the shell exists
#if [ ! -f "$SHELL_PATH" ]; then
#    echo -e "${RED}Error: Shell executable not found at $SHELL_PATH${NC}"
#    echo "Make sure you've compiled the shell in the parent directory."
#    exit 1
#fi

# Create test directory
mkdir -p test_dir
cd test_dir
SHELL_PATH="../mysh"

# Test 1: Simple command execution
run_test "Simple echo command" "echo Hello, world!" "Hello, world!"

# Test 2: Built-in command
run_test "PWD built-in command" "pwd" "$(pwd)"

# Test 3: Command not found
run_test "Command not found" "nonexistentcommand" "command not found"

# Test 4: Background process
run_test "Background process" "sleep 1 &" "Background"

# Test 5: Pipe
run_test "Pipe command" "echo Hello | grep Hello" "Hello"

# Test 6: Redirection
echo "Test content" > test_file.txt
run_test "Input redirection" "cat < test_file.txt" "Test content"

# Test 7: Output redirection
run_test "Output redirection" "echo Redirected > out_file.txt; cat out_file.txt" "Redirected"

# Test 8: Environment variables
run_test "Environment variable" "export TEST_VAR=hello; echo \$TEST_VAR" "hello"

# Test 9: Alias
run_test "Alias command" "alias myecho='echo Hello'; myecho" "[Hh]ello"

# Test 10: Wildcards
touch file1.txt file2.txt file3.txt
run_test "Wildcard expansion" "ls file*.txt" "file1.txt.*file2.txt.*file3.txt"

# Clean up
cd ..
rm -rf test_dir

# Print summary
echo "Tests completed: $((TESTS_PASSED + TESTS_FAILED))"
echo -e "${GREEN}Passed: $TESTS_PASSED${NC}"
echo -e "${RED}Failed: $TESTS_FAILED${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    exit 0
else
    exit 1
fi
