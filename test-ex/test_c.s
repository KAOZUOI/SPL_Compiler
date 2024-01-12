.data
_t6: .word 0
_v2: .word 0
_t3: .word 0
_t2: .word 0
_v0: .word 0
_t1: .word 0
_t0: .word 0
_v1: .word 0
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
isPrime:
  lw $t0, 36($sp)
  sw $t0, _v0
  li $t0, 2
  sw $t0, _v1
  lw $t0, _v0
  li $t1, 1
  bgt $t0, $t1, label0
  li $v0, 0
  jr $ra
label0:
label2:
  lw $t0, _v1
  lw $t1, _v1
  mul $t2, $t0, $t1
  sw $t2, _t0
  lw $t0, _t0
  lw $t1, _v0
  ble $t0, $t1, label3
  j label4
label3:
  lw $t0, _v0
  lw $t1, _v1
  div $t0, $t1
  mflo $t2
  sw $t2, _t1
  lw $t0, _t1
  lw $t1, _v1
  mul $t2, $t0, $t1
  sw $t2, _t2
  lw $t0, _v0
  lw $t1, _t2
  sub $t2, $t0, $t1
  sw $t2, _t3
  lw $t0, _t3
  move $t1, $zero
  bne $t0, $t1, label5
  li $v0, 0
  jr $ra
label5:
  lw $t0, _v1
  li $t1, 1
  add $t2, $t0, $t1
  sw $t2, _v1
  j label2
label4:
  li $v0, 1
  jr $ra
main:
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, _v2
  lw $t0, _v2
  addi $sp, $sp, -4
  sw $t0, 0($sp)
  addi $sp, $sp, -36
  lw $t0, _t6
  sw $t0, 0($sp)
  lw $t0, _v2
  sw $t0, 4($sp)
  lw $t0, _t3
  sw $t0, 8($sp)
  lw $t0, _t2
  sw $t0, 12($sp)
  lw $t0, _v0
  sw $t0, 16($sp)
  lw $t0, _t1
  sw $t0, 20($sp)
  lw $t0, _t0
  sw $t0, 24($sp)
  lw $t0, _v1
  sw $t0, 28($sp)
  sw $ra, 32($sp)
  jal isPrime
  lw $t0, 0($sp)
  sw $t0, _t6
  lw $t0, 4($sp)
  sw $t0, _v2
  lw $t0, 8($sp)
  sw $t0, _t3
  lw $t0, 12($sp)
  sw $t0, _t2
  lw $t0, 16($sp)
  sw $t0, _v0
  lw $t0, 20($sp)
  sw $t0, _t1
  lw $t0, 24($sp)
  sw $t0, _t0
  lw $t0, 28($sp)
  sw $t0, _v1
  lw $ra, 32($sp)
  addi $sp, $sp, 40
  sw $v0, _t6
  lw $t0, _t6
  li $t1, 1
  bne $t0, $t1, label7
  li $a0, 1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  j label9
label7:
  li $a0, 0
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
label9:
  li $v0, 0
  jr $ra
