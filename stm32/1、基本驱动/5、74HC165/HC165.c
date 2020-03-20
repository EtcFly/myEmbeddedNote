uint8_t bsp_keyScan(void)
{
   uint8_t keyV, i;
	  keyV = 0x00;
	  PL_REST;
//	  main_DelayUs(1000);
	  PL_SET;
	  for (i=0; i < 8; i++)
	  {
			   keyV |= (Q7_IN<<i);

				  CLK_SET;
				  main_DelayUs(1);
				  CLK_RESET;
				  main_DelayUs(1);
			}
			return keyV;
}