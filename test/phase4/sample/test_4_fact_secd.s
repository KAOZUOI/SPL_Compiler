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
fact:
  move $t0, $a0
  beq $t0, $t1, label1
  j label2
label1:
  move $v0, $t0
  jr $ra
label2:
  addi $t2, $t0, -1
  move $a0, $t2
  jal fact
  move $t3, $v0
  mul $t4, $t0, $t3
  move $v0, $t4
  jr $ra
main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t5, $v0
  move $t6, $t5
  bgt $t6, $t1, label3
  j label4
label3:
  move $a1, $t6
  jal fact
  move $t7, $v0
  move $s0, $t7
  j label5
label4:
  li $s0, 1
label5:
  move $a0, $s0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $s1
  jr $ra