debug
rom (D,R)
print .\DVA1BOOT.map
nooptimize
start DVECTTBL,DINTTBL(00),PResetPRG,PIntPRG(0400),P,C,D,C$DSEC,C$BSEC(0800),ROMSUM(001FFFF),RAMTOP(0080000),B,R(0094000),BDOSACCS,BDataWatchDog(009A000),S(009F000),BIntChgTable,BDOSACCS2(0FFFD10)
output .\DVA1BOOT.abs
input .\bootobj\vecttbl.obj
input .\bootobj\resetprg.obj
input .\bootobj\dbsct.obj
input .\bootobj\lowlvl.obj
input .\bootobj\sbrk.obj
input .\bootobj\dvaio.obj
input .\bootobj\DVAUMAIN.obj
input .\bootobj\BOOT.obj
input .\bootobj\jump.obj
input .\bootobj\pcaccs.obj
input .\bootobj\dosaccs1.obj
input .\bootobj\dosaccs2.obj
input .\bootobj\dosaccs3.obj
input .\bootobj\cluster.obj
input .\bootobj\diraccs.obj
input .\bootobj\DVAINIT.obj
input .\bootobj\dvamente.obj
input .\bootobj\commlib.obj
library V:\lib\c38has.lib
library V:\lib\ec2has.lib
