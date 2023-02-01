In this assignment, it is asked to find a deadlock-free solution to the Dining Philosophers problem. Monitor-based solution is used in the code that ensures if both chopsticks are free, then take them up, and the other two philosophers should wait. 
•	Initially, all philosophers begin in a state of thinking.
•	When a philosopher wants to enter the critical section, it checks left and right philosophers in test, if they are thinking, it switches to hungry state. 
•	It locks the two chopsticks and critical section mutexes, and switches to eating state.
•	After eating for a while, it releases mutexes and switches to thinking state.
•	According to the calculated dining time, it continues to enter the critical section and exits when it is completely finished.
•	Meanwhile, while one philosopher thinks, another philosopher continues to queue up hungry. The code terminates when they are all done.

RUNNING:
type make
It can take some time to terminate the code. When all dining times complated it will finis by showing the dining time message. 
Current eatig philosopher will be written on the console. 
