/*
 * Rog-O-Matic
 * Automatically exploring the dungeons of doom.
 *
 * Copyright (C) 2008 by Anthony Molinaro
 *
 * This file is part of Rog-O-Matic.
 *
 * Rog-O-Matic is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Rog-O-Matic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rog-O-Matic.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "termtokens.h"
#include "getroguetoken.h"

FILE *logfile;		/* Rogomatic score file */
int logging;		/* True if logging game */
int replaying;		/* True if replaying old game */

static char screen00 = ' ';

int   atrow  = 0;
int   atcol  = 0;    /* Current position of the Rogue (@) */
int   atrow0 = 0;
int   atcol0 = 0;  /* Position at start of turn */
char  screen[24][80];  /* Map of current Rogue screen */
int   row, col;        /* Current cursor position */
int   s_row1 = 0;
int   s_row2 = 0;

char getroguetoken (void);

void
scrollup (void)
{
  int r;
  int c;
  printf ("scroll up %d - %d\n",s_row1, s_row2);
  for (r = s_row1; r < s_row2; r++)
    {
      printf ("copy row %d to %d\n",(r+1), (r));
      for (c = 0; c < 80; c++)
        {
          screen[r][c] = screen[r+1][c];
        }
    }
  printf ("blank row %d\n",s_row2);
  for (c = 0; c < 80; c++)
    {
      screen[s_row2][c] = ' ';
    }
}

void
scrolldown (void)
{
  int r;
  int c;
  printf ("scroll down %d - %d\n",s_row1, s_row2);
  for (r = s_row2; r > s_row1; r--)
    {
      for (c = 0; c < 80; c++)
        {
          screen[r][c] = screen[r-1][c];
        }
    }
  for (c = 0; c < 80; c++)
    {
      screen[s_row1][c] = ' ';
    }
}

void
printscreen (void)
{
  int i, j;
  printf ("-- cursor  [%2d, %2d] [%c] [%3d] -------------------------------------------------\n", row, col, screen[row][col], screen[row][col]);
  printf ("             1111111111222222222233333333334444444444555555555566666666667777777777\n");
  printf ("   01234567890123456789012345678901234567890123456789012345678901234567890123456789\n");

  for (i=0; i < 24; ++i)
    {
      printf ("%02d",(i));
      if (i >= s_row1 && i <= s_row2)
        {
          printf ("*");
        }
      else
        {
          printf (" ");
        }
      for (j = 0; j < 80; ++j)
        {
          if (i == row && j == col)
            printf ("_");
          else
            printf ("%c", screen[i][j]);
        }
      printf ("\n");
    }
  printf ("--------------------------------------------------------------------------------\n");

}

void
getrogue
  ( char *waitstr,                          /* String to synchronize with */
    int   onat                              /* 0 ==> Wait for waitstr 
                                               1 ==> Cursor on @ sufficient 
                                               2 ==> [1] + send ';' when ever
                                               we eat a --More-- message */
  )
{
  register int i;
  char  ch, *s;
  int *doors;

  atrow0 = atrow;
  atcol0 = atcol;           /* Save our current posistion */
  s = waitstr;              /* FSM to check for the wait msg */

  /* While we have not reached the end of the Rogue input, read */
  /* characters from Rogue and figure out what they mean.       */
  while ((*s) || (onat && screen[row][col] != '@'))
    {
      ch = getroguetoken ();

      /* Check to see whether we have read the synchronization string */
      if (*s)
        {
          if (ch == *s)
            s++;
          else
            s = waitstr;
        }

      /* Now figure out what the token means */

      switch (ch)
        {
          case BS_TOK:
            col--;
            fprintf (stderr, "BS_TOK [%d, %d]\n",row,col);
            break;

          case CB_TOK:
            for (i =0; i < col; i++)
              {
                screen[row][i] = ' ';
              }
            break;

          case CE_TOK:
            if (row && row < 23)
              for (i = col; i < 80; i++)
                {
                  screen[row][i] = ' ';
                }
            else
              for (i = col; i < 80; i++)
                screen[row][i] = ' ';

            if (col == 0)
              screen00 = ' ';
            fprintf (stderr, "CE_TOK [%d, %d]\n",row,col);
            break;

          case CH_TOK:
            fprintf (stderr, "CH_TOK\n");
            break;

          case CL_TOK:
            for (row = 0; row < 24; ++row)
              for (col = 0; col < 80; ++col)
                screen[row][col] = ' ';
            row = 0;
            col = 0;
            fprintf (stderr, "CL_TOK [%d, %d]\n",row,col);
            break;

          case CM_TOK:
            row = number1-1;
            col = number2-1;
            fprintf (stderr, "MV_TOK [%d, %d] [%d,%d]\n",number1, number2,row,col);
            break;

          case CR_TOK:
            col = 0;
            fprintf (stderr, "CR_TOK [%d, %d]\n",row,col);
            break;

          case ER_TOK: 
            break;

          case LF_TOK:
            row++;
            fprintf (stderr, "LF_TOK [%d, %d]\n",row,col);
            break;

          case ND_TOK:
            row += number1;
            fprintf (stderr, "ND_TOK[%d] [%d, %d]\n",number1,row,col);
            break;

          case SE_TOK: 
            fprintf (stderr, "SE_TOK\n");
            break;

          case SO_TOK: 
            fprintf (stderr, "SO_TOK\n");
            break;

          case TA_TOK:
            col = 8 * (1 + col / 8);
            fprintf (stderr, "TA_TOK [%d, %d]\n",row,col);
            break;


          case UP_TOK:
            row--;
            fprintf (stderr, "UP_TOK [%d, %d]\n",row,col);
            break;

          case HM_TOK:
            col = 0;
            row = 0;
            fprintf (stderr, "HM_TOK [%d, %d]\n",row,col);
            break;

          case NU_TOK:
            row -= number1;
            fprintf (stderr, "NU_TOK[%d] [%d, %d]\n",number1,row,col);
            break;

          case NR_TOK:
            col += number1;
            fprintf (stderr, "NR_TOK[%d] [%d, %d]\n",number1,row,col);
            break;

          case NL_TOK:
            col -= number1;
            fprintf (stderr, "NL_TOK[%d] [%d, %d]\n", number1,row,col);
            break;

          case EOF:
            return;
            break;

          default:
            if (ch < ' ')
              {
                fprintf (stderr, "Unknown character '\\%o'--more--", ch);
              }
            else if (row)
              {
              }
            else if (col == 0)
              {
                screen00 = screen[0][0];
              }
            else if (col == 1 && ch == 'l' && screen[0][0] == 'I')
              {
                screen[0][0] = screen00;
                screen[0][0] = 'I';
              }
            screen[row][col++] = ch;
            fprintf (stderr, "OTHER[%c] [%d,%d]\n",ch,row,col);
            break;
        }
    }
}



int main (int argc, char *argv[])
{
  const char *file = NULL;
  char ch;
  int at_row = 0;
  int at_col = 0;

  if (argc > 1)
    {
      file = argv[1];
    }
  else
    {
      fprintf (stderr, "Usage: deciphervt100 <file>\n");
      exit (1);
    }
  fprintf (stderr, "read file %s\n",file);
  for (row=0; row < 24; ++row)
    {
      for (col = 0; col < 80; ++col)
        {
          screen[row][col] = ' ';
        }
    }

  open_frogue (file);

/*  debuglog_open ("deciphervt100.debuglog"); */
  while ((ch = getroguetoken ()) != EOF)
  {
    int i;
    int j;
    int printit = 0;

    switch (ch)
      {
        case BS_TOK:
          col--;
          printf ("BS_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;

        case CB_TOK:
          for (i =0; i < col; i++)
            {
              screen[row][i] = ' ';
            }
          printf ("CB_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
          break;

        case CE_TOK:
          if (row && row < 23)
            for (i = col; i < 80; i++)
              {
                screen[row][i] = ' ';
              }
          else
            for (i = col; i < 80; i++)
              screen[row][i] = ' ';

          if (col == 0)
            screen00 = ' ';
          printf ("CE_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;

        case CH_TOK:
          printf ("CH_TOK [%d, %d] [%d, %d]\n",number1, number2, row, col);
          s_row1 = number1-1;
          s_row2 = number2-1;
          break;

        case CL_TOK:
          for (row = 0; row < 24; ++row)
            for (col = 0; col < 80; ++col)
              screen[row][col] = ' ';
          row = 0;
          col = 0;
          printf ("CL_TOK [%d, %d]\n", row, col);
//          printit = 1;
          break;

        case CM_TOK:
          row = number1-1;
          col = number2-1;
          printf ("@ is at [%d, %d] [%c] according to me\n", at_row, at_col, screen[at_row][at_col]);
          printf ("@ is at [%d, %d] [%c] according to rogo\n", atrow, atcol, screen[atrow][atcol]);
          printf ("@ is at [%d, %d] [%c] according to rogo0\n", atrow0, atcol0,screen[atrow0][atcol0]);
          printf ("CM_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
          if (screen[row][col] == '@')
            {
              printit = 1;
            }
          break;

        case CR_TOK:
          col = 0;
          printf ("CR_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;

        case ER_TOK: 
          break;

        case LF_TOK:
          row++;
          if (row > s_row2)
            {
              scrollup ();
            }
          printf ("LF_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;

        case ND_TOK:
          row += number1;
          if (row > s_row2)
            {
              printf ("ND_TOK      scroll up\n");
              scrollup ();
            }
          printf ("ND_TOK [%2d] [%2d, %2d] [%c]\n", number1, row, col, screen[row][col]);
//          printit = 1;
          break;

        case SE_TOK: 
          printf ("SE_TOK\n");
          break;

        case SO_TOK: 
          printf ("SO_TOK\n");
          break;

        case TA_TOK:
          col = 8 * (1 + col / 8);
          printf ("TA_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;


        case UP_TOK:
          row--;
          if (row < s_row1)
            {
              printf ("UP_TOK      scroll down\n");
            }
          printf ("UP_TOK      [%2d, %2d] [%c]\n", row, col,screen[row][col]);
//          printit = 1;
          break;

        case HM_TOK:
          col = 0;
          row = 0;
          printf ("HM_TOK      [%2d, %2d] [%c]\n", row, col, screen[row][col]);
//          printit = 1;
          break;

        case NU_TOK:
          row -= number1;
          if (row < s_row1)
            {
              printf ("NU_TOK      scroll down\n");
            }
          printf ("NU_TOK [%2d] [%2d, %2d] [%c]\n", number1, row, col, screen[row][col]);
//          printit = 1;
          break;

        case NR_TOK:
          col += number1;
          printf ("NR_TOK [%2d] [%2d, %2d] [%c]\n", number1, row, col, screen[row][col]);
//          printit = 1;
          break;

        case NL_TOK:
          col -= number1;
          printf ("NL_TOK [%2d] [%2d, %2d] [%c]\n", number1, row, col, screen[row][col]);
//          printit = 1;
          break;

        case SC_TOK:
          printf ("SC_TOK      [%2d, %2d]\n", row, col);
          break;

        case RC_TOK:
          printf ("RC_TOK      [%2d, %2d]\n", row, col);
          break;

        case SR_TOK:
          printf ("SR_TOK      [%2d, %2d]\n", row, col);
          scrolldown ();
          break;

        case EOF:
          return;
          break;

        default:
          if (ch < ' ')
            {
              fprintf (stderr, "Unknown character '%c'--more--", ch);
            }
          else if (row)
            {
            }
          else if (col == 0)
            {
              screen00 = screen[0][0];
            }
          else if (col == 1 && ch == 'l' && screen[0][0] == 'I')
            {
              screen[0][0] = screen00;
              screen[0][0] = 'I';
            }
            if (ch == '@')
              {
                at_row = row;
                at_col = col;
              }
          screen[row][col++] = ch;
          printf ("OTHER   [%c] [%2d, %2d] [%c]\n", ch, row, (col-1), screen[row][col-1]);
          break;
      }

    if (printit)
      {
        printscreen ();
      }
  }
/*  debuglog_close (); */

  close_frogue ();
  exit (0);
}
