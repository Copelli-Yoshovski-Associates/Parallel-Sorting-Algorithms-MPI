# Parallel Sorting Algorithms in MPI
> Bitonic Sort, Odd-Even Transposition, and Parallel Quicksort

Parallel execution of three sorting algorithms implemented with MPI: Bitonic Sort, Odd-Even Transposition, and Parallel Quicksort. The project showcases the graphical representation of the sorting process using the Allegro library. 
This project was developed for the course of Parallel Algorithms and Distributed Systems (University of Calabria) in July 2021

## Contributors
> [Francesco Copelli](https://www.linkedin.com/in/francesco-copelli/) (Back-End and EmbASP Integration) <br>
> [Stefan Yoshovski](https://www.linkedin.com/in/stefan-yoshovski/) (UI and ASP Encode)

## Table of Contents
- [Used Technologies](#used-technologies)
- [How to run the project?](#how-to-run-the-project)
- [Video Demo and Snapshots](#video-demo-and-snapshots)
- [Bitonic Sort](#bitonic-sort)
- [Parallel Quicksort](#parallel-quicksort)
- [Odd Even Transposition](#odd-even-transposition)

## Used Technologies
- MPI (Message Passing Interface) for parallel execution
- Allegro 5 library for graphical representation of the sorting process
- Bitonic Sort, Odd-Even Transposition, and Parallel Quicksort algorithms implemented with MPI
- C++


## How to run the project?

Pre-requirements: Having allegro5 installed in the same directory of the project
<details><summary>Step-By-Step Guide to install allegro5</summary>
  
    git clone https://github.com/liballeg/allegro5.git
  
    cd allegro5
    mkdir build
    cd build
    cmake ..
    make
    sudo make install
</details>

To compile the program on Linux: 
`make`

To run the program: 
`mpirun -np <num_processors> ./sorting.out <size_sequence> <graphic>`

example: `mpirun -np 4 ./ sorting.out 1024 1` 



## Video Demo and Snapshots
- [Complete Video Demo](https://youtu.be/fi6frKzaLYc)
- [Bitonic Sort Video Demo](https://youtu.be/fi6frKzaLYc?t=0)
- [Parallel Quicksort Video Demo](https://youtu.be/fi6frKzaLYc?t=10)
- [Odd Even Transposition Video Demo](https://youtu.be/fi6frKzaLYc?t=45)


## Bitonic Sort
[![Bitonic Sort](https://github.com/Copelli-Yoshovski-Associates/Parallel-Sorting-Algorithms-MPI/blob/main/resources/bitonic_sort.png)](https://youtu.be/fi6frKzaLYc?t=0)

## Parallel Quicksort
[![Parallel Quicksort](https://github.com/Copelli-Yoshovski-Associates/Parallel-Sorting-Algorithms-MPI/blob/main/resources/qicksort.png)](https://youtu.be/fi6frKzaLYc?t=10)

## Odd Even Transposition
[![Odd Even Transposition](https://github.com/Copelli-Yoshovski-Associates/Parallel-Sorting-Algorithms-MPI/blob/main/resources/odd_even_transposition.png)](https://youtu.be/fi6frKzaLYc?t=45)

