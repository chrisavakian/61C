.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================
read_matrix:
    addi sp sp -52
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
    sw s5 24(sp)
    sw s6 28(sp)
    sw s7 32(sp)
    sw s8 36(sp)
    sw s9 40(sp)
    sw s10 44(sp)
    sw s11 48(sp)
    
    #Move relevant values
    mv s0 a0
    mv s1 a1
    mv s2 a2
    add a1 x0 x0
    add a2 x0 x0
    
    #Open the file
    mv a0 s0
    add a1 x0 x0
    jal ra fopen
    
    #Error Checking
    addi t0 x0 -1
    beq a0 t0 fopen_err
    
    mv s3 a0
    
    #Read Rows
    mv a0 s3
    mv a1 s1
    addi a2 x0 4
    jal ra fread
    
    #Error Checking
    addi t0 x0 4
    bne t0 a0 fread_err
    
    #Read Cols now
    mv a0 s3
    mv a1 s2
    addi a2 x0 4
    jal ra fread
    
    #Error Checking
    addi t0 x0 4
    bne t0 a0 fread_err
    
    #At this point the row and column are stored in 0 and 4 of the buffer of s3
    #Get those values
    lw s4 0(s1)
    lw s5 0(s2)
    
    #Malloc Time
    addi t0 x0 4
    mul s6 s4 s5
    mul s6 s6 t0
    mv a0 s6
    jal ra malloc
    
    #Error Checking
    beq a0 x0 malloc_err
    
    #Store allocated stuff at s7
    mv s7 a0
    
    #Read the rest
    mv a0 s3
    mv a1 s7
    mv a2 s6
    jal ra fread
    
    #Error Checking
    blt a0 s6 fread_err
    
    mv a0 s3
    jal ra fclose
    
    #Error Checking
    addi t0 x0 -1
    beq a0 t0 fclose_err
    
    mv a0 s7
    
    #Return Every Value
    lw s11 48(sp)
    lw s10 44(sp)
    lw s9 40(sp)
    lw s8 36(sp)
    lw s7 32(sp)
    lw s6 28(sp)
    lw s5 24(sp)
    lw s4 20(sp)
    lw s3 16(sp)
    lw s2 12(sp)
    lw s1 8(sp)
    lw s0 4(sp)
    lw ra 0(sp)
    addi sp sp 52
    #Return
    ret

fopen_err:
    li a0 27
    j exit
fread_err:
    li a0 29
    j exit
malloc_err:
    li a0 26
    j exit
fclose_err:
    li a0 28
    j exit