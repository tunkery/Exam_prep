#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function prototype - your implementation
int ft_popen(const char *file, char *const argv[], char type);

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

// Test 1: Basic 'r' type - reading from command output
void test_basic_read() {
    print_test_header("Basic Read Mode ('r')");
    
    char *args[] = {"echo", "Hello World", NULL};
    int fd = ft_popen("echo", args, 'r');
    
    if (fd == -1) {
        print_fail("ft_popen returned -1");
        return;
    }
    
    char buffer[100] = {0};
    ssize_t bytes_read = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);
    wait(NULL); // Wait for child process
    
    if (bytes_read > 0 && strstr(buffer, "Hello World")) {
        print_pass();
    } else {
        print_fail("Did not read expected output");
        printf("  Expected: 'Hello World'\n");
        printf("  Got: '%s'\n", buffer);
    }
}

// Test 2: Basic 'w' type - writing to command input
void test_basic_write() {
    print_test_header("Basic Write Mode ('w')");
    
    char *args[] = {"cat", NULL};
    int fd = ft_popen("cat", args, 'w');
    
    if (fd == -1) {
        print_fail("ft_popen returned -1");
        return;
    }
    
    const char *test_data = "Test Data\n";
    ssize_t bytes_written = write(fd, test_data, strlen(test_data));
    close(fd);
    wait(NULL); // Wait for child process
    
    if (bytes_written > 0) {
        print_pass();
    } else {
        print_fail("Failed to write to command");
    }
}

// Test 3: Chaining commands (like in the subject example)
void test_chaining_commands() {
    print_test_header("Chaining Commands (ls | grep)");
    
    // First: ls
    char *args_ls[] = {"ls", NULL};
    int fd1 = ft_popen("ls", args_ls, 'r');
    
    if (fd1 == -1) {
        print_fail("First ft_popen returned -1");
        return;
    }
    
    // Redirect fd1 to stdin for next command
    dup2(fd1, STDIN_FILENO);
    close(fd1);
    
    // Second: grep for .c files
    char *args_grep[] = {"grep", ".c", NULL};
    int fd2 = ft_popen("grep", args_grep, 'r');
    
    if (fd2 == -1) {
        print_fail("Second ft_popen returned -1");
        return;
    }
    
    char buffer[1024] = {0};
    ssize_t bytes = read(fd2, buffer, sizeof(buffer) - 1);
    close(fd2);
    
    // Wait for both children
    wait(NULL);
    wait(NULL);
    
    if (bytes > 0 && (strstr(buffer, ".c") || strstr(buffer, "ft_popen.c"))) {
        print_pass();
    } else {
        print_fail("Did not get expected grep output");
    }
}

// Test 4: Invalid parameters
void test_invalid_params() {
    print_test_header("Invalid Parameters");
    
    int all_failed_correctly = 1;
    
    // NULL file
    if (ft_popen(NULL, (char*[]){"test", NULL}, 'r') != -1) {
        printf(RED "  - NULL file should return -1" RESET "\n");
        all_failed_correctly = 0;
    }
    
    // NULL argv
    if (ft_popen("echo", NULL, 'r') != -1) {
        printf(RED "  - NULL argv should return -1" RESET "\n");
        all_failed_correctly = 0;
    }
    
    // Invalid type
    if (ft_popen("echo", (char*[]){"echo", NULL}, 'x') != -1) {
        printf(RED "  - Invalid type should return -1" RESET "\n");
        all_failed_correctly = 0;
    }
    
    if (all_failed_correctly) {
        print_pass();
    } else {
        print_fail("Some invalid parameters did not return -1");
    }
}

// Test 5: Multiple simultaneous operations
void test_multiple_operations() {
    print_test_header("Multiple Simultaneous Operations");
    
    int fds[3];
    char *args[] = {"echo", "test", NULL};
    
    // Create multiple operations
    for (int i = 0; i < 3; i++) {
        fds[i] = ft_popen("echo", args, 'r');
        if (fds[i] == -1) {
            print_fail("Failed to create operation");
            return;
        }
    }
    
    // Read from all
    int success = 1;
    for (int i = 0; i < 3; i++) {
        char buffer[100] = {0};
        if (read(fds[i], buffer, sizeof(buffer)) <= 0) {
            success = 0;
        }
        close(fds[i]);
    }
    
    // Wait for all children
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
    
    if (success) {
        print_pass();
    } else {
        print_fail("Some operations failed");
    }
}

// Test 6: Pipe functionality (reading output)
void test_pipe_ls() {
    print_test_header("Pipe with ls command");
    
    char *args[] = {"ls", "-la", NULL};
    int fd = ft_popen("ls", args, 'r');
    
    if (fd == -1) {
        print_fail("ft_popen returned -1");
        return;
    }
    
    char buffer[2048] = {0};
    ssize_t total_bytes = 0;
    ssize_t bytes;
    
    while ((bytes = read(fd, buffer + total_bytes, sizeof(buffer) - total_bytes - 1)) > 0) {
        total_bytes += bytes;
    }
    
    close(fd);
    wait(NULL);
    
    if (total_bytes > 0 && (strstr(buffer, "ft_popen") || strstr(buffer, "test_main"))) {
        print_pass();
    } else {
        print_fail("Did not get expected ls output");
    }
}

// Test 7: Non-existent command
void test_nonexistent_command() {
    print_test_header("Non-existent Command");
    
    char *args[] = {"nonexistent_command_xyz", NULL};
    int fd = ft_popen("nonexistent_command_xyz", args, 'r');
    
    if (fd != -1) {
        close(fd);
        int status;
        wait(&status);
        
        // Child should exit with error
        if (WIFEXITED(status) && WEXITSTATUS(status) == 1) {
            print_pass();
        } else {
            print_fail("Child should exit with code 1 for bad command");
        }
    } else {
        // It's also acceptable to return -1
        print_pass();
    }
}

// Test 8: Write mode with actual data processing
void test_write_to_grep() {
    print_test_header("Write Mode with grep");
    
    // Create a pipe to capture grep output
    int capture_pipe[2];
    if (pipe(capture_pipe) == -1) {
        print_fail("Failed to create capture pipe");
        return;
    }
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        close(capture_pipe[0]);
        dup2(capture_pipe[1], STDOUT_FILENO);
        close(capture_pipe[1]);
        
        char *args[] = {"grep", "hello", NULL};
        int fd = ft_popen("grep", args, 'w');
        
        if (fd != -1) {
            write(fd, "hello world\n", 12);
            write(fd, "goodbye world\n", 14);
            write(fd, "hello again\n", 12);
            close(fd);
            wait(NULL);
        }
        exit(0);
    } else {
        // Parent process
        close(capture_pipe[1]);
        char buffer[1024] = {0};
        read(capture_pipe[0], buffer, sizeof(buffer));
        close(capture_pipe[0]);
        wait(NULL);
        
        // Should contain two "hello" lines but not "goodbye"
        if (strstr(buffer, "hello") && !strstr(buffer, "goodbye")) {
            print_pass();
        } else {
            print_fail("grep did not filter correctly");
            printf("  Output: %s\n", buffer);
        }
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    printf(YELLOW "\n╔════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "║    FT_POPEN COMPREHENSIVE TESTER      ║\n" RESET);
    printf(YELLOW "╚════════════════════════════════════════╝\n" RESET);
    
    printf("\nThis tester will check your ft_popen implementation.\n");
    printf("Make sure ft_popen.c is compiled with this test.\n\n");
    
    // Run all tests
    test_basic_read();
    test_basic_write();
    test_invalid_params();
    test_pipe_ls();
    test_multiple_operations();
    test_nonexistent_command();
    test_write_to_grep();
    test_chaining_commands();
    
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
