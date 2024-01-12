.data
_v16: .word 0
_v15: .word 0
_v7: .word 0
_v11: .word 0
_v13: .word 0
_v8: .word 0
_v4: .word 0
_v2: .word 0
_v0: .word 0
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
main:
  li $t0, 110
  sw $t0, _v0
  li $t0, 97
  sw $t0, _v2
  li $t0, 3
  sw $t0, _v4
  lw $t0, _v0
  lw $t1, _v2
  sub $t2, $t0, $t1
  sw $t2, _v8
  li $t0, 2
  sw $t0, _v13
  lw $t0, _v4
  lw $t1, _v13
  mul $t2, $t0, $t1
  sw $t2, _v11
  lw $t0, _v8
  lw $t1, _v11
  add $t2, $t0, $t1
  sw $t2, _v7
  lw $t0, _v7
  sw $t0, _v4
  lw $t0, _v4
  sw $t0, _v15
  lw $a0, _v15
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t0, $zero
  sw $t0, _v16
  lw $v0, _v16
  jr $ra
