/////////////
//  Audio  //
/////////////

double note_freqs_mid[] = {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392.00, 415.30, 440.00, 466.16, 493.88};

void play_note(int index) {
  ledcWriteTone(AUDIO_PWM, note_freqs_mid[index]);
}

void stop_sound() {
  ledcWriteTone(AUDIO_PWM, 0);
}
