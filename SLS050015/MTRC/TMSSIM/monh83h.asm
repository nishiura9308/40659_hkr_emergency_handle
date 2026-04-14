;****************************************************************
;*    ＰＡＲＴＮＥＲ-ＥＴⅡ　モニタプログラム  Ｖｅｒ 1.1 	*
;*								*
;*								*
;*	ＭＯＮＨ８．ＡＳＭ					*
;*      for CPU:H8/300H H8S/2000				*
;*								*
;*   Copyright 1997 (c) 京都マイクロコンピュータ株式会社	*
;****************************************************************
;------------------------------------------------------------------------------
;	モニタプログラム本体（ＭＯＮＨ８３Ｈ． ＫＰＴ）の作成法
;==============================================================================
; 作成方法
;
;    A>PPH83H MONH83H.ASM > MONH83H.SRC
;    A>GMONH83H MONH83H.SRC
;

;-------------------------------------------------------------------------------
;　　　　　　　  ユーザのカスタマイズ箇所
;=============================================================================
;  1.ユーザがカスタマイズする必要がある部分は『;++++++･････』で示されています。
;    不用意にその他の箇所を変更／削除／追加してはいけません。
;    また、このファイルを変更する場合は、必ずコピーしてからにしてください。
;　2.＜変更箇所１，２，３，４＞がユーザが変更／追加する必要がある部分です。
;  3.＜変更箇所１＞は、指定されたシンボルをターゲットシステムに合わせて設定し
;    てください。
;  4.＜変更箇所２，３＞ではユーザプログラムの実行およびブレーク時に行うべき
;    処理を記述できます。通常は、何も記述する必要はありません。
;  5.＜変更箇所４＞では、ターゲットの初期化ルーチンを必要ならば記述します。
;  6.＜変更箇所５＞では、CFGファイルでRESET PCを設定した場合に記述します
;-----------------------------------------------------------------------------

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
*   変更箇所１：以下のシンボルを設定してください。
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++

*-------------- ＣＰＵのバスサイズの指定 ---------------------------------
* BUS8	････ ８ビットバスの場合 1 にセット
* BUS16	････ １６ビットバスの場合 1 にセット
*-------------------------------------------------------------------------
#define BUS8		1
#define BUS16		0

*-------------- ＣＰＵの割り込み制御モードの指定 -------------------------
* INTMODE2 ････ 1 H8Sで割り込み制御モード 2 を使用する場合 1 にセット
* INTMODE2 ････ 0 H8Sで割り込み制御モード 0,1,3 を使用する場合 0 にセット
*-------------------------------------------------------------------------
;#define INTMODE2	1
#define INTMODE2	0

;-------------- モニタ動作時割り込み許可／禁止の設定 -------------------------
; MONSTID ････ モニタ動作中は割り込み禁止状態の場合 1 にセット
; MONSTIE ････ モニタ動作中は割り込み許可状態の場合 1 にセット
; MONSTIC ････ モニタ動作中はユーザプログラムの状態を引き継ぐの場合 1 にセット
;-----------------------------------------------------------------------------
#define	MONSTID	1
#define	MONSTIE	0
#define	MONSTIC	0



;=============================================================
;  この部分以降 <変更箇所２＞ までは、変更しないでください。
;=============================================================

;===== Target Status/Control Port ==============
;===== Target Status/Control Port ==============
;-- NEW ET size 256 FIX

#define BUS32	0

#if BUS8
;--- 8bit bus new ET size256 ---
CMDRD	equ	0x86
STAT	equ	0x80
STAT1	equ	0x81
RUNOFF	equ	0x80
RUNON	equ	0x82
CMDWRS  equ	0x84
#endif

#if BUS16
;--- 16bit bus ---
CMDRD	equ	0x8c+1
STAT	equ	0x80+1
STAT1	equ	0x82+1
RUNOFF	equ	0x80+1
RUNON	equ	0x84+1
CMDWRS  equ	0x88+1
#endif

#if BUS32
;--- 16bit bus ---

CMDRD	equ	0x118+3
STAT	equ	0x100+3
STAT1	equ	0x104+3
RUNOFF	equ	0x100+3
RUNON	equ	0x108+3
CMDWRS  equ	0x110+3

#endif


BRKVECT	equ	0x1c


	org	0

cmdstart:
	db	0x15			; Table Mark				/ 0h
	rdw	0			; 1 shadow_jump
	rdw	0			; 3
	rdw	0			; 5
	rdw	usrnmip			; 7 ユーザＮＭＩ（ＴＲＡＰ）pachiアドレス / 7h
	rdw	(nmientry & 0x7f)	; 9   NMI Entry point			/ 9h
	rdw	(sbrkentry & 0x7f)	; 11  TRACE Entry(NON SHADOW)		/ bh
	rdw	0			; 13  NMI Entry point(shadowentry LOW)			/ dh
	rdw	0			; 15  NMI Entry point(shadowentry HIGH)			/ fh
#if	MONSTID
	rdw	0
#else
#if	MONSTIE
	rdw	1
#else
#if	MONSTIC
	rdw	2
#endif
#endif
#endif


#if BUS8 /* { */
	rdw	1		; 19  BUS SIZE
#endif /* } */
#if BUS16 /* { */
	rdw	2		; 19  BUS SIZE
#endif /* } */



;******************************************
;	コモンバンク(Bank 7F)
;******************************************

	org	0x90

mainlp:
	mov.b	#cmdstart,r1l

;---- 1Byte RX ----	er1=RETURN  r0l=get data  er2=monitor address
getrs:
	mov.b	#STAT,r2l
	btst	#7,@er2			; Communication wait state
	beq	getrs:8
getrs1:
	mov.b	#CMDRD,r2l
	mov.b	@ER2,r0l
	mov.b	r1l,r2l
	jmp	@er2			; RETURN

;---- 1Byte TX ----	er1=RETURN  r0h = count r0l=put data  er2=monitor address
putrs:
	mov.b	#STAT,r2l
	btst	#0,@er2			; Communication wait state
	beq	putrs:8
prsll:
	mov.b	r0l,r0h
	and.b	#0x0f,r0h
#if BUS8
	shll.b	r0h
#endif
#if BUS16
	shll.b	r0h
	shll.b	r0h
#endif
	mov.b	r0h,r2l
	mov.b	@er2,r0h		; Low 4 bit send

	mov.b	r0l,r0h
	and.b	#0xf0,r0h

#if BUS8 /* { */
	shlr.b	r0h
	shlr.b	r0h
	shlr.b	r0h
	bset	#5,r0h		; OR 0x20
#endif /* } */
#if BUS16
	shlr.b	r0h
	shlr.b	r0h
	bset	#6,r0h		; OR 0x40
#endif
	mov.b	r0h,r2l
	mov.b	@er2,r0h		; High 4 bit send

bnext:
	mov.b	#CMDWRS,r2l
	mov.b	@er2,r2l
	mov.b	r1l,r2l
	jmp	@er2			; RETURN


;******************************************
; PROFILE ENTRY
;******************************************
#if 1 /* { */
prof_ent:

; PUTL
; 下位から送る
	mov.b	#prf01,r1l
	bra     prsll:8		; bsr prsll
prf01:

	mov.b	r1h,r0l
	mov.b	#prf02,r1l
	bra     prsll:8		; bsr prsll
prf02:

	mov.w	e1,r1
	mov.b	r1l,r0l
	mov.b	#prf03,r1l
	bra     prsll:8		; bsr prsll
prf03:

	mov.b	#0x0,r0l
	mov.b	#prf04,r1l
	bra     prsll		; bsr prsll
prf04:

	pop.l	er1
	pop.l	er2
	pop.w	r0
	rte
#endif /* } */


	org	0xfa
	dw	0			; RTC hosei

;******************************************
;	PEEKB (BANK 8)
;******************************************
	BANK	8
peekb:
	GETL		; ER3 <- addres
	mov.b	@er3,r0l
	PUTB_MAIN

;******************************************
;	PEEKW (BANK 10)
;******************************************
	BANK	10
peekw:
	GETL		; ER3 <- addres
	mov.w	@er3,r3
	PUTW_MAIN

;******************************************
;	POKEB (BANK 18)
;******************************************
	BANK	18
pokeb:
	GETL		; ER3 <- addres
	GETB		; R0L <- DATA
	mov.b	r0l,@er3
	BANK_NEXT


;******************************************
;	POKEW (BANK 20)
;******************************************
	BANK	20
pokew:
	GETL		; ER3 <- addres
	mov.l	er3,er4
	GETW		; R3 <- DATA
	mov.w	r3,@er4
	BANK_NEXT


;******************************************
;	GETBLK (BANK 28)
;******************************************
	BANK	28
getblk:
	GETL
	mov.l	er3,er4		; ER4 <- addres
	GETW
	mov.w	r3,r5		; R5 <- count
	mov.w	#0,e5
	BANK_NEXT

	BANK	29
gblklp:
	mov.b	@er4+,r0l
	PUTB
	dec.l	#1,er5
	bne	gblklp:8
	JMP_MAIN

;******************************************
;	PUTBLK (BANK 30)
;******************************************
	BANK	30

putblk:
	GETL
	mov.l	er3,er4		; ER4 <- addres
	GETW
	mov.w	r3,r5		; R5 <- count
	mov.w	#0,e5
	BANK_NEXT

	BANK	31
pblklp:
	GETB
	mov.b	r0l,@er4
	inc.l	#1,er4
	dec.l	#1,er5
	bne	pblklp:8
	BANK_NEXT

*******************************************
*	FILL
*******************************************
	BANK	38
fill:
	GETL
	mov.l	er3,er4		; er4 <- address
	PUTB_MAIN

	BANK	39
	GETL
	mov.l	er3,er5		; er5 <- count
	GETW
	mov.w	r3,r6		; r6 <- WORD data
	BANK_NEXT

	BANK	3a
	GETB
	or.b	#0,r0l		; 1..word , 0..byte
	bne	fillw:8
fillp:
	mov.b	r6l,@er4
	inc.l	#1,er4
	dec.l	#1,er5
	bne	fillp:8
	bra	fillend:8
fillw:
	mov.w	r6,@er4
	inc.l	#2,er4
	dec.l	#1,er5
	bne	fillw:8

fillend:
	mov.b	#0,r0l
fill1:
	PUTB_MAIN

*******************************************
*	MOVE
*******************************************
	BANK	40
move_cmd:
	GETL
	mov.l	er3,er4		; er4 <- (SRC address)
	BANK_NEXT

	BANK	41
	GETL
	mov.l	er3,er5		; er5 <- (DST address)
	BANK_NEXT

	BANK	42
	GETL
	mov.l	er3,er6		; er6 <- (count)
	BANK_NEXT

	BANK	43
	GETB				* direction
	or.b	#0,r0l
	beq	movinc:8
movdec:
	mov.b	@er4,r0l
	mov.b	r0l,@er5
	dec.l	#1,er4
	dec.l	#1,er5
	dec.l	#1,er6
	bne	movdec:8
	bra	mov_end:8
movinc:
	mov.b	@er4,r0l
	mov.b	r0l,@er5
	inc.l	#1,er4
	inc.l	#1,er5
	dec.l	#1,er6
	bne	movinc:8
mov_end:
	mov.b	#0,r0l
move1:
	PUTB_MAIN


;******************************************
;	SPSET (BANK 50)
;******************************************
	BANK	50

	GETL
	mov.l	er3,sp		; USER STACK (SP)
	mov.b	@er2,r2l	; NMI Request CLR
	mov.b	#RUNOFF,r2l
	mov.b	@er2,r2l
	BANK_NEXT

;******************************************
;	FLGSET (BANK 51)
;******************************************
	BANK	51
flgset:
	stc	ccr,r0l
	PUTB
	GETB
	ldc	r0l,ccr
	BANK_NEXT


;******************************************
;	GCMD (BANK 60)
;******************************************
	BANK	60

gcmd:
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;   変更箇所２：ＵＳＲＧＯＲＴＮ
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;================＜ＵＳＲＧＯＲＴＮ＞===================
; Ｇコマンド等でユーザプログラムが実行される直前に
; このルーチンが呼ばれます。ユーザプログラム実行前に
; インタバルタイマ等のハードをスタートしたい時に
; この部分に記述してください。通常は何も記述しなくて
; 結構です。
; ＳＰ以外のレジスタの保存は必要ありません。
; この部分で割り込みを禁止しておけば、ユーザプログラム
; に制御が移るまで割り込み禁止で走ります。
;=======================================================
usrgortn:
	push.l	er2
	push.l	er1
;
; この部分に処理ルーチンを記述します。
;

;-----------------------------------------------
	pop.l	er1
	pop.l	er2

	BANK_NEXT


	BANK 61
gcmd1:
	GETL
	mov.l	er3,SP		; USER STACK
	BANK_NEXT


	BANK 62
	GETL
	push.l	er3		; PC  CCR  ER2  ER0  ER1  ER3

	BANK_NEXT


	BANK 63
	GETL
	mov.l	er3,er4		; ER4
	BANK_NEXT


	BANK 64
	GETL
	mov.l	er3,er5		; ER5
	BANK_NEXT


	BANK 65
	GETL
	mov.l	er3,er6		; ER6(FP)
	pop.l	er3		; kanda
	pop.l	er1
	pop.l	er0
	push.w	r1
	BANK_NEXT


	BANK 66
	BANK_NEXT

	BANK 67

;	bra	skip:8
;	dw	0
skip:
;	pop.l	er3
	pop.w	r1
;	pop.w	r0
        
	mov.b	#RUNON,r2l
	mov.b	@er2,r2l
	pop.l	er2
;	db	0x56,0x0f0	; rtb
	db	0x56,0x070	; rtb


nmientry:
	push.w	r0
	mov.b	#1,r0l			; nmif ON
	push.l	er2
	mov.l	@BRKVECT,er2
	mov.b	#RUNON,r2l
	btst	#02,@er2		; Check HOST_NMI(Break)
	beq	usrg1:8
	btst	#7,@er2
	beq	next_brk:8
	push.l	er1
	mov.b	#CMDRD,r2l
	btst	#0,@er2			; 1char RX

	mov.b	#0xfe,r0l		; prof sign
	mov.b	#(prf00-0x3380),r1l
	bra     (0x3380+prsll):16	; put data(bsr putrs)
prf00:
	mov.l	@(10:16,sp),er1		; PC get
	mov.w	r1,r0
	bra	(0x3380+prof_ent):16
usrg1:
	pop.l	er2
	pop.w	r0
;-- User NMI --
	db	0x5a
usrnmip:
	db	0			; Jump to USR NMI  A23-A20
	dw	0			; Jump to USR NMI  A19-A00

;===== Break/Step End Entry =====
sbrkentry:
#if INTMODE2 /* { */
	orc	#0x7,exr	; H8S Disable to int (Mode 2)
#endif /* } */
	push.w	r0
	mov.b	#0,r0l			; nmif ON
	push.l	er2
	mov.l	@BRKVECT,er2
next_brk:
	mov.b	#RUNOFF,r2l
	mov.b	@er2,r2l
;	push.l	er1
	push.w	r1
;	push.l	er3
;	mov.l	er2,er1
	mov.b	r0l,r1h
	mov.b	#0xff,r0l
	PUTB
	mov.b	r1h,r0l
	PUTB_MAIN

	BANK 68
	BANK_NEXT

	BANK 69
	BANK_NEXT

	BANK 6a			; H8S
	GETW
	push.w	r3		; EXR push

	BANK_NEXT

	BANK 6b			; H8S
	GETL
	LDMAC ER3,MACH		; db	0x03,0x23

	BANK_NEXT

	BANK 6c			; H8S
	GETL
	LDMAC ER3,MACL		; db	0x03,0x33

	BANK_NEXT

	BANK 6e			; H8S
	GETL
	LDC.B R3L,EXR		; H8S EXR

	BANK_NEXT

	BANK 6f			; BREAK - 1
	db	0x02,0x1b	; STC.B EXR,R3L		; H8S
	PUTL_MAIN		; EXR
	
;******************************************
;	BREAK
;******************************************
	BANK 70

	push.l	er3	; kanda
	mov.b	#RUNOFF,r2l
	mov.b	@er2,r0l	; for intr.
	ldc	#0x0C0,CCR	; Disable to interrupt
	BANK_NEXT


	BANK 71
	mov.l	er6,er3
	PUTL_MAIN		; ER6


	BANK 72
	mov.l	er5,er3
	PUTL_MAIN		; ER5


	BANK 73
	mov.l	er4,er3
	PUTL_MAIN		; ER4


	BANK 74
	pop.l	er3
	PUTL_MAIN		; ER3

	BANK 75
	pop.w	r1
	mov.l	er1,er3
	PUTL_MAIN		; ER1

	BANK 76
	pop.l	er3
	PUTL_MAIN		; ER2

	BANK 77
	pop.w	r0
	mov.l	er0,er3
	PUTL_MAIN		; ER0


	BANK 78
	mov.b	@SP+,r0l	; CCR
	PUTB
	mov.b	@SP+,r0l
	PUTB_MAIN		; PC(A23-A16)

	BANK 79
	pop.w	r3
	PUTW			; PC(A15-A00)
	mov.l	SP,er3
	PUTL_MAIN		; SP

	BANK
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;   変更箇所３：ＵＳＲＢＲＫＲＴＮ
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;=================＜ＵＳＲＢＲＫＲＴＮ＞=======================
; ユーザプログラムがブレークした直後にこのルーチン
; が呼ばれます。ユーザプログラムブレーク後に
; インタバルタイマ等のハードをストップしたい時に
; この部分に記述してください。通常何も記述しなくて結構です。
; レジスタの保存は必要ありません。
;=============================================================

usrbrkrtn:
	push.l	er2
	push.l	er1
;
; この部分に処理ルーチンを記述します。
;
;	mov.b	#0x18,r1l
;	mov.b	#0xa5,r1h
;	mov.w	r1,@0xffa8
;-------------------------------------
; この行より下は変更禁止
;-------------------------------------
	pop.l	er1
	pop.l	er2
	BANK_NEXT

	BANK 7b			; H8S
	STMAC MACH,ER3		; db	0x2,0x23

	PUTL_MAIN		; MACH

	BANK 7c			; H8S
	STMAC MACL,ER3		; db	0x2,0x33

	PUTL_MAIN		; MACL

;******************************************
;	USER INIT2
;******************************************
	BANK	7d

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;   変更箇所４：ＵＳＲＩＮＩＴ２
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;==========＜ＵＳＲＩＮＩＴ２＞================================
; ターゲットシステムの初期化ルーチンを記述します。
; この領域には、USRINIT1で記述しきれなかったルーチンを書いてください
;--------------------------------------------------------------
;
; 変更禁止
USRINIT2:
;
; USRINIT1に全ての初期化ルーチンが記述できなかった場合は、
; 残りのルーチンをこの部分に書き加えてください。
; 初期化ルーチンは USRINIT1 , USRINIT2 の順で実行されます。
;

;-------------------------------------
; この行より下は変更禁止
;-------------------------------------
	BANK_NEXT

;******************************************
;	USER INIT1
;******************************************
	BANK	7e

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;   変更箇所４：ＵＳＲＩＮＩＴ１
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;==========＜ＵＳＲＩＮＩＴ＞================================
; ターゲットシステムの初期化ルーチン
; モニタプログラムが動作できるための最低限の初期化
; を行なってください。
; レジスタの保存は必要ありません。
;
; 初期化ルーチンを記述する領域はUSRINI1とUSRINIT2の２ヶ所あります; 
; USRINIT1(この部分)に記述できるバイト数は34Hバイトです。もし、この領域に
; すべてのコードが記述できない場合はUSRINIT2に分けて書いてください。
;============================================================
;
; 変更禁止
USRINIT:

	mov.b	#0xfe,r1l
	mov.b	r1l,@0xffffcd	; CS0,CS1,CS2,CS3 Enable

;
; Ｈ８／３００２でCS3に8ビットRAMが接続
;	mov.b	#0xff,r1l	; CS3 enable
;	mov.b 	r1l,@0xffffcd
;	mov.b	@0xffffec,r1l	; area 3 8bits
;	or.b	#0x8,r1l
;	mov.b	r1l,@0xffffec

; Ｈ８Ｓ／２６５５でCS0,CS1,CS2を有効にする
;	mov.b	#0xfc,r1l	; CS0,CS1,CS2 enable
;	mov.b 	r1l,@0xfebf
#if BUS8 && 1	/* 北斗電子 HSB8S2655-1 */
;	mov.b	#0xfc,r1l	; ROM/RAM 2ステート
;	mov.b 	r1l,@0xfed1	; ASTCR
;	mov.b	#0xf5,r1l	; ROM/RAM 2ステート 1 ウエイト
;	mov.b 	r1l,@0xfed3	; WCRL
#endif

#if BUS16 && 1	/* 北斗電子 HSB8S2655-A */
;	mov.b	#0xfa,r1l	; ROM/RAM 2ステート RTC 3ステート
;	mov.b 	r1l,@0xfed1	; ASTCR
;	mov.b	#0xc0,r1l	; ROM/RAM 2ステート 0 ウエイト
;	mov.b 	r1l,@0xfed3	; WCRL
#endif
;;	mov.b	@0xffec,r1l	; area 3 8bits
;;	or.b	#08h,r1l
;;	mov.b	r1l,@0xffec


; この部分に初期化ルーチンを記述します。
; ＣＰＵリセットの直後にこのルーチンが走ります
; したがって、スタックを含めなんの初期化もまだ行なわれていません。
; 
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
*   変更箇所５：CFGファイルでRESET PCを設定した場合に記述
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
; CFGファイルでRESET PCを設定した場合にはモニタの先頭アドレスを
; ER1及びER2に入れて下さい。
; 例えばモニタの先頭アドレスがE00番地(MONITOR CODE ADDRESS e00)
; の場合は"#define MONADR 1"と記述し、MONADRHにモニタアドレスの
; 上位16ビット、MONADRLにモニタアドレスの下位16ビットをセットし
; 下さい。
; RESET PCを記述していない場合は変更しないで下さい。
; (自動的にリセットベクタのアドレスから取ってきます。)
;

#define MONADR	0
MONADRH equ	0
MONADRL equ	0x0e00

#if  MONADR
	mov.w	#MONADRL,r1	; xxxxxxxx --> er1
	mov.w	#MONADRH,e1
#else
	mov.w	#0,e2		; 0 --> er2
	mov.w	#0,r2
	mov.l	@er2,er1
#endif
	mov.l	er1,er2
	mov.b	#CMDRD,r2l
	mov.b	@er2,r2l

	mov.b	#0x12,r0l
	PUTB_MAIN
;	BANK_NEXT

	BANK
	end
