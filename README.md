# Exam Practice Testers

This repository contains three C programming exercises with comprehensive test suites to help you practice and verify your implementations.

## 📚 Exercises

### 1. ft_popen
A function that mimics the behavior of `popen()` from libc, creating a pipe connection to a child process.

**Location:** `/ft_popen/`

**Files:**
- `subject.txt` - Exercise description
- `ft_popen.c` - Your implementation
- `test_main.c` - Comprehensive test suite
- `Makefile` - Build configuration

### 2. picoshell
A function that executes a pipeline of commands, connecting the output of one command to the input of the next.

**Location:** `/picoshell/`

**Files:**
- `subject.txt` - Exercise description
- `picoshell.c` - Your implementation
- `test_main.c` - Comprehensive test suite
- `Makefile` - Build configuration

### 3. vbc (Value By Calculation)
A program that evaluates mathematical expressions with addition, multiplication, and parentheses.

**Location:** `/vbc/`

**Files:**
- `subject.txt` - Exercise description
- `vbc0.c` - Working reference implementation
- `given.c` - Skeleton code to complete
- `test_main.c` - Comprehensive test suite
- `Makefile` - Build configuration

---

## 🚀 Quick Start

### Testing ft_popen

```bash
cd ft_popen
make                # Compile your implementation with tests
make run            # Run the test suite
make valgrind       # Check for memory leaks
make clean          # Clean up object files
make fclean         # Clean up everything
```

### Testing picoshell

```bash
cd picoshell
make                # Compile your implementation with tests
make run            # Run the test suite
make valgrind       # Check for memory leaks
make clean          # Clean up object files
make fclean         # Clean up everything
```

### Testing vbc

```bash
cd vbc
make                # Compile vbc and test suite
make run            # Run the test suite
make valgrind       # Check for memory leaks
make clean          # Clean up object files
make fclean         # Clean up everything
```

---

## 📝 How to Use These Testers

### 1. Read the Subject
First, read the `subject.txt` file in each directory to understand the requirements.

### 2. Write Your Implementation
- For **ft_popen**: Edit `ft_popen.c`
- For **picoshell**: Edit `picoshell.c`
- For **vbc**: Edit `given.c` or create your own implementation

### 3. Compile and Test
Use the Makefile commands:
```bash
make        # Compile everything
make run    # Run tests
```

### 4. Understand Test Results
The testers provide clear output:
- ✓ **Green checkmarks** = Test passed
- ✗ **Red X marks** = Test failed
- **Summary** shows how many tests passed

### 5. Debug Failed Tests
When a test fails, the output shows:
- What was expected
- What your code produced
- Why it failed

### 6. Check for Memory Leaks
```bash
make valgrind
```
This checks for:
- Memory leaks
- File descriptor leaks
- Invalid memory access

---

## 🎯 Test Coverage

### ft_popen Tests
1. Basic read mode ('r')
2. Basic write mode ('w')
3. Invalid parameters
4. Pipe with ls command
5. Multiple simultaneous operations
6. Non-existent command handling
7. Write mode with grep
8. Chaining commands (ls | grep)

### picoshell Tests
1. Single command (no pipe)
2. Simple pipe (2 commands)
3. Triple pipe (3 commands)
4. Pipe with sed
5. Pipe with grep (match found)
6. Pipe with grep (no match)
7. Complex pipeline (4 commands)
8. ls | grep pipeline
9. wc pipeline
10. Return value verification
11. Multiple arguments
12. Long pipeline (5+ commands)

### vbc Tests
1. Single digits (0-9)
2. Simple addition
3. Multiple addition
4. Simple multiplication
5. Multiple multiplication
6. Order of operations (multiplication before addition)
7. Simple parentheses
8. Nested parentheses
9. Complex expressions
10. Error cases (invalid syntax)
11. Edge cases (zeros, large results)

---

## 💡 Tips for Practice

### General Tips
1. **Read the subject carefully** - Understand all requirements
2. **Start simple** - Get basic cases working first
3. **Test frequently** - Run tests after each change
4. **Use valgrind** - Always check for memory leaks
5. **Debug systematically** - Fix one failing test at a time

### ft_popen Tips
- Remember to close file descriptors properly
- Handle error cases (NULL parameters, invalid type)
- Don't forget to fork before execvp
- Parent and child need different return paths

### picoshell Tips
- Track which file descriptors to close at each stage
- Remember: last_fd stores the read end for the next iteration
- Close write end in parent, close both ends after dup2 in child
- Wait for all child processes before returning

### vbc Tips
- Implement operator precedence correctly (* before +)
- Handle parentheses by recursively parsing
- Check for syntax errors (unexpected tokens, end of input)
- Use a tree structure to represent the expression

---

## 🔧 Compilation Flags

All Makefiles use these flags:
```
-Wall -Wextra -Werror -g
```

- `-Wall -Wextra` = Enable all warnings
- `-Werror` = Treat warnings as errors
- `-g` = Include debug symbols for valgrind/gdb

---

## 📊 Understanding Test Output

### Success Example
```
=== TEST: Basic Read Mode ('r') ===
✓ PASS

╔════════════════════════════════════════╗
║           TEST SUMMARY                 ║
╚════════════════════════════════════════╝

Tests passed: 8/8

🎉 ALL TESTS PASSED! 🎉
Your implementation looks good!
```

### Failure Example
```
=== TEST: Basic Read Mode ('r') ===
✗ FAIL: Did not read expected output
  Expected: 'Hello World'
  Got: ''

╔════════════════════════════════════════╗
║           TEST SUMMARY                 ║
╚════════════════════════════════════════╝

Tests passed: 7/8

❌ SOME TESTS FAILED
Keep working on your implementation!
```

---

## 🐛 Debugging Help

### Using GDB
```bash
gcc -g ft_popen.c test_main.c -o test
gdb ./test
(gdb) run
(gdb) backtrace
```

### Using Valgrind
```bash
make valgrind
```

Look for:
- "definitely lost" (memory leaks)
- "file descriptor ... open" (FD leaks)
- "Invalid read/write" (buffer overflows)

### Common Issues

**ft_popen:**
- Forgetting to close FDs in child process
- Not checking for NULL parameters
- Wrong FD returned for 'r' vs 'w'

**picoshell:**
- Not closing all file descriptors
- Wrong order of dup2 and close
- Not waiting for all children

**vbc:**
- Wrong operator precedence
- Not handling nested parentheses
- Missing error checks for invalid syntax

---

## 📖 Example Usage

### Testing Your Own Code

1. **Write your implementation:**
```c
// In ft_popen.c
int ft_popen(const char *file, char *const argv[], char type)
{
    // Your code here
}
```

2. **Compile and test:**
```bash
make run
```

3. **If tests fail, check output and fix:**
```bash
# See which test failed and why
# Fix the issue
# Run again
make re && make run
```

4. **Check for leaks:**
```bash
make valgrind
```

5. **Submit when all tests pass!**

---

## 🎓 Learning Resources

### System Calls Used

**ft_popen:**
- `pipe()` - Create a pipe
- `fork()` - Create child process
- `dup2()` - Duplicate file descriptors
- `execvp()` - Execute program
- `close()` - Close file descriptor

**picoshell:**
- `pipe()` - Create pipes between commands
- `fork()` - Create child processes
- `dup2()` - Redirect I/O
- `execvp()` - Execute commands
- `wait()` - Wait for children
- `close()` - Close file descriptors

**vbc:**
- `malloc()`, `calloc()`, `free()` - Memory management
- `printf()` - Output results
- `isdigit()` - Check for digits

### Recommended Reading
- `man pipe`
- `man fork`
- `man dup2`
- `man execvp`
- `man wait`

---

## 🤝 Contributing

If you find bugs in the testers or want to add more test cases, feel free to modify the `test_main.c` files!

---

## ⚖️ License

These testers are provided for educational purposes. Use them to practice and improve your C programming skills!

---

## ✨ Good Luck!

Remember: The goal is to learn, not just to pass tests. Understand *why* your code works, not just *that* it works.

Happy coding! 🚀
