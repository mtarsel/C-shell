C-shell
===========

Type help for more info.

###Features

 jobs - prints out the list of running background jobs.
 
 kill [process_id] - kills background process_id to kill a running background job.
 
 history - display a record of the last 100 commands typed.
 
 ![int] - execute [int] command in history array
 
 cd [location] - change directory
 
 sometext < infile.txt > outfile.txt - create a new process to run sometext and assign STDIN for the new process to infile and STDOUT for the new process to outfile
 
 exit - exit or quit shell.
 
 Append & to the end of any command to run it in the background.
 
 
###Installation

```bash
sudo apt-get install build-essential
sudo apt-get install lib64readline-gplv2-dev(For 64 Bit)
sudo apt-get install libreadline-gplv2-dev(For 32 bit)
make
```

http://web2.clarkson.edu/class/cs444/assignments/shell/
