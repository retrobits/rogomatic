/*
 * termtokens.h: Rog-O-Matic XIV (CMU) Fri Dec 28 22:16:05 1984 - mlm
 * Copyright (C) 1985 by A. Appel, G. Jacobson, L. Hamey, and M. Mauldin
 *
 * Various tokens used by the screen reading package.
 */

# define BS_TOK ctrl('H')       /* Move left by one */
# define CE_TOK -2              /* Clear screen */
# define CL_TOK ctrl('L')       /* Clear screen and go to home 0,0 */
# define CM_TOK -3              /* Move to position number1, number2 */
# define CR_TOK ctrl('M')       /* Move to column 0 */
//# define DO_TOK -4
# define ER_TOK -5
# define LF_TOK ctrl('J')       /* Line feed, move down one row */
# define ND_TOK -6              /* Move down number1 rows */
# define SE_TOK -7              /* End standout mode */
# define SO_TOK -8              /* Start standout mode */
# define TA_TOK ctrl('I')       /* Tab to next hw stop */
# define UP_TOK -9              /* Move up one row */
# define HM_TOK -10             /* Go to home 0,0 */
# define CH_TOK -11             /* Change scroll region to number1 - number2 */
# define NU_TOK -12             /* Move up number1 rows */
# define NR_TOK -13             /* Move right number1 cols */
# define NL_TOK -14             /* Move left number1 cols */
# define SC_TOK -15             /* Save cursor */
# define RC_TOK -16             /* Restore cursor */
# define SR_TOK -17             /* Scroll reverse */
# define CB_TOK -18             /* Clear to beginning of line */

