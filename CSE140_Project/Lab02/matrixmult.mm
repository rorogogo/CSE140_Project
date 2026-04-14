# Traditional Matrix Multiply program
		.data
matrix_a:
	.word  1,  2,  3,  4,  5,  6,  7,  8,  9, 10
	.word  2,  3,  4,  5,  6,  7,  8,  9, 10,  1
	.word  3,  4,  5,  6,  7,  8,  9, 10,  1,  2
	.word  4,  5,  6,  7,  8,  9, 10,  1,  2,  3
	.word  5,  6,  7,  8,  9, 10,  1,  2,  3,  4
	.word  6,  7,  8,  9, 10,  1,  2,  3,  4,  5
	.word  7,  8,  9, 10,  1,  2,  3,  4,  5,  6
	.word  8,  9, 10,  1,  2,  3,  4,  5,  6,  7
	.word  9, 10,  1,  2,  3,  4,  5,  6,  7,  8
	
matrix_b:
	.word 1,  2,  3,  4,  5,  6,  7,  8,  9
	.word 2,  3,  4,  5,  6,  7,  8,  9,  1
	.word 3,  4,  5,  6,  7,  8,  9,  1,  2
	.word 4,  5,  6,  7,  8,  9,  1,  2,  3
	.word 5,  6,  7,  8,  9,  1,  2,  3,  4
	.word 6,  7,  8,  9,  1,  2,  3,  4,  5
	.word 7,  8,  9,  1,  2,  3,  4,  5,  6
	.word 8,  9,  1,  2,  3,  4,  5,  6,  7
	.word 9,  1,  2,  3,  4,  5,  6,  7,  8
	.word 8,  7,  6,  5,  4,  3,  2,  1,  9   
	
matrix_c:
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,    
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,    
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0,  
	.word   0,  0,  0,  0,  0,  0,  0,  0,  0  
	
	
m:	.word 10 #columns
n:	.word 9  #rows

nline:  .string "\n"			#Define new line string
space:	.string " "
msga: 	.string "Matrix A is: \n"
msgb: 	.string "Matrix B is: \n"
msgc: 	.string "Matrix C=A*B is: \n"

		.text
		.globl main
main:

		la	s0, m	
		lw	s0, 0(s0)
		la	s1, n
		lw	s1, 0(s1)
		la	s2, matrix_a
		la	s3, matrix_b
		la	s4, matrix_c

		la	a0, msga
		la 	a1, matrix_a
		add	a2, s0, zero	
		add	a3, s1, zero		
		jal	PRINT_MAT 
		la	a0, msgb
		la 	a1, matrix_b
		add	a2, s1, zero	
		add	a3, s0, zero		
		jal	PRINT_MAT 
		
		

# Your CODE HERE		

#before anything, I need to find out how the data is stored, how far apart is each element in the matrix? How far apart is each row/column from the other? 
#I need to do some testing to find out how the data is laid out.
#Loop 1 and 2 can just be checking if the value is less, and then incrementing the register value and jumping to the next loop, loop 3 has to check the value, increment, and do the actual logic using the registers from 1/2.


#First create a loop for integrating the first i value
#Second create a loop for integrating the j value
#Third create a loop for the k value
#Within the third loop, use the current value i/j/k, to find the value of each # at each point.

#Checklist of things I need to do:
#1.Initialize registers to hold the counters (i, j, k)
#2.create a loops that will increment and branch if less than M/N
#3 Use pointer manipulation to find the value at that increment
#4 Must use slli for... I'm guessing multiplication? note: as I'm leaving lab, it's because it can be used to easily multiply by 4 (the word size)

#Pseudocode:
#Start -> (itialize values)
#Loop 1: check if i < M, if it is i += 1 and jump to loop 2
#Loop 2: check if j < M, if it is j += 1 and jump to loop 3
#Loop 3: check if k < N, if it is k += 1 and do the following:
#element of c (i,j) = element of a(i,k) * element of b(k,j)

#Open "temporaries" I can use: t0-t2(x5-x7), and t3-t6(x28-x31)
#The "global pointer" is x3
# It is stated that pseudo instructions are allowed
#Here are some helpful ones I found I might use:
#https://stackoverflow.com/questions/65006052/how-do-i-write-not-operation-for-the-risc-v-assembly-language

#s2 is a, s3 is b, s4 is c


#for (i = 0; i < M; i++)
#	for (j = 0; j < M; j++)
#		for (k = 0; k < N; k++)
#			C[i][j] = C[i][j] + A[i][k] x B[k][j];


start:
	li t0, 0          # i = 0

#Loop 1 (i loop)
LP1:
	bge t0, s1, finish	
	li t1, 0           #set j back to 0 after loop
#Loop 2 (j loop)
LP2:
	bge t1, s1, nexti
	
	li t2, 0           #set k back to 0 after loop
#Loop 3 (k loop)
LP3:
	bge t2, s0, nextj 
	#A[i][k]
	# address = baseA + 4*(i*m + k)
	mul t3, t0, s0     # i * m
	add t3, t3, t2     # i*m + k
	slli t3, t3, 2     # multiply by 4
	add t3, s2, t3     # add base address of A
	lw t4, 0(t3)       # t4 = element
	#B[k][j]
	# address = baseB + 4*(k*n + j)
	mul t3, t2, s1     # k * n
	add t3, t3, t1     # k*n + j
	slli t3, t3, 2
	add t3, s3, t3
	lw t5, 0(t3)       # t5 = B element
	#C[i][j]
	# address = baseC + 4*(i*n + j)
	mul t3, t0, s1     # i * n
	add t3, t3, t1     # i*n + j
	slli t3, t3, 2
	add t3, s4, t3
	lw t6, 0(t3)       # current C[i][j]

	# multiply and accumulate
	mul t4, t4, t5     # A * B
	add t6, t6, t4     # C += product
	sw t6, 0(t3)       # store back

	# increment k
	addi t2, t2, 1
	j LP3

#k done increase j
nextj:
	addi t1, t1, 1
	j LP2

#j done increase i
nexti:
	addi t0, t0, 1
	j LP1

finish:

# End CODE

		la	a0, msgc
		la 	a1, matrix_c
		add	a2, s1, zero	
		add	a3, s1, zero
		jal	PRINT_MAT 

#   Exit
		li	 a7,10
    		ecall


PRINT_MAT:	li	a7,4
		ecall
		addi a4,x0,0	
PL4:	bge	a4,a3,PL1
		addi a5,x0,0
PL3:	bge	a5,a2,PL2

		lw	a0,0(a1)
		li	a7,1
		ecall
		la	a0,space
		li	a7,4
		ecall
		addi a1,a1,4
		addi a5,a5,1
		b 	PL3

PL2:	addi	a4,a4,1
		la	a0,nline
		li	a7,4
		ecall
		b	PL4
PL1:	jr	ra
