		 
		 /*This vcode prodeuced nice scrolling effect*/
		 case UP:
/*	Collect bit 0 data to temp array		(col 0 to 126)*/
				for(col = 0 ;  col <=126 ; col++)// going upto 126 only,else if for 127 it can oveflow
				{
					temp[col/8] <<=1;
					temp[col/8]  |= GlcdBuffer[0][col] & 0x01;
				}	
/*	Collect bit 0 data to temp array		(col 127)*/ 
//couldnt involve in above loop bcoz due to 127 it will overflow and result in an infinite loop
				temp[15] <<=1;
				temp[15] |= GlcdBuffer[0][127] & 0x01;
/*	Now shift all data  	*/					
			for(page = 0 ;  page < 7 ; page++)
				for(col = 0 ;  col <=126 ; col++)// for col 0 to 126 only
				{
					GlcdBuffer[page][col] <<= 1;
					GlcdBuffer[page][col]  |= ((GlcdBuffer[page+1][col] & 0x01) << 7);
				}	
/*	Collect bit 0 data to temp array		(col 0 to 126)*/
				for(col = 0 ;  col <=126 ; col++)// going upto 126 only,else if for 127 it can oveflow
				{
					GlcdBuffer[7][col] |= (( temp[col/8] >> (col % 7 ) ) & 0x01) << 7;
				}
					

/*this function reflects half image*/
void GlcdInvert()
{
	uchar page,bit,temp[16] = {0};
	uint col;
	for(page=0;page<4;page++)
		{
		for(bit=0 ; bit < 8 ; bit++)
			{
				for(col=0 ; col < 128 ; col++)
				{
					temp[col/8] <<=1;
					temp[col/8] |= (GlcdBuffer[page][col] >> bit)  & 0x01;
				}
				for(col=0 ; col < 128 ; col++)
				{
					GlcdBuffer[7-page][col] <<= 1;
					GlcdBuffer[7-page][col] |= (( temp[col/8] >> (7 - (col % 8) ) ) & 0x01);//<< 7;
				}		
			}
		}	
	GlcdUpdateBufferOnLCD();			
}