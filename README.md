# Priotitized Dining Philosophers
A variation of the Dining Philosophers problem with a ticket system. Each philosopher is assigned a ticket value, and the one with the next higher ticket value gets to dine next, even if they have to wait for one or two forks to be released.

## Prerequisites
- Qt 6 library 
  
## How to Run

1. Clone this repository:
```
git clone <repo-link>
```
2. Run the project:
```
qmake6 diningPhilosophers.pro
make
./diningPhilosophers <numOfThreads>
```
