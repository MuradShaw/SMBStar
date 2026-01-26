.text
.align 4
.set sp, 1

.extern returnFromHijack

.global reduceMultiplier
reduceMultiplier:
    b hijack

	cmpwi r8, 1						# new type
	beq hijack
	
	# default
    stfs f1, 0x8(r3)
    b returnFromHijack

hijack:
    lis r7, newMultiplier@h
    ori r7, r7, newMultiplier@l

    lfs f1, 0(r7)          # load custom multiplier in
    stfs f1, 0x8(r3)

    b returnFromHijack


.data

newMultiplier:
	.float 0.0