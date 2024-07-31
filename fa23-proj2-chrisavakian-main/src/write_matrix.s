.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fwrite error or eof,
#     this function terminates the program with error code 30
# ==============================================================================
write_matrix:

    # Prologue
    addi sp sp -24
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
    
    mv s1 a1 #copy pointer to matrix so that it can be changed
    mv s2 a2 #save num of rows
    mv s3 a3 #save num of cols
    
    #open file
    addi a1 x0 1 #a1 is the write permission, we set to 1 so we can write to file
    jal fopen  #opening file, a0 is filename and a1 permission bit set to 1
    blt a0 x0 open_error #if fopen fails, we exit function
    mv s0 a0 #save file discriptor
    
    #Put num of rows and cols in the stack 
    addi sp sp -4
    sw s2 0(sp)
   
    #Write number of rows and columns to file
    mv a0 s0 #load discriptor to a0
    addi a2 x0 1 #number of elements to write
    addi a3 x0 4  #size of elements, i.e 4 bytes
    mv a1 sp  #a1 points to the buffer containing what we want to write
    jal fwrite
    blt a0 x0 write_error #if -1 then error
    
    sw s3 0(sp)
    
    #Write number of rows and columns to file
    mv a1 sp  #a1 points to the buffer containing what we want to write
    mv a0 s0 #load discriptor to a0
    addi a2 x0 1 #number of elements to write
    addi a3 x0 4  #size of elements, i.e 4 bytes
    jal fwrite
    blt a0 x0 write_error #if -1 then error
    #return registers to previous location
    addi sp sp 4
   
    #write matrix to file
    
load_matrix:
    mv a0 s0 #load file discriptor to a0
    mv a1 s1 #load matrix into a1
    mul s4 s2 s3 #total number elements in matrix
    mv a2 s4  #a2 is the arguement for fwrite
    addi a3 x0 4  #size of elements, i.e 4 bytes
    jal fwrite
    blt a0 x0 write_error #if -1 then error
    bne a0 s4 load_matrix #if a2 elements werent written then loop
    
    #close file
    mv a0 s0 #load file discriptor to a0
    jal fclose
    bne a0 x0 close_error
       
    
    # Epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    addi sp sp 24

    jr ra

open_error:
    li a0 27
    j exit
write_error:
    li a0 30
    j exit
close_error:
    li a0 28
    j exit