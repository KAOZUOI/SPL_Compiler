.data
_prompt: .asciiz "Please enter an integer: "
_ret: .asciiz "\n"

_result: .word 0
_v1: .word 0
_n: .word 0
_v3: .word 0
_m: .word 0
_v16: .word 0
_v10: .word 0
_v5: .word 0
_v24: .word 0
_v18: .word 0
_v8: .word 0
_v7: .word 0
_v14: .word 0
_v21: .word 0

.globl main
.text
read:
    li $v0, 4
    la $a0, _prompt
    syscall
    li $v0, 5
    syscall
    jr $ra
write:
    li $v0, 1
    syscall
    li $v0, 4
    la $a0, _ret
    syscall
    move $v0, $0
    jr $ra

fact:
    addi $sp,$sp,-32
    sw $s0,0($sp)
    sw $s1,4($sp)
    sw $s2,8($sp)
    sw $s3,12($sp)
    sw $s4,16($sp)
    sw $s5,20($sp)
    sw $s6,24($sp)
    sw $s7,28($sp)

    li   $t0,1
    sw   $t0,_v3

    move $t0,$a0
    lw   $t1,_v3
    beq  $t0,$t1,label1

    j label2

label1:
    lw $s7,28($sp)
    lw $s6,24($sp)
    lw $s5,20($sp)
    lw $s4,16($sp)
    lw $s3,12($sp)
    lw $s2,8($sp)
    lw $s1,4($sp)
    lw $s0,0($sp)
    addi $sp,$sp,32
    move $t0,$a0
    move $v0,$t0
    jr $ra

    j label3

label2:
    li   $t0,1
    sw   $t0,_v10

    move $t0,$a0
    lw   $t1,_v10
    sub  $t2,$t0,$t1
    sw   $t2,_v8

    addi $sp, $sp, -76
    sw $a0,  0($sp) ## store $a0
    sw $a1,  4($sp) ## store $a1
    sw $a2,  8($sp) ## store $a2
    sw $a3,  12($sp) ## store $a3
    lw $t0,_result
    sw $t0, 16($sp)
    lw $t0,_v1
    sw $t0, 20($sp)
    lw $t0,_n
    sw $t0, 24($sp)
    lw $t0,_v3
    sw $t0, 28($sp)
    lw $t0,_m
    sw $t0, 32($sp)
    lw $t0,_v16
    sw $t0, 36($sp)
    lw $t0,_v10
    sw $t0, 40($sp)
    lw $t0,_v5
    sw $t0, 44($sp)
    lw $t0,_v24
    sw $t0, 48($sp)
    lw $t0,_v18
    sw $t0, 52($sp)
    lw $t0,_v8
    sw $t0, 56($sp)
    lw $t0,_v7
    sw $t0, 60($sp)
    lw $t0,_v14
    sw $t0, 64($sp)
    lw $t0,_v21
    sw $t0, 68($sp)
    sw $ra, 72($sp)
    lw $a0,56($sp)
    jal fact
    lw $a0,  0($sp) ## store $a0
    lw $a1,  4($sp) ## store $a1
    lw $a2,  8($sp) ## store $a2
    lw $a3,  12($sp) ## store $a3
    lw $t0,16($sp)
    sw $t0,_result
    lw $t0,20($sp)
    sw $t0,_v1
    lw $t0,24($sp)
    sw $t0,_n
    lw $t0,28($sp)
    sw $t0,_v3
    lw $t0,32($sp)
    sw $t0,_m
    lw $t0,36($sp)
    sw $t0,_v16
    lw $t0,40($sp)
    sw $t0,_v10
    lw $t0,44($sp)
    sw $t0,_v5
    lw $t0,48($sp)
    sw $t0,_v24
    lw $t0,52($sp)
    sw $t0,_v18
    lw $t0,56($sp)
    sw $t0,_v8
    lw $t0,60($sp)
    sw $t0,_v7
    lw $t0,64($sp)
    sw $t0,_v14
    lw $t0,68($sp)
    sw $t0,_v21
    lw $ra, 72($sp)
    addi $sp, $sp, 76
    sw $v0,_v7

    move $t0,$a0
    lw   $t1,_v7
    mul  $t2,$t0,$t1
    sw   $t2,_v5

    lw $s7,28($sp)
    lw $s6,24($sp)
    lw $s5,20($sp)
    lw $s4,16($sp)
    lw $s3,12($sp)
    lw $s2,8($sp)
    lw $s1,4($sp)
    lw $s0,0($sp)
    addi $sp,$sp,32
    lw   $t0,_v5
    move $v0,$t0
    jr $ra

label3:
main:
    addi $sp,$sp,-32
    sw $s0,0($sp)
    sw $s1,4($sp)
    sw $s2,8($sp)
    sw $s3,12($sp)
    sw $s4,16($sp)
    sw $s5,20($sp)
    sw $s6,24($sp)
    sw $s7,28($sp)

    addi $sp, $sp, -8 ### push stack to store $ra
    sw   $a0,  0($sp) ## store $a0
    sw   $ra,  4($sp) ### store $ra
    jal  read ### invoke read
    lw   $a0,  0($sp) ## store $a0
    lw   $ra,  4($sp) ## read $ra
    addi $sp, $sp, 8
    move $t0,$v0
    sw   $t0,_v14

    li   $t0,1
    sw   $t0,_v16

    lw   $t0,_v14
    lw   $t1,_v16
    bgt  $t0,$t1,label4

    j label5

label4:
    addi $sp, $sp, -76
    sw $a0,  0($sp) ## store $a0
    sw $a1,  4($sp) ## store $a1
    sw $a2,  8($sp) ## store $a2
    sw $a3,  12($sp) ## store $a3
    lw $t0,_result
    sw $t0, 16($sp)
    lw $t0,_v1
    sw $t0, 20($sp)
    lw $t0,_n
    sw $t0, 24($sp)
    lw $t0,_v3
    sw $t0, 28($sp)
    lw $t0,_m
    sw $t0, 32($sp)
    lw $t0,_v16
    sw $t0, 36($sp)
    lw $t0,_v10
    sw $t0, 40($sp)
    lw $t0,_v5
    sw $t0, 44($sp)
    lw $t0,_v24
    sw $t0, 48($sp)
    lw $t0,_v18
    sw $t0, 52($sp)
    lw $t0,_v8
    sw $t0, 56($sp)
    lw $t0,_v7
    sw $t0, 60($sp)
    lw $t0,_v14
    sw $t0, 64($sp)
    lw $t0,_v21
    sw $t0, 68($sp)
    sw $ra, 72($sp)
    lw $a0,64($sp)
    jal fact
    lw $a0,  0($sp) ## store $a0
    lw $a1,  4($sp) ## store $a1
    lw $a2,  8($sp) ## store $a2
    lw $a3,  12($sp) ## store $a3
    lw $t0,16($sp)
    sw $t0,_result
    lw $t0,20($sp)
    sw $t0,_v1
    lw $t0,24($sp)
    sw $t0,_n
    lw $t0,28($sp)
    sw $t0,_v3
    lw $t0,32($sp)
    sw $t0,_m
    lw $t0,36($sp)
    sw $t0,_v16
    lw $t0,40($sp)
    sw $t0,_v10
    lw $t0,44($sp)
    sw $t0,_v5
    lw $t0,48($sp)
    sw $t0,_v24
    lw $t0,52($sp)
    sw $t0,_v18
    lw $t0,56($sp)
    sw $t0,_v8
    lw $t0,60($sp)
    sw $t0,_v7
    lw $t0,64($sp)
    sw $t0,_v14
    lw $t0,68($sp)
    sw $t0,_v21
    lw $ra, 72($sp)
    addi $sp, $sp, 76
    sw $v0,_v18

    j label6

label5:
    li   $t0,1
    sw   $t0,_v21

label6:
    addi $sp, $sp, -8 ### push stack to store $ra
    sw   $a0,  0($sp) ## store $a0
    sw   $ra,  4($sp) ### store $ra
    lw   $t0,_v21
    move $a0,  $t0
    jal  write ### invoke write
    lw   $ra,  4($sp) ## read $ra
    lw   $a0,  0($sp) ## store $a0
    addi $sp, $sp, 8

    li   $t0,0
    sw   $t0,_v24

    lw $s7,28($sp)
    lw $s6,24($sp)
    lw $s5,20($sp)
    lw $s4,16($sp)
    lw $s3,12($sp)
    lw $s2,8($sp)
    lw $s1,4($sp)
    lw $s0,0($sp)
    addi $sp,$sp,32
    lw   $t0,_v24
    move $v0,$t0
    jr $ra

