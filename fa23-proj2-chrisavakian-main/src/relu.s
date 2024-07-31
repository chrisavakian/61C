.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
    # Prologue
add t2, x0,x0 # We define a counter variable to 0 

bge x0, a1, Else
    j loop_start

Else:
    li a0 36
    j exit

loop_start:
bge t2 a1 loop_end

lw a2 0(a0) #take the word from t5, t6 now contains the numer

bge a2 x0 loop_continue #if t6 is larger than or equal to 0, then forget about it

add a2 x0 x0 #otherwise set t6 to 0
sw a2 0(a0) #save that word back into t5

loop_continue:
addi t2 t2 1
addi a0 a0 4
j loop_start


loop_end:


    # Epilogue
    ret

    jr ra