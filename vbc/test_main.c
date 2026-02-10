#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Color codes for output
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

int g_tests_passed = 0;
int g_tests_total = 0;

void print_test_header(const char *test_name, const char *expression) {
    printf("\n" BLUE "=== TEST %d: %s ===" RESET "\n", g_tests_total + 1, test_name);
    printf("  Expression: '%s'\n", expression);
    g_tests_total++;
}

void print_pass(const char *expected) {
    printf(GREEN "  ✓ PASS - Expected: %s" RESET "\n", expected);
    g_tests_passed++;
}

void print_fail(const char *expected, const char *got) {
    printf(RED "  ✗ FAIL" RESET "\n");
    printf("    Expected: %s\n", expected);
    printf("    Got:      %s\n", got);
}

// Run the vbc program with the given expression and capture output
char *run_vbc(const char *expression, int *exit_code) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return NULL;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        
        // Execute vbc
        execl("./vbc", "vbc", expression, NULL);
        exit(127); // If execl fails
    } else {
        // Parent process
        close(pipe_fd[1]);
        
        static char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t total = 0;
        ssize_t bytes;
        
        while ((bytes = read(pipe_fd[0], buffer + total, sizeof(buffer) - total - 1)) > 0) {
            total += bytes;
        }
        
        close(pipe_fd[0]);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (exit_code) {
            *exit_code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        }
        
        // Remove trailing newline
        if (total > 0 && buffer[total - 1] == '\n') {
            buffer[total - 1] = '\0';
        }
        
        return buffer;
    }
    return NULL;
}

void test_expression(const char *name, const char *expr, const char *expected_output, int expected_exit) {
    print_test_header(name, expr);
    
    int exit_code;
    char *output = run_vbc(expr, &exit_code);
    
    if (!output) {
        print_fail(expected_output, "(failed to run)");
        return;
    }
    
    if (exit_code == expected_exit && strcmp(output, expected_output) == 0) {
        print_pass(expected_output);
    } else {
        print_fail(expected_output, output);
        if (exit_code != expected_exit) {
            printf("    Exit code: expected %d, got %d\n", expected_exit, exit_code);
        }
    }
}

int main(int argc, char **argv) {
    printf(YELLOW "\n╔════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "║      VBC COMPREHENSIVE TESTER         ║\n" RESET);
    printf(YELLOW "╚════════════════════════════════════════╝\n" RESET);
    
    printf("\nThis tester will check your vbc implementation.\n");
    printf("Make sure vbc executable exists in current directory.\n");
    printf("Compile with: gcc -o vbc vbc0.c (or your implementation)\n\n");
    
    // Test if vbc exists
    if (access("./vbc", X_OK) != 0) {
        printf(RED "ERROR: vbc executable not found!\n" RESET);
        printf(YELLOW "Please compile your vbc program first:\n" RESET);
        printf("  gcc -o vbc vbc0.c\n\n");
        return 1;
    }
    
    printf("Running tests...\n");
    
    // Basic tests
    test_expression("Single digit", "1", "1", 0);
    test_expression("Single digit 2", "5", "5", 0);
    test_expression("Single digit 3", "9", "9", 0);
    
    // Addition tests
    test_expression("Simple addition", "2+3", "5", 0);
    test_expression("Multiple addition", "1+2+3", "6", 0);
    test_expression("Long addition", "1+2+3+4+5", "15", 0);
    
    // Multiplication tests
    test_expression("Simple multiplication", "2*3", "6", 0);
    test_expression("Multiple multiplication", "2*3*4", "24", 0);
    
    // Order of operations tests
    test_expression("Multiply before add", "3*4+5", "17", 0);
    test_expression("Add then multiply", "3+4*5", "23", 0);
    test_expression("Complex order", "2+3*4+5", "19", 0);
    
    // Parenthesis tests
    test_expression("Simple parenthesis", "(1)", "1", 0);
    test_expression("Multiple nested", "(((((((3)))))))", "3", 0);
    test_expression("Change order", "(3+4)*5", "35", 0);
    test_expression("Complex with parens", "(1+2)*3", "9", 0);
    test_expression("Nested arithmetic", "((6*6+7+5+8)*(1+0+4*8+7)+2)+4*(1+2)", "2254", 0);
    
    // Complex expression tests
    test_expression("Very nested", "(((((2+2)*2+2)*2+2)*2+2)*2+2)*2", "188", 0);
    test_expression("Mix of operations", "2+3*4+5*6+7", "49", 0);
    
    // From subject examples
    test_expression("Subject example 1", "3*4+5", "17", 0);
    test_expression("Subject example 2", "3+4*5", "23", 0);
    test_expression("Subject example 3", "(3+4)*5", "35", 0);
    
    // Error cases
    test_expression("Incomplete expression", "1+", "Unexpected end of input", 1);
    test_expression("Unexpected closing paren", "1+2)", "Unexpected token ')'", 1);
    test_expression("Missing operand", "1++2", "Unexpected token '+'", 1);
    test_expression("Just operator", "+", "Unexpected token '+'", 1);
    test_expression("Just closing paren", ")", "Unexpected token ')'", 1);
    test_expression("Unmatched open paren", "(1+2", "Unexpected token ')'", 1);
    test_expression("Invalid character", "1+a", "Unexpected token 'a'", 1);
    test_expression("Trailing operator", "1+2*", "Unexpected end of input", 1);
    
    // Edge cases
    test_expression("All zeros", "0+0+0", "0", 0);
    test_expression("Zero multiply", "5*0", "0", 0);
    test_expression("Zero in expression", "5+0+3", "8", 0);
    
    // Larger numbers through calculation
    test_expression("Result > 10", "9+9", "18", 0);
    test_expression("Large multiplication", "9*9", "81", 0);
    test_expression("Build larger number", "9*9+9*9+9+9", "180", 0);
    
    // More parenthesis tests
    test_expression("Nested parens", "((1+2)*(3+4))", "21", 0);
    test_expression("Multiple groups", "(1+2)+(3+4)", "10", 0);
    test_expression("Parens with multiply", "2*(3+4)", "14", 0);
    
    // Print summary
    printf("\n" YELLOW "╔════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "║           TEST SUMMARY                 ║\n" RESET);
    printf(YELLOW "╚════════════════════════════════════════╝\n" RESET);
    
    printf("\nTests passed: " GREEN "%d" RESET "/" BLUE "%d" RESET "\n", 
           g_tests_passed, g_tests_total);
    
    double percentage = (double)g_tests_passed / g_tests_total * 100;
    printf("Success rate: %.1f%%\n", percentage);
    
    if (g_tests_passed == g_tests_total) {
        printf(GREEN "\n🎉 ALL TESTS PASSED! 🎉\n" RESET);
        printf(GREEN "Your implementation is correct!\n" RESET);
        return 0;
    } else if (percentage >= 80) {
        printf(YELLOW "\n⚠️  MOST TESTS PASSED\n" RESET);
        printf(YELLOW "You're close! Fix the remaining issues.\n" RESET);
        return 1;
    } else {
        printf(RED "\n❌ MANY TESTS FAILED\n" RESET);
        printf(YELLOW "Keep working on your implementation!\n" RESET);
        return 1;
    }
}
