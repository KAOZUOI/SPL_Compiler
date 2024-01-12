.data
_v3: .word 0
_t5: .word 0
_v2: .word 0
_t4: .word 0
_t3: .word 0
_t2: .word 0
_v1: .word 0
_t1: .word 0
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
  lw $t0, 36($sp)
  sw $t0, _v1
  lw $t0, _v1
  li $t1, 1
  beq $t0, $t1, label1
  j label2
label1:
  lw $v0, _v1
  jr $ra
label2:
  lw $t0, _v1
  li $t1, 1
  sub $t2, $t0, $t1
  sw $t2, _t1
  lw $t0, _t1
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -36
  lw $t0, _v3
  sw $t0, 0($sp)
  lw $t0, _t5
  sw $t0, 4($sp)
  lw $t0, _v2
  sw $t0, 8($sp)
  lw $t0, _t4
  sw $t0, 12($sp)
  lw $t0, _t3
  sw $t0, 16($sp)
  lw $t0, _t2
  sw $t0, 20($sp)
  lw $t0, _v1
  sw $t0, 24($sp)
  lw $t0, _t1
  sw $t0, 28($sp)
  sw $ra, 32($sp)
  jal fact
  lw $t0, 0($sp)
  sw $t0, _v3
  lw $t0, 4($sp)
  sw $t0, _t5
  lw $t0, 8($sp)
  sw $t0, _v2
  lw $t0, 12($sp)
  sw $t0, _t4
  lw $t0, 16($sp)
  sw $t0, _t3
  lw $t0, 20($sp)
  sw $t0, _t2
  lw $t0, 24($sp)
  sw $t0, _v1
  lw $t0, 28($sp)
  sw $t0, _t1
  lw $ra, 32($sp)
  addi $sp, $sp, 40
  sw $v0, _t2
  lw $t0, _v1
  lw $t1, _t2
  mul $t2, $t0, $t1
  sw $t2, _t3
  lw $v0, _t3
  jr $ra
main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, _t4
  lw $t0, _t4
  sw $t0, _v2
  lw $t0, _v2
  li $t1, 1
  bgt $t0, $t1, label3
  j label4
label3:
  lw $t0, _v2
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -36
  lw $t0, _v3
  sw $t0, 0($sp)
  lw $t0, _t5
  sw $t0, 4($sp)
  lw $t0, _v2
  sw $t0, 8($sp)
  lw $t0, _t4
  sw $t0, 12($sp)
  lw $t0, _t3
  sw $t0, 16($sp)
  lw $t0, _t2
  sw $t0, 20($sp)
  lw $t0, _v1
  sw $t0, 24($sp)
  lw $t0, _t1
  sw $t0, 28($sp)
  sw $ra, 32($sp)
  jal fact
  lw $t0, 0($sp)
  sw $t0, _v3
  lw $t0, 4($sp)
  sw $t0, _t5
  lw $t0, 8($sp)
  sw $t0, _v2
  lw $t0, 12($sp)
  sw $t0, _t4
  lw $t0, 16($sp)
  sw $t0, _t3
  lw $t0, 20($sp)
  sw $t0, _t2
  lw $t0, 24($sp)
  sw $t0, _v1
  lw $t0, 28($sp)
  sw $t0, _t1
  lw $ra, 32($sp)
  addi $sp, $sp, 40
  sw $v0, _t5
  lw $t0, _t5
  sw $t0, _v3
  j label5
label4:
  li $t0, 1
  sw $t0, _v3
label5:
  lw $a0, _v3
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
