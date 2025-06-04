.text
.align 4
.set sp, 1

.extern returnFromBGFix

.global removeBGLoop
removeBGLoop:
    lis r7, NewLoopThreshold@h
    ori r7, r7, NewLoopThreshold@l

    lfs f1, 0(r7)

    b returnFromBGFix

NewLoopThreshold:
	.float 100000