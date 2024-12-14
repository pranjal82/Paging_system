##  KNOWN ERROR CASES  WEAKNESSES WITH THE PROGRAM

1) The program provides a basic structure of a paging system however it doesn't support multi-level paging.

2) Since we are not freeing up the data in the memory, it can happen that a physical address say X of process P store some value V and now P is killed. Moreover, some other process say Q is run and it's logical address on translation gives X as the physical address, now if we print this address, we get the value V which is different from the one given in tc i.e. 0 

3) The program is an abstraction of physical memory, I'm using it as a map.

# SOURCES

https://www.geeksforgeeks.org/file-handling-c-classes/ : For File Handling commands
https://www.w3schools.com/cpp/cpp_date.asp : Displaying current date and time
TextBook Andrew Tannebaum : Page replacement Algorithms

