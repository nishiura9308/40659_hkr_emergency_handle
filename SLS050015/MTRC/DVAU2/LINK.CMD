debug
print .\DVAU2.map
nooptimize
start PEntryProgram,P,C,D,B(0080000),BDataWatchDog,BBootCheck(009E800),S(009F000),BIntChgTable(0FFFD10)
output .\DVAU2.abs
input .\obj\entry.obj
input .\obj\sbrk.obj
input .\obj\DVAMAIN.obj
input .\obj\DVAINIT2.obj
input .\obj\COMMLIB.obj
input .\obj\DVAEIDS.obj
input .\obj\DVATIME.obj
input .\obj\DVACOMM.obj
library V:\lib\c38has.lib
library V:\lib\ec2has.lib
