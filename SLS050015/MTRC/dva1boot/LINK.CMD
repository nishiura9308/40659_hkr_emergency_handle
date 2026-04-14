debug
rom (D,R)
print .\DVA1BOOT.map
nooptimize
start DVECTTBL,DINTTBL(00),PResetPRG,PIntPRG(0400),P,C,D,C$DSEC,C$BSEC(0800),ROMSUM(001FFFF),RAMTOP(0100000),BDOSACCS,BDataWatchDog,B,R(017C000),S(017F000),BIntChgTable,BDOSACCS2(0FFFD10)
output .\DVA1BOOT.abs
input .\obj\vecttbl.obj
input .\obj\resetprg.obj
input .\obj\dbsct.obj
input .\obj\lowlvl.obj
input .\obj\sbrk.obj
input .\obj\dvaio.obj
input .\obj\DVAUMAIN.obj
input .\obj\BOOT.obj
input .\obj\jump.obj
input .\obj\pcaccs.obj
input .\obj\dosaccs1.obj
input .\obj\dosaccs2.obj
input .\obj\dosaccs3.obj
input .\obj\cluster.obj
input .\obj\diraccs.obj
input .\obj\DVAINIT.obj
library V:\lib\c38has.lib
library V:\lib\ec2has.lib
