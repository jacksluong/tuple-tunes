void get_next_note(){
    // js returns 0, 1U, 2R, 3D, 4L for idle, up, right, down, left (cardinal directions) respectively
  

  if (js == 2){
      scale_index = (scale_index + 1) % 8;
      note_index = note_index + scale_steps[scale_index];
      
  }

  if (js == 4){
    scale_index = (scale_index + 7) % 8; //same as subtracting 1
  }
  
  selected_key = 
}
