.globl argmax

.text
# =================================================================
# FUNCTION: Given a int array, return the index of the largest
#   element. If there are multiple, return the one
#   with the smallest index.
# Arguments:
#   a0 (int*) is the pointer to the start of the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   a0 (int)  is the first index of the largest element
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# =================================================================
argmax:
    # Prologue
    add t0 x0 x0 #i = 0 (counter)
    add t1 x0 x0 #Maxindex = 0
    lw t2 0(a0)#Max val = a0[0]

bge x0, a1, Else
    j loop_start

Else:
    li a0 36
    j exit


loop_start:
bge t0 a1 loop_end#if i >= a1 then end

lw t3 0(a0) #get value out of the value we want to check

ble t3 t2 loop_continue # if largest value >= curr value then continue loop but skip

add t2 t3 x0 #t3 is now our largest value
add t1 t0 x0 #current index is now our largest index

loop_continue:
addi t0 t0 1 #increment pointer by 1
addi a0 a0 4 #go to the next word
j loop_start

loop_end:
    # Epilogue
    add a0 t1 x0 #move index to a0

    jr ra