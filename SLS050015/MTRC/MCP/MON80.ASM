;**********************************************************
;*  ＰＡＲＴＮＥＲ-ＥＴ(80)　モニタプログラム　Ver 1.02   *
;*      ＭＯＮ８０．ＡＳＭ				  *
;*      for CPU:8085/Z80/64180ｼﾘｰｽﾞ	                  *
;*							  *
;*   Copyright 1989 (c) 京都マイクロコンピュータ（株）	  *
;**********************************************************
;------------------------------------------------------------------------------
;          モニタプログラム本体（ＭＯＮ８０．ＥＰＴ）の作成法
;==============================================================================
; 作成方法
;
;	A>pp80 mon80.asm >mon80.src	････ プリプロセス(MON80.ASM->MON80.SRC)
;	A>ia80s				････ アセンブル	 (MON80.SRC->MON86.EPT)
;
; 注意：PARTNER-Tシリーズをすでにご使用のお客様は、モニタプログラムは
;       PARTNER-ET 専用のものが必要です。したがって、PARTNER-T用のモニタと
;       混在しないようにしてください。また、このソースを使って PARTNER-ET専用
;       のモニタを作成してください。

;-----------------------------------------------------------------------------
;　　　　　　　  ユーザのカスタマイズ箇所
;=============================================================================
;  1.ユーザがカスタマイズする必要がある部分は『;++++++･････』で示されています。
;    不用意にその他の箇所を変更／削除／追加してはいけません。
;    また、このファイルを変更する場合は、必ずコピーしてからにしてください。
;　2.＜変更箇所１＞，＜変更箇所２＞がユーザが変更／追加する必要がある部分です。
;  3.＜変更箇所１＞は、指定されたシンボルをターゲットシステムに合わせて設定し
;    てください。
;  4.＜変更箇所２＞では、＜ＵＳＲＩＮＩＴ＞，＜ＵＳＲＧＯＲＴＮ＞，
;    ＜ＵＳＲＢＲＫＲＴＮ＞の合計３このルーチンを必要ならば記述してください。
;  5.ユーザカスタマイズルーチン内でデータ領域が必要な場合は、
;    ＜ユーザ用データ領域＞を使用してください。
;-----------------------------------------------------------------------------


;+++++++++++++++++++++++++++++++++++++++++++++++++++++++
;   変更箇所１：以下のシンボルを設定してください。
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++

;-------------- モニタのコードアドレスの設定 ---------------------------------
; MONADR ････ モニタの使用するコード領域の先頭アドレスを設定してください
;-----------------------------------------------------------------------------
#define MONADR	0x3F00

;-------------- ターゲットＣＰＵの種類を指定します ---------------------------
; CPU85 ...... 8085ｼﾘｰｽﾞの場合 1 にセット
; CPUZ80 ..... Z80(TMPZ84)ｼﾘｰｽﾞの場合 1 にセット
; CPU180 ..... HD64180ｼﾘｰｽﾞの場合 1 にセット
; KC80 ....... KC80(KAWATETSU)の場合 1 にセット
; CPU1600 .... CPU1600(ROHM)の場合 1 にセット
;-----------------------------------------------------------------------------
#define CPU85	0
#define CPUZ80	0
#define CPU180	1
#define KC80	0
#define CPU1600	0

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

{r0}	macro
b	=	0
c	=	8
d	=	10h
e	=	18h
h	=	20h
l	=	28h
a	=	38h
	endm

{r1}	macro
b	=	0
c	=	8
d	=	10h
e	=	18h
h	=	20h
l	=	28h
(hl)	=	30h
a	=	38h
	endm

{d0}	macro
bc	=	4ch
de	=	5ch
hl	=	6ch
sp	=	7ch
	endm

<out0 (*),*> macro x,y
	db	0edh,01h+{r0=y},x
	endm

<in0 *,(*)> macro x,y
	db	0edh,{r0=x},y
	endm

CMDRD	equ	MONADR+45h
CLRNMI	equ	MONADR+40h
STAT	equ	MONADR+41h
RUNOFF	equ	MONADR+47h
RUNON	equ	MONADR+46h
CMDWR0	equ	MONADR+43h
CMDWR1	equ	MONADR+42h
CMDWRE  equ	MONADR+44h

	org	0
	.phase	MONADR

shadowbrk1 equ	$
cmdstart equ	$
	db	01h		; Table Mark				/ 0h
	db	CPUNO		; CPUタイプ(0,1,2=8085,Z80,64180)	/ 1h
	db	7
	dw	MONADR		; モニタのコードスタートアドレス	/ 3h
	dw	0
	dw	usrnmip+67h*40h-MONADR
				; ユーザＮＭＩ（ＴＲＡＰ）pachiアドレス / 7h
	dw	nmientry	; NMI Entry point			/ 9h
	dw	bpret		; SOFT BP Entry(NON SHADOW)		/ bh
	dw	shadowentry	; NMI Entry point(LOW)			/ dh
	dw	shadow_high	; NMI Entry point(HIGH)			/ fh
#if	MONSTID
	dw	0
#else
#if	MONSTIE
	dw	1
#else
#if	MONSTIC
	dw	2
#endif
#endif
#endif
	.dephase

	org	CLRNMI-MONADR
	db	0ffh				;SOFTBRK*8+0c7h

	org	RUNON-MONADR
	ret

;******************************************
;	コモンバンク
;******************************************

	org	48h
	.phase	MONADR+48h

mainlp:
	ld	hl,cmdstart
	nop

;---- 1Byte RX ----
getrs:
	ld	a,(STAT)
	and	8
	jp	z,getrs
getrs1:
	ld	a,(CMDRD)
	jp	(hl)

;---- 1Byte TX ----
putrs:
	ld	c,8
	ld	b,a
prsl:
	ld	a,(STAT)
	and	4
	jp	z,prsl
prsl0:
	ld	a,b
	rla
	ld	b,a
	jp	c,prs1
	ld	a,(CMDWR0)
	jp	prs2
prs1:
	ld	a,(CMDWR1)
prs2:
	dec	c
	jp	nz,prsl0
bnext:
	ld	a,(CMDWRE)
	jp	(hl)

	org	MONADR+7dh
shadow_high:
	jp	shadowentry

;******************************************
;	PEEKB
;******************************************
	BANK	8
peekb:
	GETW		; DE <- addres
	ld	a,(de)
	PUTB_MAIN

;******************************************
;	INPB
;******************************************
	BANK	9

;---- inportB -----
inpb:
	GETW		; DE <- addres

#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	b,d
	ld	c,e
	in	a,(c)
#endif
#if CPU85
;----- in a,immediate --
; in :l..address
; out:a..data
in_im: 
	ex	de,hl
	ld	h,0c9h
	push	hl
	ld	h,0dbh			; in 	a,(nn) , ret
	push	hl
	ld	hl,in_ret
	push	hl
	ld	hl,3
	add	hl,sp
	jp	(hl)
in_ret:
	pop	bc
	pop	bc
#endif
	PUTB_MAIN


;******************************************
;	PEEKW
;******************************************
	BANK	10
peekw:
	GETW		; DE <- addres
	ld	a,(de)
	PUTB
	inc	de
	ld	a,(de)
	PUTB_MAIN

;******************************************
;	POKEB
;******************************************
	BANK	18
pokeb:
	GETW		; DE <- addres
	GETB
	ld	(de),a
	BANK_NEXT


;******************************************
;	OUTB
;******************************************
	BANK	19
outpb:
	GETW			; DE <- addres
	GETB			; a <- data
#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	b,d
	ld	c,e
	out	(c),a
#endif
#if CPU85
out_im:
	ex	de,hl
	ld	h,0c9h			; ret
	push	hl
	ld	h,0d3h			; out (nn),a
	push	hl
	ld	hl,out_ret
	push	hl
	ld	hl,3
	add	hl,sp
	jp	(hl)
out_ret:
	pop	af
	pop	af
#endif
	BANK_NEXT


;******************************************
;	POKEW
;******************************************
	BANK	20
pokew:
	GETW		; DE <- addres
	GETB
	ld	(de),a
	inc	de
	GETB
	ld	(de),a
	BANK_NEXT


;******************************************
;	GETBLK
;******************************************
	BANK	28
getblk:
	GETW		; DE <- addres
gblklp:
	ld	a,(de)
	inc	de
	PUTB
	GETB
	or	a
	jp	nz,gblklp
	BANK_NEXT


;******************************************
;	PUTBLK
;******************************************
	BANK	30

putblk:
	GETW		; DE <- address
pblklp:
	GETB
	ld	(de),a
	inc	de
	GETB
	or	a
	jp	nz,pblklp
	BANK_NEXT


;******************************************
;	SPSET
;******************************************
	BANK	50

	GETW
	ex	de,hl
	ld	sp,hl		; USER STACK
	ld	a,(CLRNMI)	; NMI Request CLR
	ld	a,(RUNOFF)
	BANK_NEXT


;******************************************
;	FLGSET
;******************************************
	BANK	51
flgset:
	GETB
	or	a
	jp	z,flgs1
	ei
	jp	flgs2
flgs1:
	di
flgs2:
	BANK_NEXT

#if CPUZ80 || CPU180 || KC80 || CPU1600
;******************************************
;	MMUGET_KC80
;******************************************
	BANK	52
mmuget_kc80:

	ld	bc,0
mmg_kc0:
	in	a,(c)
	exx
	PUTB
	exx
	inc	c
	ld	a,c
	cp	7
	jr	nz,mmg_kc0
	in	a,(c)
	PUTB_MAIN

;******************************************
;	MMUSET_KC80
;******************************************
	BANK	53
mmuset_kc80:

	ld	bc,0
mms_kc0:
	exx
	GETB
	exx
	out	(c),a
	inc	c
	ld	a,c
	cp	8
	jr	nz,mms_kc0
	BANK_NEXT

;******************************************
;	MMUGET_64180
;******************************************
	BANK	54
mmuget_64180:

	ld	bc,38h
mmg_640:
	in	a,(c)
	exx
	PUTB
	exx
	inc	c
	ld	a,c
	cp	38h+2
	jr	nz,mmg_640
	in	a,(c)
	PUTB_MAIN

;******************************************
;	MMUSET_64180
;******************************************
	BANK	55
mmuset_64180:

	ld	bc,38h
mms_640:
	exx
	GETB
	exx
	out	(c),a
	inc	c
	ld	a,c
	cp	38h+3
	jr	nz,mms_640
	BANK_NEXT
#endif

;******************************************
;	MMUGET_64180S
;******************************************
	BANK	56
mmuget_64180S:

	ld	bc,1h
mmg_640S:
	in	a,(c)
	exx
	PUTB
	exx
	inc	c
	ld	a,c
	cp	1h+2
	jr	nz,mmg_640S
	in	a,(c)
	PUTB_MAIN

;******************************************
;	MMUSET_64180S
;******************************************
	BANK	57
mmuset_64180S:

	ld	bc,1h
mms_640S:
	exx
	GETB
	exx
	out	(c),a
	inc	c
	ld	a,c
	cp	1h+3
	jr	nz,mms_640S
	BANK_NEXT
#endif

;******************************************
;	GCMD
;******************************************
	BANK	60

gcmd:
;================＜ＵＳＲＧＯＲＴＮ＞===================
; Ｇコマンド等でユーザプログラムが実行される直前に
; このルーチンが呼ばれます。ユーザプログラム実行前に
; インタバルタイマ等のハードをスタートしたい時に
; この部分に記述してください。通常は何も記述しなくて
; 結構です。
; ＳＰ以外のレジスタの保存は必要ありません。
; この部分で割り込みを禁止しておけば、ユーザプログラム
; に制御が移るまで割り込み禁止で走ります。
;========================================================
usrgortn:
;
; この部分に処理ルーチンを記述します。
;

;-----------------------------------------------

	BANK_NEXT

	BANK
gcmd1:
	di
	GETW
	ex	de,hl
	ld	sp,hl		; USER STACK
	GETW
	push	de		; PC
	GETW
	push	de		; AF
	BANK_NEXT


	BANK
	GETW			; AF'
	push	de
	pop	af
	ex	af,af'
	GETW			; BC'
	push	de
	pop	bc
	exx
	GETW			; DE'
	push	de
	exx
	pop	de
	exx
	BANK_NEXT


	BANK
	GETW			; HL
	push	de
	exx
	pop	hl
	exx
	GETW			; IX
	push	de
	pop	ix
	GETW			; IY
	push	de
	pop	iy
	BANK_NEXT


	BANK

	GETW			; BC
	push	de
	GETW			; HL
	push	de
	GETW			; DE
	BANK_NEXT


	BANK

	GETB
#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	i,a
#endif
#if CPU85
;;	and	07h		; M7.5-5.5
;;	or	8		; set MSE
	db	30h		; SIM
#endif
	GETB
	ld	b,a
	BANK_NEXT


	BANK

	GETB
#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	i,a
#endif
#if CPU85
;;	and	07h		; M7.5-5.5
;;	or	8		; set MSE
	db	30h		; SIM
#endif
	GETB
	ld	b,a
	ld	hl,shadow_gcmd
	ld	a,(CMDWRE)
	GETB_BASE


;************************
;****** NON SHADOW ******
;************************
	BANK	67

	xor	a
	or	b
#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	a,(RUNON)		; @ i 1994.4.14
#endif
	jp	z,1$
	ei
1$:
	pop	hl
	pop	bc
#if CPUZ80 || CPU180 || KC80 || CPU1600
;	ld	a,(RUNON)		; @@@ d 1994.4.14
	pop	af
	ret
#endif
#if CPU85
	pop	af
	jp	RUNON
#endif

nmientry:
	push	af
	ld	a,(STAT)		; HOST_NMI(Break)
	and	2			; Check HOST_NMI(Break)
	jp	nz,nmibrk
;-- User NMI --
	pop	af
usrnmip:
	jp	0			; jmp to USR NMI

nmibrk:
	ld	a,(CLRNMI)		; NMI Request CLR
	ld	a,(RUNOFF)

	ld	a,1
#if CPUZ80 || CPU180 || KC80 || CPU1600
	jr	next_brk
#endif
#if CPU85
	jp	next_brk
#endif

;===== Break/Step End Entry =====
bpret:
	push	af
	ld	a,(RUNOFF)
#if CPU85
	db	20h		; RIM
	di
#endif
#if CPUZ80 || CPU180 || KC80 || CPU1600
;--eidichk--
	ld	a,i
	jp	pe,1$		; EI
	ld	a,i		; retry for Z80/64180R0 Bug
	jp	pe,1$		; EI
	ld	a,i		; retry for Z80/64180R0 Bug
1$:
	di
#endif
	push	af		; I_MASK & (CY=0 not NMI)
	xor	a
next_brk:
	push	bc
	push	hl
	PUTB_MAIN


;********************
;****** SHADOW ******
;********************
	BANK

shadowentry:
;--- NMI entry ---
	push	af
	ld	a,(STAT)		; HOST_NMI(Break)
	and	2			; Check HOST_NMI(Break)
	jp	z,shadow_sbrk

	ld	a,(RUNOFF)
	ld	a,(CLRNMI)		; NMI Request CLR
	ld	a,1
	jp	shadow_next

;===== Break/Step End Entry =====
shadow_sbrk:
	ld	a,(RUNOFF)
#if CPU85
	db	20h		; RIM
	di
#endif
#if CPUZ80 || CPU180 || KC80 || CPU1600
;--eidichk--
	ld	a,i
	jp	pe,1$		; EI
	ld	a,i		; retry for Z80/64180R0 Bug
	jp	pe,1$		; EI
	ld	a,i		; retry for Z80/64180R0 Bug
1$:
	di
#endif
	push	af		; I_MASK & (CY=0 not NMI)
	xor	a
shadow_next:
	push	bc
	push	hl
	PUTB_MAIN

shadow_gcmd:
	xor	a
	or	b
	jp	z,1$
	ei
1$:
	pop	hl
	pop	bc
#if CPUZ80 || CPU180 || KC80 || CPU1600
	ld	a,RUNON-4
	ld	r,a
#endif
	pop	af
	jp	RUNON


;******************************************
;	BREAK
;******************************************
	BANK	70

#if CPUZ80 || CPU180 || KC80 || CPU1600
;--eidichk--
	ld	a,i
	jp	pe,eidiend	; EI
	ld	a,i		; retry for Z80/64180R0 Bug
	jp	pe,eidiend	; EI
	ld	a,i		; retry for Z80/64180R0 Bug
eidiend:
	push	af		; I_reg & EI/DI
#endif
#if CPU85
	db	20h		; RIM
	push	af		; RIM PUSH
#endif
	PUTW			; DE
	pop	de
	PUTW_MAIN		; IM


	BANK

	ld	a,(RUNOFF)	; for intr.
	PUTW			; DE
	pop	hl
	pop	bc
	pop	de
	push	bc
	push	hl
	PUTW_MAIN		; IM


	BANK

	pop	de
	PUTW			; HL
	pop	de
	PUTW			; BC
	pop	de
	PUTW_MAIN		; AF


	BANK

	pop	de
	PUTW			; PC
	ld	hl,0
	add	hl,sp
	ex	de,hl
	PUTW			; SP
	GETB			; IEF
#if	MONSTID
	di
#else
#if	MONSTIE
	ei
#else
#if MONSTIC			/* 割り込み状態継続 */
#if CPUZ80 || CPU180 || KC80 || CPU1600
	and	4
	jr	z,intdi1
	ei
#endif
#if CPU85			/* 8085 */
	and	8
	jp	z,intdi1
	ei
#endif
intdi1:
#endif
#endif
#endif
	BANK_NEXT


	BANK

	ex	af,af'
	push	af
	pop	de
	PUTW			; AF'
	exx
	push	bc
	exx
	pop	de
	PUTW			; BC'
	exx
	push	de
	exx
	pop	de
	PUTW_MAIN		; DE
	BANK_NEXT

	BANK

	exx
	push	hl
	exx
	pop	de
	PUTW			; HL'
	push	ix
	pop	de
	PUTW			; IX
	push	iy
	pop	de
	PUTW_MAIN		; IY


	BANK
;================＜ＵＳＲＢＲＫＲＴＮ＞=======================
; ユーザプログラムがブレークした直後にこのルーチン
; が呼ばれます。ユーザプログラムブレーク後に
; インタバルタイマ等のハードをストップしたい時に
; この部分に記述してください。通常何も記述しなくて結構です。
; ＳＰ以外のレジスタの保存は必要ありません。
;=============================================================

usrbrkrtn:
;
; この部分に処理ルーチンを記述します。
;
	BANK_NEXT

;******************************************
;	USER INIT2
;******************************************
	BANK	7d

;==========＜ＵＳＲＩＮＩＴ２＞================================
; ターゲットシステムの初期化ルーチンを記述します。
; この領域には、USRINIT1で記述しきれなかったルーチンを書いてください
;--------------------------------------------------------------
;
USRINIT2:			; 変更禁止
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

;==========＜ＵＳＲＩＮＩＴ＞================================
; ターゲットシステムの初期化ルーチン
; モニタプログラムが動作できるための最低限の初期化
; を行なってください。
; 例えば、
;  1.ＤＲＡＭのリフレシュコントローラの初期化(64180etc.)
;  2.ＷＡＩＴコントローラの初期化(64180etc.)
;  3.その他必要なハードウェアの最低限の初期化
; レジスタの保存は必要ありません。
;
; 初期化ルーチンを記述する領域はUSRINI1とUSRINIT2の２ヶ所あります 
; USRINIT1(この部分)に記述できるバイト数は37Hバイトです。もし、この領域に
; すべてのコードが記述できない場合はUSRINIT2に分けて書いてください。
;============================================================
USRINIT:
;----------------------------------------
; この部分に初期化ルーチンを書き加えます
;----------------------------------------
;-------- 64180 Sample (0WAIT , No Refresh) -------
	xor	a
	out0	(32h),a
	out0	(36h),a

	ld	a,0c8h		; BANK_LOW .. 0x4000 , COMMON_LOW .. 0xc000
	out0	(3ah),a

	ld	a,14h		; 0x3c000 - 0xc000 = 0x30000 ( COMMON BASE )
	out0	(38h),a
;-------------------------------------------------
;--------- TMPZ84C015BF --------------------------
;	LD	A,03h
;	OUT	(0F0h),A
;	LD	A,0B1h
;	OUT	(0F1h),A
;
;======== KC80 Sample (NMI,MMU) ===========
;	ld	a,0fh		; B1 = 0f ( 0x4000 )
;	out	(00h),a
;	ld	a,032h		; B4 = 32 ( 0xCC00 )
;	out	(06h),a
;	ld	a,08h
;	out	(3bh),a
;==========================================
;
;-------------------------------------
; この行より下は変更禁止
;-------------------------------------
	ld	a,(CMDRD)
	BANK_NEXT

	BANK

	end
