.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the number of elements to use is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
    # Prologue
addi t0 x0 1#t0 equals one to do a comparison
blt a2 t0 RETURN1

blt a3 t0 RETURN2#checks both strdies if less than one,
blt a4 t0 RETURN2#if less than one, then return the second thing

addi t1 x0 4#set a variable equal to 4
mul a3 a3 t1#now our strides are now the correct bytes long for a skip
mul a4 a4 t1

add t2 x0 x0#this is our index

add t3 x0 x0#we will be adding the dot product to this before moving it to a0

loop_start:
bge t2 a2 loop_end #if we have used enough elements, then end

lw t4 0(a0)#load the number from the pointer
lw t5 0(a1)

mul t6 t4 t5 #multiply the two number together and store it

add t3 t3 t6#add our multiplication to the rest of the summation

addi t2 t2 1#add the index by one
add a0 a0 a3#add the correct number of bytes to get the next value we want wrt the stride
add a1 a1 a4
j loop_start
loop_end:
add a0 x0 t3

    # Epilogue


    jr ra
    
    
RETURN1:
li a0 36
j exit

RETURN2:
li a0 37
j exit