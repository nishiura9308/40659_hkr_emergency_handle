;/*[HW.H]*************************************************************
;*	y`’n‰º“S@Ž©“®•ú‘—§Œäz				     *
;*-------------------------------------------------------------------*
;*	‚g^‚v‘€ì—p’è”’è‹`  ‚o‚`‚`—p					     *
;*-------------------------------------------------------------------*
;*    Author	TOA Corporation   A.Murakami			     *
;*								     *
;*    Revison	a00. 1995/11/28					     *
;*								     *
;********************************************************************/
;@@@S
;†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£
;	‚b‚o‚tŠÖŒW
;†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£†£

$.\INC\IO64180.INC


					;-- ŠO•”ƒ|[ƒgƒAƒhƒŒƒX --
EXT_PORT0	equ	80h		; IN  b0-b4: ”ñí’âŽ~“ü—Í
						;        b5: EMG Microphone
						; OUT b0-b4: ”ñí’Ê•ñ‘•’u‰¹º“ü—ÍƒQ[ƒg
						;        b6: AMP LED
						;		 b7: RUN LED
EXT_PORT1	equ	81h		; IN  b0-b1: Noise Level
						;     b2-b3: AMP Output Confirm
						;        b4: Voice 24V
						;	  b5-b7: Address
						; OUT b0-b5: Speaker Output
						;     b6-b7: AMP Output
EXT_PORT2	equ	82h		; IN
						; OUT b0-b1: Noise Level ON
						;		 b7: Watch Dog Enable
EXT_PORT3	equ	83h		; OUT Debug LED

EXT_PORT7	equ	87h		; OUT Watch Dog Reset


TM0CNST		equ	3072		; 10msƒJƒEƒ“ƒ^’l
TM1CNST		equ	307			;  1msƒJƒEƒ“ƒ^’l
							; ƒ³=6.144MHz
							; Clock = 0.16276uS
							; TimeClock = 3.25521 uS
							; 10mS Count = 3072

					;-----	RAM—Ìˆæ		-----
RAMTOP		equ	0C000h		; RAM—Ìˆæ æ“ª”Ô’n
RAMBOT		equ	0fdffh		; RAM—Ìˆæ ÅI”Ô’n

