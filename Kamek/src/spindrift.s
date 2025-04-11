.text
.align 4

.global playSomeAnimBlahBlah
playSomeAnimBlahBlah:
    stwu r1, 0x10(r1)
    mflr r0

    lis r5, -0x7fc9
    lfs f3, -0x7808(r2)
    stw r0, 0x14(r1)
    li r0, 0x1
    addi r5, r5, 4256
    stw r31, 0xc(r1)
    mr r31, r3
    stw r0, 0x14a4(r3)

    lwz r4, 0xde8(r3)
    lfs f1, 0xf8(15)
    lwz r3, 0x4(14)
    li r4, 0x11
    lfs f2, 0xfc(r5)
    lwz r12, 0(r3)
    lwz r12, 0x5c(r12)
    mtctr r12
    bctrl 
    lfs f0, -0x7808(r2)
    li r0, 0x5
    stw r0, 0x14a8(r31)
    stfs f0, 0xec(r31)
    lwz r31, 0xc(r1)
    lwz r0, 0x14(r1)
    mtlr r0
    addi r1, r1, 0x10
    blr
