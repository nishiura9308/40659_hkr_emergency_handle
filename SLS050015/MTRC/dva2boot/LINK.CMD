debug
rom (D,R)
print .\DVA2BOOT.map
nooptimize
start DVECTTBL,DINTTBL(00),PResetPRG,PIntPRG(0400),P,D,C$DSEC,C$BSEC(0800),ROMSUM(01FFFF),RAMTOP(0080000),B,R(009E000),BDataWatchDog,BBootCheck(009E800),S(009F000),BIntChgTable(0FFFD10)
output .\DVA2BOOT.abs
input .\obj\dbsct.obj
input .\obj\lowlvl.obj
input .\obj\sbrk.obj
input .\obj\vecttbl.obj
input .\obj\resetprg.obj
input .\obj\DVAUMAIN.obj
input .\obj\jump.obj
input .\obj\DVAINIT.obj
input .\obj\boot.obj
library V:\lib\c38has.lib
library V:\lib\ec2has.lib
