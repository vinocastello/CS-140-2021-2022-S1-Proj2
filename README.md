# CS 140 A.Y. 2021-2022 S1 Project 2: File server using thread

### Author:
- De Castro, Hans Gabriel H. (2019-03184)
## Deliverables
1. file_server.c
- source code for the implementation
2. cs140proj2docu - 201903184.pdf
- Written documentation with link to video documentation
3. CS 140 Proj 2 - 201903184.mp4
- Video documentation
4. README.md
- contains brief summary of project and some instructions
## Environment
The Operating system that I used in creating this project is Windows 10 Pro. I wrote the code in Visual Studio Code v1.63.2. However, I compiled and ran the file_server on WSL. The distribution that I installed in WSL is Ubuntu. The executable file_server is located in the directory /home/hans/CS_140/Proj2. The text files commands.txt, read.txt, and empty.txt are also found here.
## NOTE
I was only able to implement and test until level 3. 
This means that this "file server" can continously receive user request and each command spawns a worker thread that performs the said request.
These threads work concurrently. However, I have not yet included mutex locks to take into account data races.
## Commands
1. write <file path or name> <string input>
- writes string input to desired file path or file name
2. read <file path or name>
- reads contents of target file and writes it into read.txt
3. empty <file path or name>
- reads the contents of target file and writes it into empty.txt
- empties the target file after reading
4. thread_count
- prints the current value of thread_counter to terminal
## Implementation
### Outline
1. main function
- This serves as the "master" thread
- This is where the array of threads is initialized
- continuously accepts input
- will break only when you enter "exit"
- parses input
- spawns thread to their appropriate functions with arguments
2. save_command
- This function writes all incoming input to commands.txt attached with timestamp
3. worker_write
- Thread for write command is spawned here
- appends the input string to specified file
4. worker_read
- Thread for read command is spawned here
- calls readfile to read the contents of specified file and append it to read.txt
5. worker_empty
- Thread for empty command is spawned here
- calles readfile to read contents of specified file and append it to empty.txt
- empties the specified file
6. readfile
- This function reads the contents of target file and writes it to either read.txt if the command is read or empty.txt if the command is empty
7. random_sleep
- This function has two modes
- the first mode is to make calling thread sleep for 1s with 80% probability of happening or to make it sleep for 6s with 20% probability of happening
- the second mode is to make calling thread sleep for 7,8,9, or 10 seconds. Each have 25% probability of happening
- it calls get_random to generate random number
8. get_random
- this function was provided in uvle
- this function generates a random number in the range [0,100)
## Testing
Note that the source code is file_server.c and please create these empty text files before starting:
1. commands.txt
2. read.txt
3. empty.txt
Note as well that these files are assumed to be in the same directory as the executable file file_server
## Compiling the program
Run this command in shell
```
$ gcc file_server.c -o file_server -lpthread
```
## Running the program
```
./file_server
```
## Sample test of commands (./file_server was already done)
```
$ write test.txt HELLO
```
or
```
$ write /home/hans/CS_140/Proj2/test.txt HELLO
```
```
$ read test.txt
```
```
$ empty test.txt
```
```
$ thread_count
```
- file_server can read/write from/to files in same directory as the executable
- file_server can also read/write from/to files in other directories as long as complete path of the file is given

## Video Documentation Link
[CS 140 Proj 2 - 201903184](https://drive.google.com/file/d/1dDvq2kthdnKHzEM3irLlxAcxK36rF_mR/view?usp=sharing)
