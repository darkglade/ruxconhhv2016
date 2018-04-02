#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//       RRRR   U   U  X   X  BBBB    AAA   DDDD    GGGG  EEEEE
//       R   R  U   U   X X   B   B  A   A  D   D  G      E
//       RRRR   U   U    X    BBBB   AAAAA  D   D  G  GG  EEEE
//       R  R   U   U   X X   B   B  A   A  D   D  G   G  E
//       R   R   UUU   X   X  BBBB   A   A  DDDD    GGGG  EEEEE
//==============================================================================

#define RUXBADGE_BITS          56 // 56
#define RUXBADGE_HDR_MARK     900 // 1750
#define RUXBADGE_HDR_SPACE   1800 // 3502
#define RUXBADGE_BIT_MARK     250 // 502
#define RUXBADGE_ONE_SPACE    700 //1244
#define RUXBADGE_ZERO_SPACE   300 // 400

//+=============================================================================
#if SEND_RUXBADGE
void  IRsend::sendRuxBadge (unsigned long address,  unsigned long data)
{
	// Set IR carrier frequency
	enableIROut(35);

	// Header
	mark(RUXBADGE_HDR_MARK);
	space(RUXBADGE_HDR_SPACE);

	// Address
	for (unsigned long  mask = 1UL << (24 - 1);  mask;  mask >>= 1) {
		mark(RUXBADGE_BIT_MARK);
		if (address & mask)  space(RUXBADGE_ONE_SPACE) ;
		else                 space(RUXBADGE_ZERO_SPACE) ;
    }

	// Data
	for (unsigned long  mask = 1UL << (32 - 1);  mask;  mask >>= 1) {
        mark(RUXBADGE_BIT_MARK);
        if (data & mask)  space(RUXBADGE_ONE_SPACE) ;
        else              space(RUXBADGE_ZERO_SPACE) ;
    }

	// Footer
    mark(RUXBADGE_BIT_MARK);
    space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
#if DECODE_RUXBADGE
bool  IRrecv::decodeRuxBadge (decode_results *results)
{
    unsigned long long  data   = 0;
    int                 offset = 1;

    if (!MATCH_MARK(results->rawbuf[offset++], RUXBADGE_HDR_MARK ))  return false ;
    if (!MATCH_MARK(results->rawbuf[offset++], RUXBADGE_HDR_SPACE))  return false ;

    // decode address
    for (int i = 0;  i < RUXBADGE_BITS;  i++) {
        if (!MATCH_MARK(results->rawbuf[offset++], RUXBADGE_BIT_MARK))  return false ;

        if      (MATCH_SPACE(results->rawbuf[offset],RUXBADGE_ONE_SPACE ))  data = (data << 1) | 1 ;
        else if (MATCH_SPACE(results->rawbuf[offset],RUXBADGE_ZERO_SPACE))  data = (data << 1) | 0 ;
        else                                                                 return false ;
        offset++;
    }

    results->value       = (unsigned long)data;
    results->address     = (unsigned long)(data >> 32);
    results->decode_type = RUXBADGE;
    results->bits        = RUXBADGE_BITS;

    return true;
}
#endif

