/* �G���R�[�_�̃p���X�̃J�E���g  */
/* �쐬���J�n�� 2016.07.02      */

#ifndef ENC_H
#define ENC_H

class ENC{

private:

	int pulses;  // �G���R�[�_�̃p���X�̃J�E���g
	int old_a;   // �O�̃s���̏��
	int old_b;
	int now_a;   // ���݂̃s���̏��
	int now_b;
	int old_bit; // �O��̃r�b�g
	int now_bit; // ����̃r�b�g
	int chan_a;  // A���̃s��
	int chan_b;  // B���̃s��
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
	
	/* �p���X�̃J�E���g                   */
	/* ���� ���̊֐���loop�̒��Ŏg�p����  */
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

	/* ���荞�݃s�����g�p�����J�E���g */
	/* 12�J�E���g                   */
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