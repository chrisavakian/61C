.globl classify

.text
# =====================================
# COMMAND LINE ARGUMENTS
# =====================================
# Args:
#   a0 (int)        argc
#   a1 (char**)     argv
#   a1[1] (char*)   pointer to the filepath string of m0
#   a1[2] (char*)   pointer to the filepath string of m1
#   a1[3] (char*)   pointer to the filepath string of input matrix
#   a1[4] (char*)   pointer to the filepath string of output file
#   a2 (int)        silent mode, if this is 1, you should not print
#                   anything. Otherwise, you should print the
#                   classification and a newline.
# Returns:
#   a0 (int)        Classification
# Exceptions:
#   - If there are an incorrect number of command line args,
#     this function terminates the program with exit code 31
#   - If malloc fails, this function terminates the program with exit code 26
#
# Usage:
#   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>
classify:
    #Prologue
    addi t0 x0 5
    bne a0 t0 incorrect_num
    
    addi sp sp -52
    sw s0 0(sp)
    sw s1 4(sp)
    sw s2 8(sp)
    sw s3 12(sp)
    sw s4 16(sp)
    sw s5 20(sp)
    sw s6 24(sp)
    sw s7 28(sp)
    sw s8 32(sp)
    sw s9 36(sp)
    sw s10 40(sp)
    sw s11 44(sp)
    sw ra 48(sp)
    
    #Save arguments
    mv s0 a0    #argc, num of arguments provided                a0 -> s0
    mv s1 a1    #argv**, a pointer to an arrray of strings      a1 -> s1
    mv s2 a2    #if 0, print classification, otherwise nothing  a2 -> s6
    
# Read pretrained m0
    #allocate space for pointers a1 and a2
    addi a0 x0 8    #num of bytes for pointer, save num rows and cols
    jal malloc
    beq a0 x0 malloc_fail
    mv s3 a0  #save allocated pointer for num rows and cols 
    
    #call to read_matrix
    addi t0 x0 4 # t0 = 4
    mv a1 a0   #a1 = rows of m0
    add a2 a1 t0 #a2 = cols pointer of m0
    lw a0 4(s1) #load filepath of m0 into a0
    jal read_matrix
    mv s4 a0 #store matrix pointer. s4 = m0

# Read pretrained m1
    #allocate space for pointers a1 and a2
    addi a0 x0 8     #num of bytes for pointer, save num rows and cols
    jal malloc
    beq a0 x0 malloc_fail
    mv s5 a0  #save allocated pointer for num rows and cols
    
    #call to read_matrix
    addi t0 x0 4 # t0 = 4
    mv a1 a0    #a1 = rows pointer of m1
    add a2 a1 t0 #a2 = cols pointer of m1
    lw a0 8(s1) #load filepath of m1 into a0
    jal read_matrix
    mv s6 a0 #store return pointer of read_matrix. s6 = m1

# Read input matrix
     #allocate space for pointers a1 and a2
    addi a0 x0 8    #num of bytes for pointer, save num rows and cols
    jal malloc
    beq a0 x0 malloc_fail
    mv s7 a0  #move allocated pointer
    
    #call to read_matrix
    mv a1 a0    #a1 = rows pointer of input
    addi a2 a1 4 #a2 = cols pointer of input
    lw a0 12(s1) #load filepath of input into a0
    jal read_matrix
    mv s8 a0 #store return pointer of read_matrix. s8 = input
       

# Compute h = matmul(m0, input)
    #allocate space for h
    lw t0 0(s3) # t1 = number of rows of m0
    lw t1 4(s7) #t2 = number of cols of input
    mul a0 t1 t0 # (dimension t0 × t1)
    addi t2 x0 4 #each space is 4 bytes
    mul a0 a0 t2 # size in bytes, 4 bytes each
    jal malloc
    beq a0 x0 malloc_fail
    mv s9 a0 # s9 = h
    
    #Set values for matmul call
    mv a0 s4 #a0 = s4
    lw a1 0(s3) # a1 = number of rows of m0
    lw a2 4(s3) #a2 = number of cols of m0
    
    mv a3 s8     #a3 = pointer to start of input
    lw a4 0(s7)  #number of rows of input
    lw a5 4(s7)  #number of cols of input
    mv a6 s9 # a6 = h
    
    #call matmul
    addi sp sp -4
    sw a6 0(sp)
    jal matmul
    lw a6 0(sp)
    addi sp sp 4

# Compute h = relu(h)
    mv a0 a6 # a0 = h. a0 is argument for relu
    #compute num of integers in h
    lw t0 0(s3) # t1 = number of rows of h
    lw t1 4(s7) #t2 = number of cols of h
    mul a1 t1 t0 # a1 = num of integers in h
    #call relu
    jal relu
    
# Compute o = matmul(m1, h)
    #allocate memory for o
    lw t0 0(s5) #t0 = num rows of m1
    lw t1 4(s7) #t1 = num cols of h
    mul a0 t1 t0 # a0 = (dimension t0 × t1)
    addi t2 x0 4 #size of each element = 4
    mul a0 a0 t2 # size in bytes, 4 bytes each
    jal malloc
    beq a0 x0 malloc_fail
    mv s10 a0 # s10 = o
    mv a6 s10 # a6 = o
    
    #set arguments for matmul
    mv a0 s6 #a0 = s6
    lw a1 0(s5) #a1 = num rows of m1
    lw a2 4(s5) #a2 = num cols of m1
    
    mv a3 s9 #a3 = h
    lw a4 0(s3) #a4 = num rows of h
    lw a5 4(s7) #a5 = num cols of h
    
    #call matmul
    addi sp sp -4
    sw a6 0(sp)
    jal matmul
    lw a6 0(sp)
    addi sp sp 4

# Write output matrix o
    lw a0 16(s1)    #a0 = output file path, char pointer
    mv a1 a6       #a1 = o
    lw a2 0(s5)     #a2 = rows of o
    lw a3 4(s7)     #a3 = cols of o
    
    jal write_matrix
    
# Compute and return argmax(o)
    lw t0 0(s5) #t0 = rows of o
    lw t1 4(s7) #t1 = cols of o
    mul t3 t0 t1 #t3 = rows * cols
    
    #arguments for argmax
    mv a0 s10   #a0 = o
    add a1 t3 x0 #a1 = size of o
    
    #call argmax
    addi sp sp -4
    sw ra 0(sp)
    jal argmax
    lw ra 0(sp)
    addi sp sp 4
    
    #save return value of argmax
    mv s11 a0
   
    # If enabled, print argmax(o) and newline
    bne s2 x0 free_mem
    jal print_int
    li a0 '\n'
    jal print_char
    
free_mem: 
    #Free space allocated for row and col of m0
    mv a0 s3
    jal free
    
    #Free space allocated for m0
    mv a0 s4
    jal free
    
    #Free space allocated for row and col of m1
    mv a0 s5
    jal free
    
    #Free space allocated for m1
    mv a0 s6
    jal free
    
    #Free space allocated for row and col of input
    mv a0 s7 
    jal free
    
    #Free space allocated for input
    mv a0 s8
    jal free
    
    #Free space allocated for h
    mv a0 s9
    jal free
    
    #Free space allocated for o
    mv a0 s10
    jal free

    #Load return value into a0
    mv a0 s11
    
    #load saved registers
    lw s0 0(sp)
    lw s1 4(sp)
    lw s2 8(sp)
    lw s3 12(sp)
    lw s4 16(sp)
    lw s5 20(sp)
    lw s6 24(sp)
    lw s7 28(sp)
    lw s8 32(sp)
    lw s9 36(sp)
    lw s10 40(sp)
    lw s11 44(sp)
    lw ra 48(sp)
    addi sp sp 52
    jr ra
    
incorrect_num:
   li a0 31
   jal exit
   
malloc_fail:
   li a0 26
   jal exit