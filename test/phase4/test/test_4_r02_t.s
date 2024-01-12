.data
_v14: .word 0
_v13: .word 0
_v10: .word 0
_v11: .word 0
_v8: .word 0
_v7: .word 0
_v6: .word 0
_v4: .word 0
_v3: .word 0
_v0: .word 0
_v2: .word 0
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
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  sw $v0, _v2
  lw $t0, _v2
  sw $t0, _v0
  lw $t0, _v0
  sw $t0, _v3
  move $t0, $zero
  sw $t0, _v4
  lw $t0, _v3
  lw $t1, _v4
  bgt $t0, $t1, label0
  j label1
label0:
  li $t0, 1
  sw $t0, _v6
  lw $a0, _v6
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  j label2
label1:
  lw $t0, _v0
  sw $t0, _v7
  move $t0, $zero
  sw $t0, _v8
  lw $t0, _v7
  lw $t1, _v8
  blt $t0, $t1, label3
  j label4
label3:
  li $t0, 1
  sw $t0, _v11
  move $t0, $zero
  lw $t1, _v11
  sub $t2, $t0, $t1
  sw $t2, _v10
  lw $a0, _v10
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  j label5
label4:
  move $t0, $zero
  sw $t0, _v13
  lw $a0, _v13
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
label5:
label2:
  move $t0, $zero
  sw $t0, _v14
  lw $v0, _v14
  jr $ra
