debug
print .\DVAU1.map
nooptimize
start PEntryProgram,P,C,D,B(0100000),BBACKUP(0160000),BDOSACCS,BDataWatchDog(017C000),S(017F000),BIntChgTable,BDOSACCS2(0FFFD10)
output .\DVAU1.abs
input .\obj\entry.obj
input .\obj\lowlvl.obj
input .\obj\sbrk.obj
input .\obj\dosaccs3.obj
input .\obj\DVAMAIN.obj
input .\obj\DVAINIT1.obj
input .\obj\dvatms.obj
input .\obj\dvatime.obj
input .\obj\hdlc.obj
input .\obj\commlib.obj
input .\obj\dvaev500.obj
input .\obj\dvatrace.obj
input .\obj\dvatable.obj
input .\obj\dvamcp.obj
input .\obj\dvaproc.obj
input .\obj\dvaio.obj
input .\obj\dvamente.obj
library V:\lib\c38has.lib
library V:\lib\ec2has.lib
