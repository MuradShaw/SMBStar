.text
.align 4
.set sp, 1

.extern returnFromLimiterFix
.extern freddyFazbearion
.extern continueFromTopFix

.global limiterFix
limiterFix:
	lis r7, titleScreen@h
	ori r7, r7, titleScreen@l
	
	lfs f0, 0(r7)
	fcmpo cr0, f3, f0
	bgt freddyFazbeari
	
	fsubs f7, f23, f3	# Get our difference between the last screentop and our current
	fabs f7, f7
	
	lis r7, jitterThreshold@h
	ori r7, r7, jitterThreshold@l

	lfs f0, 0(r7)
	
	fcmpo cr0, f7, f0
	bgt distanceFix
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l

	lfs f0, 0(r7)
	
	fcmpo cr0, f0, f23 # Our previous is smaller than our new one, meaning we're going up
	blt goingUp
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l

	stfs f23, 0(r7)
	
	cmpwi r27, 0
	beq freddyFazbeario
	
	b returnFromLimiterFix

goingUp:
	fsubs f7, f0, f23
	fabs f7, f7
	
	lis r7, jitterThreshold@h
	ori r7, r7, jitterThreshold@l

	lfs f0, 0(r7)
	
	fcmpo cr0, f7, f0
	bgt distanceFix_goingUp
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l
	stfs f23, 0(r7)
	
	b freddyFazbeari
	
distanceFix:
	lis r7, newDistance@h
    ori r7, r7, newDistance@l
    lfs f0, 0(r7)

    fadds f23, f3, f0
	
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l
	stfs f23, 0(r7)
	
	cmpwi r27, 0
	beq freddyFazbeario
	
    b returnFromLimiterFix

distanceFix_goingUp:
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l

	lfs f0, 0(r7)
	
	fadds f3, f0, 0
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l

	lfs f7, 0(r7)
	
	lis r7, newDistance@h
    ori r7, r7, newDistance@l
    lfs f0, 0(r7)

    fsubs f23, f7, f0
	
	
	lis r7, jitterThresholdHigh@h
	ori r7, r7, jitterThresholdHigh@l
	stfs f23, 0(r7)
	
	cmpwi r27, 0
	beq freddyFazbeario
	
    b returnFromLimiterFix

freddyFazbeario:
	b freddyFazbearion

freddyFazbeari:
	cmpwi r27, 0
	beq freddyFazbeario
	
	b returnFromLimiterFix

.global limiterFixTop
limiterFixTop:
	lis r7, titleScreen@h
	ori r7, r7, titleScreen@l
	
	lfs f0, 0(r7)
	fcmpo cr0, f28, f0
	bgt restoreOriginalFunction
	
	lfs f0, -0x0154(r3)
	
	fcmpo cr0, f28, f0
	bgt checkConditions
	
	b restoreOriginalFunction

checkConditions:
	#lis r7, hopefulFix@h
    #ori r7, r7, hopefulFix@l
    #lfs f8, 0(r7)
	
	fsubs f7, f0, f28
	fabs f7, f7
	
	#fcmpo cr0, f7, f8
	#bgt restoreOriginalFunction
	
	lis r7, jitterThresholdTop@h
    ori r7, r7, jitterThresholdTop@l
    lfs f8, 0(r7)
	
	fcmpo cr0, f7, f8
	bgt distanceFixTop
	
	b restoreOriginalFunction

distanceFixTop:
	lis r7, zero@h
    ori r7, r7, zero@l
    lfs f7, 0(r7)
	
	fadds f28, f0, f7
	
	b restoreOriginalFunction
	
restoreOriginalFunction:
	fadds f2, f28, f2
	b continueFromTopFix

jitterThresholdTop:
	.float 30.0
	
jitterThreshold:
	.float 13.0
	
jitterThresholdHigh:
	.float -800.0

previousTopScreen:
	.float 0.0
	
titleScreen:
	.float -400

hopefulFix:
	.float 200.0
	
zero:
	.float 7.0
	
newDistance:
	.float -7