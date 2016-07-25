/* エンコーダのパルスのカウント  */
/* 作成日開始日 2016.07.02      */

#ifndef ENC_H
#define ENC_H

class ENC{

private:

	int pulses;  // エンコーダのパルスのカウント
	int old_a;   // 前のピンの状態
	int old_b;
	int now_a;   // 現在のピンの状態
	int now_b;
	int old_bit; // 前回のビット
	int now_bit; // 今回のビット
	int chan_a;  // A相のピン
	int chan_b;  // B相のピン
	int mode;    // 1:pullup 2:not pullup

public:

	ENC(int pin1, int pin2, int MODE) : chan_a(pin1), chan_b(pin2), pulses(0), old_a(0), old_b(0), now_a(0), now_b(0), old_bit(0), now_bit(0), mode(MODE)
	{
		pinMode(chan_a, INPUT_PULLUP);  
		pinMode(chan_b, INPUT_PULLUP);

		old_a = digitalRead(chan_a);    
		old_b = digitalRead(chan_b);

		if (mode == 1){
			if ((old_a == LOW) && (old_b == LOW))        old_bit = 0x03;
			else if ((old_a == LOW) && (old_b == HIGH))  old_bit = 0x02;
			else if ((old_a == HIGH) && (old_b == LOW))  old_bit = 0x01;
			else if ((old_a == HIGH) && (old_b == HIGH)) old_bit = 0x00;
		}else if (mode == 2){
			if ((old_a == HIGH) && (old_b == HIGH))     old_bit = 0x03;
			else if ((old_a == HIGH) && (old_b == LOW)) old_bit = 0x02;
			else if ((old_a == LOW) && (old_b == HIGH)) old_bit = 0x01;
			else if ((old_a == LOW) && (old_b == LOW))  old_bit = 0x00;
		}
	}
	
	/* パルスのカウント                   */
	/* 注意 この関数はloopの中で使用する  */
	inline void enc_count(void)
	{
		now_a = digitalRead(chan_a);
		now_b = digitalRead(chan_b);

		if (mode == 1){
			if ((now_a == LOW) && (now_b == LOW))        now_bit = 0x03;
			else if ((now_a == LOW) && (now_b == HIGH))  now_bit = 0x02;
			else if ((now_a == HIGH) && (now_b == LOW))  now_bit = 0x01;
			else if ((now_a == HIGH) && (now_b == HIGH)) now_bit = 0x00;
		}else if (mode == 2){
			if ((now_a == HIGH) && (now_b == HIGH))      now_bit = 0x03;
			else if ((now_a == HIGH) && (now_b == LOW))  now_bit = 0x02;
			else if ((now_a == LOW) && (now_b == HIGH))  now_bit = 0x01;
			else if ((now_a == LOW) && (now_b == LOW))   now_bit = 0x00;
		}

		if ((old_bit == 0x02) && (now_bit == 0x03)) pulses++;
		else if ((old_bit == 0x03) && (now_bit == 0x01)) pulses++;
		else if ((old_bit == 0x01) && (now_bit == 0x00)) pulses++;
		else if ((old_bit == 0x00) && (now_bit == 0x02)) pulses++;
		else if ((old_bit == 0x00) && (now_bit == 0x01)) pulses--;
		else if ((old_bit == 0x01) && (now_bit == 0x03)) pulses--;
		else if ((old_bit == 0x03) && (now_bit == 0x02)) pulses--;
		else if ((old_bit == 0x02) && (now_bit == 0x00)) pulses--;
		
		old_bit = now_bit;
	}

	/* 割り込みピンを使用したカウント */
	/* 12カウント                   */
	inline void encode(void)
	{
		now_a = digitalRead(chan_a);
		now_b = digitalRead(chan_b);

		if (mode == 1){
			if ((now_a == LOW) && (now_b == LOW))        now_bit = 0x03;
			else if ((now_a == LOW) && (now_b == HIGH))  now_bit = 0x02;
			else if ((now_a == HIGH) && (now_b == LOW))  now_bit = 0x01;
			else if ((now_a == HIGH) && (now_b == HIGH)) now_bit = 0x00;
		}
		else if (mode == 2){
			if ((now_a == HIGH) && (now_b == HIGH))      now_bit = 0x03;
			else if ((now_a == HIGH) && (now_b == LOW))  now_bit = 0x02;
			else if ((now_a == LOW) && (now_b == HIGH))  now_bit = 0x01;
			else if ((now_a == LOW) && (now_b == LOW))   now_bit = 0x00;
		}

		if ((old_bit == 0x02) && (now_bit == 0x02)) pulses++;
		else if ((old_bit == 0x01) && (now_bit == 0x01)) pulses++;
		else if ((old_bit == 0x03) && (now_bit == 0x03)) pulses--;
		else if ((old_bit == 0x00) && (now_bit == 0x00)) pulses--;

		old_bit = now_bit;

	}
	
	inline int get_pulses(void)
	{
		return pulses;
	}

	inline void reset(void)
	{
		pulses = 0;
	}

};

#endif