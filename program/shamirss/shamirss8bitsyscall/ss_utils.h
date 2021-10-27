/*
 * parameters of Secret Sharing
 */
typedef struct SS_param {
	int k;
	int n;
} SS_param;

/*
 * operations of secret sharing
 */
extern void split(char *path, int *GF_vector);
extern void combine(char *path[], int shareNum, int *GF_vector);

/*
 * generating functions to prepare secret sharing
 */
extern void generate_server_id(int *serverId, int n);
extern void generate_polynomial(int *poly, int secret, int k);

/*
 * create shares
 */
extern void create_shares(int *serverId, int *poly, int *shares, SS_param SS, int *GF_vector);
