.text
.align 4
.set sp, 1

.extern returnFromExtensionChange
.extern returnFromInputDetection

.global increaseWaitingTime
increaseWaitingTime:
	li r0, 0x168				# New timer value. 0x78 was the original
	stw r0, 0x3b8(r31)			# Load into var
	
	b returnFromExtensionChange

.global skipInputDetection
skipInputDetection:
	b returnFromInputDetection