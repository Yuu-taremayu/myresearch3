/*
 * define message of usage
 */
#define USAGE "Usage: %s \n\t --mode=split -n=num -k=num FILE \n\t --mode=combine FILE [...]\n"

/*
 * define extension of share file
 */
#define EXT ".share"
#define EXTLEN (int)strlen(EXT)

/*
 * Galois field size 2^8
 * also use modulo num
 */
#define FIELD_SIZE 256

/*
 * Bit mask
 * use BIT_MASK for truncate upper bit
 */
#define BIT_MASK 0xff

/*
 * for reading shares
 */
#define BUFSIZE 2
