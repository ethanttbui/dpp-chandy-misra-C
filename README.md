# chandy-misra-C

C implementation of Chandy/Misra solution to the Dinning Philosophers problem

# Configuration

**System requirement:** Ubuntu 14.04 
  
**Compile:**

    gcc DPP.c -o DPP -Wall -pthread

**Run:**  

    ./DPP N S T

N (int): number of philosophers  
S (int): a seed for generating a sequence of random numbers, representing the amount of time philosophers spend on eating and thinking 
T (int): the amount of time (in seconds) for which the simulation will run  

# Problem Statement

Five philosophers sit at a round table with bowls of spaghetti. Forks are placed between each pair of adjacent philosophers.  
Each philosopher must alternately think and eat. However, a philosopher can only eat spaghetti when they have both left and right forks. Each fork can be held by only one philosopher and so a philosopher can use the fork only if it is not being used by another philosopher. After an individual philosopher finishes eating, they need to put down both forks so that the forks become available to others. A philosopher can take the fork on their right or the one on their left as they become available, but cannot start eating before getting both forks.  
  
Eating is not limited by the remaining amounts of spaghetti or stomach space; an infinite supply and an infinite demand are assumed.  
  
The problem is how to design a discipline of behavior (a concurrent algorithm) such that no philosopher will starve; i.e., each can forever continue to alternate between eating and thinking, assuming that no philosopher can know when others may want to eat or think.

Source: [Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem "Wikipedia")

# Chandy/Misra Solution

1. For every pair of philosophers contending for a resource, create a fork and give it to the philosopher with the lower ID (n for agent Pn). Each fork can either be dirty or clean. Initially, all forks are dirty.
2. When a philosopher wants to use a set of resources (i.e. eat), said philosopher must obtain the forks from their contending neighbors. For all such forks the philosopher does not have, they send a request message.
3. When a philosopher with a fork receives a request message, they keep the fork if it is clean, but give it up when it is dirty. If the philosopher sends the fork over, they clean the fork before doing so.
4. After a philosopher is done eating, all their forks become dirty. If another philosopher had previously requested one of the forks, the philosopher that has just finished eating cleans the fork and sends it.

Source: [Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem "Wikipedia")

