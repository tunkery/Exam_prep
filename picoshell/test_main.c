#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function prototype - your implementation
int picoshell(char **cmds[]);

// Color codes for output
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"

int g_tests_passed = 0;
int g_tests_total = 0;

void print_test_header(const char *test_name) {
    printf("\n" BLUE "=== TEST: %s ===" RESET "\n", test_name);
    g_tests_total++;
}

void print_pass() {
    printf(GREEN "✓ PASS" RESET "\n");
    g_tests_passed++;
}

void print_fail(const char *reason) {
    printf(RED "✗ FAIL: %s" RESET "\n", reason);
}

// Helper function to capture stdout
char *capture_output(char **cmds[]) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        return NULL;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        
        int result = picoshell(cmds);
        exit(result);
    } else {
        // Parent process
        close(pipe_fd[1]);
        
        static char buffer[4096];
        memset(buffer, 0, sizeof(buffer));
        ssize_t total = 0;
        ssize_t bytes;
        
        while ((bytes = read(pipe_fd[0], buffer + total, sizeof(buffer) - total - 1)) > 0) {
            total += bytes;
        }
        
        close(pipe_fd[0]);
        
        int status;
        waitpid(pid, &status, 0);
        
        // Remove trailing newline for easier comparison
        if (total > 0 && buffer[total - 1] == '\n') {
            buffer[total - 1] = '\0';
        }
        
        return buffer;
    }
    return NULL;
}

// Test 1: Single command (no pipe)
void test_single_command() {
    print_test_header("Single Command");
    
    char **cmds[] = {
        (char*[]){"echo", "Hello", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "Hello") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'Hello'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 2: Simple pipe (2 commands)
void test_simple_pipe() {
    print_test_header("Simple Pipe (echo | cat)");
    
    char **cmds[] = {
        (char*[]){"echo", "test", NULL},
        (char*[]){"cat", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "test") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'test'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 3: Triple pipe
void test_triple_pipe() {
    print_test_header("Triple Pipe (echo | cat | cat)");
    
    char **cmds[] = {
        (char*[]){"echo", "pipeline", NULL},
        (char*[]){"cat", NULL},
        (char*[]){"cat", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "pipeline") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'pipeline'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 4: Pipe with sed
void test_pipe_with_sed() {
    print_test_header("Pipe with sed (echo | sed)");
    
    char **cmds[] = {
        (char*[]){"echo", "hello", NULL},
        (char*[]){"sed", "s/l/x/g", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "hexxo") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'hexxo'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 5: Pipe with grep (match found)
void test_pipe_grep_match() {
    print_test_header("Pipe with grep - match found");
    
    char **cmds[] = {
        (char*[]){"echo", "hello world", NULL},
        (char*[]){"grep", "world", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "hello world") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'hello world'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 6: Pipe with grep (no match)
void test_pipe_grep_no_match() {
    print_test_header("Pipe with grep - no match");
    
    char **cmds[] = {
        (char*[]){"echo", "hello", NULL},
        (char*[]){"grep", "xyz", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    // Should return empty output when grep finds no match
    if (output && strlen(output) == 0) {
        print_pass();
    } else {
        print_fail("Expected empty output");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 7: Complex pipeline (4 commands)
void test_complex_pipeline() {
    print_test_header("Complex Pipeline (4 commands with sed)");
    
    char **cmds[] = {
        (char*[]){"echo", "aaa bbb ccc", NULL},
        (char*[]){"sed", "s/a/x/g", NULL},
        (char*[]){"sed", "s/b/y/g", NULL},
        (char*[]){"sed", "s/c/z/g", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "xxx yyy zzz") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'xxx yyy zzz'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 8: ls | grep pipeline
void test_ls_grep() {
    print_test_header("ls | grep pipeline");
    
    char **cmds[] = {
        (char*[]){"ls", NULL},
        (char*[]){"grep", "picoshell", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    // Should contain "picoshell" in the output
    if (output && strstr(output, "picoshell")) {
        print_pass();
    } else {
        print_fail("Expected to find 'picoshell' in output");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 9: wc -l pipeline
void test_wc_pipeline() {
    print_test_header("echo | wc -w pipeline");
    
    char **cmds[] = {
        (char*[]){"echo", "one two three", NULL},
        (char*[]){"wc", "-w", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    // wc -w should output 3
    if (output && strstr(output, "3")) {
        print_pass();
    } else {
        print_fail("Expected to find '3' in output");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 10: Return value check (success)
void test_return_value_success() {
    print_test_header("Return Value - Success");
    
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        print_fail("Failed to create pipe");
        return;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        
        char **cmds[] = {
            (char*[]){"echo", "test", NULL},
            (char*[]){"cat", NULL},
            NULL
        };
        
        int result = picoshell(cmds);
        exit(result);
    } else {
        // Parent process
        close(pipe_fd[1]);
        char buffer[100];
        read(pipe_fd[0], buffer, sizeof(buffer));
        close(pipe_fd[0]);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            print_pass();
        } else {
            print_fail("Expected return value 0");
            printf("  Got: %d\n", WEXITSTATUS(status));
        }
    }
}

// Test 11: Multiple word arguments
void test_multiple_args() {
    print_test_header("Command with multiple arguments");
    
    char **cmds[] = {
        (char*[]){"echo", "one", "two", "three", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "one two three") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'one two three'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

// Test 12: Long pipeline (5+ commands)
void test_long_pipeline() {
    print_test_header("Long Pipeline (5 commands)");
    
    char **cmds[] = {
        (char*[]){"echo", "test", NULL},
        (char*[]){"cat", NULL},
        (char*[]){"cat", NULL},
        (char*[]){"cat", NULL},
        (char*[]){"cat", NULL},
        NULL
    };
    
    char *output = capture_output(cmds);
    
    if (output && strcmp(output, "test") == 0) {
        print_pass();
    } else {
        print_fail("Output mismatch");
        printf("  Expected: 'test'\n");
        printf("  Got: '%s'\n", output ? output : "(null)");
    }
}

int main(int argc, char **argv) {
    printf(YELLOW "\n╔════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "║   PICOSHELL COMPREHENSIVE TESTER      ║\n" RESET);
    printf(YELLOW "╚════════════════════════════════════════╝\n" RESET);
    
    printf("\nThis tester will check your picoshell implementation.\n");
    printf("Make sure picoshell.c is compiled with this test.\n\n");
    
    // Run all tests
    test_single_command();
    test_simple_pipe();
    test_triple_pipe();
    test_pipe_with_sed();
    test_pipe_grep_match();
    test_pipe_grep_no_match();
    test_complex_pipeline();
    test_ls_grep();
    test_wc_pipeline();
    test_return_value_success();
    test_multiple_args();
    test_long_pipeline();
    
    // Print summary
    printf("\n" YELLOW "╔════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "║           TEST SUMMARY                 ║\n" RESET);
    printf(YELLOW "╚════════════════════════════════════════╝\n" RESET);
    
    printf("\nTests passed: " GREEN "%d" RESET "/" BLUE "%d" RESET "\n", 
           g_tests_passed, g_tests_total);
    
    if (g_tests_passed == g_tests_total) {
        printf(GREEN "\n🎉 ALL TESTS PASSED! 🎉\n" RESET);
        printf(GREEN "Your implementation looks good!\n" RESET);
        return 0;
    } else {
        printf(RED "\n❌ SOME TESTS FAILED\n" RESET);
        printf(YELLOW "Keep working on your implementation!\n" RESET);
        return 1;
    }
}
