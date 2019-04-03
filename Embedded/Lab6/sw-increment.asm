; Assembly file for increment() function
; Lab 3, ECE 266, fall 2017
;Arsalan Babar and Imran Babar
; Created by Zhao Zhang

; Function prototype: stopWatchIncrement(displayState_t *pDisplayState);
; This function increment the number shown on the stop watch display. It should be called once every one centisecond.
					.text
					.global stopWatchIncrement	; make this symbol global
stopWatchIncrement	.asmfunc

					; Update c2, continue to c1 if c2 is reset to zero
					LDRB	r1, [r0, #0]	    ; load c2 to r1, note c2's offset is zero which can be omitted
					ADD		r1, #1				; increment c2 by one
					STRB	r1, [r0, #0]	    ; store back c2
					CMP		r1, #10				; compare c2 and 10
					BNE		return				; if c2 != 10, jump to return
					MOV		r1, #0				; reset c2 to zero
					STRB		r1, [r0, #0]	; store back again

					; YOUR CODE: update c1, s2, and s1
					LDRB	r1, [r0, #1]	; load c1 to r1, c2's offset is zero which is not used
					ADD		r1, #1			; incremented c2
					STRB	r1, [r0, #1]	; loaded back into c2
					CMP		r1, #10			; made a comparison between c2 and 10
					BNE		return			; if c2 != 10, it should go to return
					MOV		r1, #0			; c2 should be reset to zero
					STRB	r1, [r0, #1]

					LDRB	r1, [r0, #2]	; loaded s1 to r1, note s2's offset is zero and doesnt need to be used.
					ADD		r1, #1			; incremented s1
					STRB	r1, [r0, #2]	; loaded back into s1
					CMP		r1, #10			; made a comparison between s1 and 10
					BNE		return			; if s1 != 10, it should go to return
					MOV		r1, #0			; reset s1
					STRB	r1, [r0, #2]

					LDRB	r1, [r0, #3]	; load s2 to r1, s2's offset is zero which doesnt need to used
					ADD		r1, #1			; increment s1
					STRB	r1, [r0, #3]	; store s1
					CMP		r1, #6			; made a comparison between s1 and 10
					BNE		return			; if s1 != 10, it should go to return
					MOV		r1, #0		    ; reset s2 to two
					STRB	r1, [r0, #3]

return				BX		LR					; return
					.endasmfunc

					.end
