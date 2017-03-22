unsigned long far_ptr(unsigned int,unsigned int);

unsigned long far_ptr(segment,offset)
  unsigned int segment;
  unsigned int offset;
    {
      unsigned long result;

      result=(unsigned long) segment;
      result*=(unsigned long) 0x10000;
      result+=(unsigned long) offset;
      return(result);
    }
