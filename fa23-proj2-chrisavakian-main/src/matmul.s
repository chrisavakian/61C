.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*)  is the pointer to the start of m0
#   a1 (int)   is the # of rows (height) of m0
#   a2 (int)   is the # of columns (width) of m0
#   a3 (int*)  is the pointer to the start of m1
#   a4 (int)   is the # of rows (height) of m1
#   a5 (int)   is the # of columns (width) of m1
#   a6 (int*)  is the pointer to the the start of d
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 38
# =======================================================
matmul:
    # Error checks
    bne a2 a4 mismatch
    ble a4 x0 mismatch
    ble a5 x0 mismatch
    ble a1 x0 mismatch 
    ble a2 x0 mismatch
    
    #TODO:Need to store every value. Not storing t registers since they are temporary
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
    #Done storing every value
    
    #Move all of the a into the s
    mv s0 a0
    mv s1 a1
    mv s2 a2
    mv s3 a3
    mv s4 a4
    mv s5 a5
    mv s6 a6
    
    #This will be a constant for 4 bytes
    addi s9 x0 4
    
    #These will be our counter variables for outer and inner
    add s7 x0 x0
    add s8 x0 x0
    
outer_loop_start:
#Check if we are done with the outer loop
beq s7 s1 outer_loop_end
#Every time we are here we need to set the inner counter var back to 0 for the next loop
add s8 x0 x0

inner_loop_start:
#Check if we are done with the inner loop
beq s8 s5 inner_loop_end

#This now means a0 is at the correct spot
add t0 x0 x0
mul t0 s7 s2
mul t0 t0 s9
#t0 now contains the correct number of bytes we need to move the first pointer at
add a0 s0 t0


#This now means a1 is at the correct spot
add t0 x0 x0
mul t0 s8 s9
#t0 now points to the correct item in the first row that will give us the column
add a1 s3 t0

#Assign the rest of the values to the correct spot
add a2 s2 x0
addi a3 x0 1
add a4 s5 x0

#Call dot function
jal ra dot

#Put the result (a0) into the correct spot
add t0 x0 x0
mul t0 s7 s5
add t0 t0 s8
mul t0 t0 s9
add t0 t0 s6

sw a0 0(t0)

addi s8 s8 1
j inner_loop_start
inner_loop_end:
#Add one to the counter variable
addi s7 s7 1

#Jump back to outer loop
j outer_loop_start

outer_loop_end:
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

mismatch:
li a0 38
j exit