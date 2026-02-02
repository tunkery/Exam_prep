#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

int	picoshell(char **cmds[])
{
	pid_t	pid; // bunu fork icin kullaniyoruz.
	int		fd[2]; // bunu da o an kullanmak istedigimiz pipe icin.
	int		last_fd = -1; // onceden kullandigimiz pipe ne vermis buna kaydediyoruz.
	int		i = 0;
	int		ret = 0;
	int		status;

	// cmds'nin icinde bizim run yapmamiz gereken bir veya birden fazla hem command hem de
	// argumanlar var. 

	// burada pipe'larin olmasinin sebebi bizim execvp'yi calistirdigimizda onu child process
	// ile yapmak zorunda olmamiz. Mesela fork ile execvp'yi run yaptigimizda eger program hata
	// verirse veya o fork'a bir bilgi iletmemiz gerekirse orada pipe'lari kullanmamiz lazim.

	while (cmds[i]) // butun argumanlari alana kadar while loop'luyoruz.
	{
		if (cmds[i + 1] && pipe(fd) == -1) // altta kullanacagimiz pipe'i aciyoruz.
		{
			if (last_fd != -1)
				close(last_fd);
			return (1);
		}
		pid = fork(); // child process'i aciyoruz burada.
		if (pid == -1) // child process fail yediyse pipe'lari kapatiyoruz. 
		{
			if (last_fd != -1)
				close(last_fd);
			if (cmds[i + 1])
			{
				close(fd[0]);
				close(fd[1]);
			}
			return (1);
		}
		if (pid == 0) // child process okeyse devamini getiriyoruz.
		{
			if (last_fd != -1) // eger bu ilk pipe degil ise onceki pipe'in input'unu kopyaliyoruz.
			{
				if (dup2(last_fd, STDIN_FILENO) == -1)
					exit(1);
				close(last_fd); // pipe'in read tarafini kapatiyoruz.
			}
			if (cmds[i + 1]) // eger son arguman degilse bunu yapiyoruz.
			{
				close(fd[0]); // read tarafi kapatiliyor.
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(1);
				close(fd[1]); // duplicate yaptiktan sonra write tarafini da kapatiyoruz.
			}
			execvp(cmds[i][0], cmds[i]); // commandi execute ediyoruz.
			exit(1); // eger execvp fail yerse exit yapiyor.
		}
		if (last_fd != -1) // eger ilk command degilse onceki pipe'in read tarafini kapatiyoruz.
			close(last_fd); // cunku parent'a lazim degil.
		if (cmds[i + 1])
		{
			// eger bir tane daha command varsa parent'in write tarafini kapatiyoruz
			// ve diger loop icin read tarafini kaydediyoruz last_fd ile.
			close(fd[1]);
			last_fd = fd[0];
		}
		i++;
	}
	while (wait(&status) > 0)
	{
		if ((WIFEXITED(status) && WEXITSTATUS(status) != 0) || WIFSIGNALED(status))
			ret = 1;
	}
	return (ret);
}
