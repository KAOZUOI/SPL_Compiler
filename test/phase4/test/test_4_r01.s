# SPL compiler generated assembly
.data
_prmpt: .asciiz "Enter an integer: "
_eol: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prmpt
  syscall
  li $v0, 5
  syscall
  jr $ra
write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _eol
  syscall
  move $v0, $0
  jr $ra
hanoi:
  move $t0, $a0
  move $t1, $a1
  move $t2, $a2
  move $t3, $a3
  move $t4, $t0
  li $t5, 1
  beq $t4, $t5, label0
  j label1
label0:
  move $t6, $t1
  li $t7, 10000
  mul $s0, $t6, $t7
  move $s1, $t3
  add $s2, $s0, $s1
  move $a0, $s2
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  j label2
label1:
  move $s3, $t0
  li $s4, 1
  sub $s5, $s3, $s4
  move $s6, $t1
  move $s7, $t3
  move $t8, $t2
  move $a0, $t8
  move $a1, $s7
  move $a2, $s6
  move $a3, $s5
  jal hanoi
  move $t9, $v0
  move $t0, $t1
  li $t0, 10000
  mul $t0, $t0, $t1
  move $t2, $t3
  add $t2, $t2, $t3
  move $a0, $t4
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t4, $t4
  li $t5, 1
  sub $t5, $t5, $t6
  move $t7, $t7
  move $s0, $s0
  move $s1, $s1
  addi $sp, $sp, -4
  sw $s2, 0($sp)
  addi $sp, $sp, -4
  sw $s3, 0($sp)
  addi $sp, $sp, -4
  sw $s4, 0($sp)
  addi $sp, $sp, -4
  sw $s5, 0($sp)
  jal hanoi
  move $s6, $v0
label2:
  li $s6, 0
  move $v0, $s6
  jr $ra
main:
  li $s6, 3
  move $s6, $s6
  move $s7, $s7
  li $t8, 1
  li $t8, 2
  li $t8, 3
  addi $sp, $sp, -4
  sw $t8, 0($sp)
  addi $sp, $sp, -4
  sw $t8, 0($sp)
  addi $sp, $sp, -4
  sw $t9, 0($sp)
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  jal hanoi
  move $t0, $v0
  li $t0, 0
  move $v0, $t0
  jr $ra