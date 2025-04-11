.text
.align 4
.set sp, 1

.extern returnFromJumpHeightChange
.extern breakBlock

#		STATS		#

.global updateJumpHeight
updateJumpHeight:
	lwz r8, 0x14d4(r31)				# Get our current character ID
	
	cmpwi r8, 1						# Luigi
	beq LuigiTime
	
	cmpwi r8, 2						# Toad
	beq WarioTime
	
	cmpwi r8, 3						# Toad 2
	beq WaluigiTime
	
	b MarioTime						# Else (includes Yoshi!)

LuigiTime:
	lis r7, LuigiMaxJumpHeight@h
	ori r7, r7, LuigiMaxJumpHeight@l
	
	lfs f0, 0(r7)					# Put our custom JH into our float
	b returnFromJumpHeightChange	# Restore our original instruction(s)

WarioTime:
	lis r7, WarioMaxJumpHeight@h
	ori r7, r7, WarioMaxJumpHeight@l
	
	lfs f0, 0(r7)					
	b returnFromJumpHeightChange	

WaluigiTime:
	lis r7, WarioMaxJumpHeight@h
	ori r7, r7, WarioMaxJumpHeight@l
	
	lfs f0, 0(r7)				
	b returnFromJumpHeightChange

MarioTime:
	lis r7, MarioMaxJumpHeight@h
	ori r7, r7, MarioMaxJumpHeight@l
	
	lfs f0, 0(r7)					
	b returnFromJumpHeightChange


.data

# Max Jump Heights
MarioMaxJumpHeight:
	.float 3.628					# Original height

LuigiMaxJumpHeight:
	.float 3.80						# Forgot what it was before L.O.L....

WarioMaxJumpHeight:
	.float 3.467
	
WaluigiMaxJumpHeight:
	.float 3.814