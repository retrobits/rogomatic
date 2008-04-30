/*
 * Rog-O-Matic
 * Automatically exploring the dungeons of doom.
 *
 * Copyright (C) 2008 by Anthony Molinaro
 * Copyright (C) 1985 by Appel, Jacobson, Hamey, and Mauldin.
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

/*
 * pack.c:
 *
 * This file contains functions which mess with Rog-O-Matics pack
 */

# include <curses.h>
# include <string.h>
# include "types.h"
# include "globals.h"

static char *stuffmess [] = {
    "strange object", "food", "potion", "scroll",
    "wand", "ring", "hitter", "thrower",
    "missile", "armor", "amulet", "gold",
    "none" };

/*
 * itemstr: print the inventory message for a single item.
 */

char *itemstr (i)
register int i;
{ static char ispace[128];
  register char *item = ispace;

  if (i < 0 || i >= MAXINV)
  { sprintf (item, "%d out of bounds", i); }
  else if (inven[i].count < 1)
  { sprintf (item, "%c)      nothing", LETTER(i)); }
  else
  { sprintf (item, "%c) %4d %d*%s:", LETTER(i), worth(i),
             inven[i].count, stuffmess[(int)inven[i].type]);

    if (inven[i].phit != UNKNOWN && inven[i].pdam == UNKNOWN)
      sprintf (item, "%s (%d)", item, inven[i].phit);
    else if (inven[i].phit != UNKNOWN)
      sprintf (item, "%s (%d,%d)", item, inven[i].phit, inven[i].pdam);

    if (inven[i].charges != UNKNOWN)
      sprintf (item, "%s [%d]", item, inven[i].charges);

    sprintf (item, "%s %s%s%s%s%s%s%s%s%s.",	  /* DR UTexas */
            item, inven[i].str, 
             (itemis (i, KNOWN) ? "" : ", unknown"),
             (used (inven[i].str) ? ", tried" : ""),
             (itemis (i, CURSED) ? ", cursed" : ""),
             (itemis (i, UNCURSED) ? ", uncursed" : ""),
             (itemis (i, ENCHANTED) ? ", enchanted" : ""),
             (itemis (i, PROTECTED) ? ", protected" : ""),
             (itemis (i, WORTHLESS) ? ", useless" : ""),
             (!itemis (i, INUSE) ? "" :
              (inven[i].type == armor || inven[i].type == ring) ?
              ", being worn" : ", in hand"));
  }

  return (item);
}

/*
 * dumpinv: print the inventory. calls itemstr.
 */

dumpinv (f)
register FILE *f;
{ register int i; 

  if (f == NULL)
    at (1,0);

  for (i=0; i<MAXINV; i++)
  { if (inven[i].count == 0)			/* No item here */
      ;
    else if (f != NULL)				/* Write to a file */
    { fprintf (f, "%s\n", itemstr (i)); }
    else					/* Dump on the screen */
    {
      debuglog ("pack : dumpinv %d/%d : %s\n", i, MAXINV, itemstr (i));
//      printw ("%s\n", itemstr (i));
    }
  }
}

/*
 * removeinv: remove an item from the inventory.
 */

removeinv (pos)
int pos;
{ 
  debuglog ("pack : removeinv (%d) %d : START\n",pos, invcount);
  dumpinv (NULL);
  if (--(inven[pos].count) == 0)
  { clearpack  (pos);		/* Assure nothing at that spot  DR UT */
    rollpackup (pos);		/* Close up the hole */
  }

  countpack ();
  checkrange = 1;
  debuglog ("pack : removeinv (%d) %d : END\n",pos, invcount);
  dumpinv (NULL);
}

/*
 * deleteinv: delete an item from the inventory. Note: this function
 * is used when we drop rather than throw or use, since bunches of
 * things can be dropped all at once.
 */

deleteinv (pos)
int pos;
{ 
  debuglog ("pack : deleteinv (%d) %d : START\n",pos,invcount);
  dumpinv (NULL);

  if (--(inven[pos].count) == 0 || inven[pos].type == missile)
  { clearpack  (pos);		/* Assure nothing at that spot  DR UT */
    rollpackup (pos);		/* Close up the hole */
  }

  countpack ();
  checkrange = 1;
  debuglog ("pack : deleteinv (%d) %d : END\n",pos,invcount);
  dumpinv (NULL);
}

/*
 * clearpack: zero out slot in pack.  DR UTexas 01/05/84
 */

clearpack (pos)
int pos;
{
  debuglog ("pack : clearpack (%d) %d : START\n",pos,invcount);
  dumpinv (NULL);

  if (pos >= MAXINV) return;
  inven[pos].count = 0;
  inven[pos].str[0] = '\0';
  inven[pos].phit = UNKNOWN;
  inven[pos].pdam = UNKNOWN;
  inven[pos].charges = UNKNOWN;
  forget (pos, (KNOWN | CURSED | ENCHANTED | PROTECTED | UNCURSED |
                INUSE | WORTHLESS));
  debuglog ("pack : clearpack (%d) %d : END\n",pos,invcount);
  dumpinv (NULL);
  
}

/* 
 * rollpackup: We have deleted an item, move up the objects behind it in
 * the pack.
 */

rollpackup (pos)
register int pos;
{ register char *savebuf;
  register int i;

  debuglog ("pack : rollpackup (%d) %d : START\n",pos,invcount);
  dumpinv (NULL);
  if (version >= RV53A) return;

  if (pos < currentarmor) currentarmor--;
  else if (pos == currentarmor) currentarmor = NONE;
       
  if (pos < currentweapon) currentweapon--;
  else if (pos == currentweapon) currentweapon = NONE;
       
  if (pos < leftring) leftring--;
  else if (pos == leftring) leftring = NONE;
       
  if (pos < rightring) rightring--;
  else if (pos == rightring) rightring = NONE;

  savebuf = inven[pos].str;
  for (i=pos; i+1<invcount; i++)
    inven[i] = inven[i+1];

  inven[--invcount].str = savebuf;

  inven[invcount].count = 0; /* mark this slot as empty - NYM */
  debuglog ("pack : rollpackup (%d) %d : END\n",pos,invcount);
  dumpinv (NULL);
}

/* 
 * rollpackdown: Open up a new spot in the pack, and move down the
 * objects behind that position.
 */

rollpackdown (pos)
register int pos;
{ register char *savebuf;
  register int i;

  debuglog ("pack : rollpackdown (%d) %d : START\n",pos, invcount);
  dumpinv (NULL);
  if (version >= RV53A)
    {
      return;
    }

//  debuglog ("rollpackdown : invcount = %d MAXINV = %d\n",invcount, MAXINV);
//  debuglog ("rollpackdown : savebuf = inven[%d].str = '%s'\n", invcount, inven[invcount].str);
  savebuf = inven[invcount].str;
//  debuglog ("rollpackdown : loop\n");
  for (i=invcount; i>pos; --i)
  { inven[i] = inven[i-1];
    if (i-1 == currentarmor)   currentarmor++;
    if (i-1 == currentweapon)  currentweapon++;
    if (i-1 == leftring)       leftring++;
    if (i-1 == rightring)      rightring++;
  }
//  debuglog ("rollpackdown : inven[%d].str = savebuf = '%s'\n", pos, savebuf);
  inven[pos].str = savebuf;
//  debuglog ("rollpackdown : inven[%d].str = '%s'\n", pos, inven[pos].str);

  if (++invcount > MAXINV)
    {
      debuglog ("pack : rollpackdown : usesynch = 0\n");
      usesynch = 0; 
    }

  debuglog ("pack : rollpackdown (%d) %d : END\n",pos, invcount);
  dumpinv (NULL);
}

/*
 * resetinv: send an inventory command. The actual work is done by
 * doresetinv, which is called by a demon in the command handler.
 */

resetinv()
{ 
  debuglog ("pack : resetinv\n");
  if (!replaying)
    {
      command (T_OTHER, "i");
    }
  else
    {
      /* if we are replaying, then the original game would have caused
       * doresetinv to be called via the command above, so just call it 
       * directly.  If this isn't called the replay core dumps with a
       * segfault because the inventory structure is incorrect - NYM
       */
      doresetinv ();
    }
}

/*
 * doresetinv: reset the inventory.  DR UTexas 01/05/84
 */

doresetinv ()
{ int i;
  static char space[MAXINV][80]; 

  debuglog ("pack : doresetinv : usesynch = 1\n");
  usesynch = 1;
  checkrange = 0;

  for(i=0; i<MAXINV; ++i) 
  { inven[i].str = space[i];
    clearpack (i);
  }

  invcount = objcount = urocnt = 0;
  currentarmor = currentweapon = leftring = rightring = NONE;
  
  if (version >= RV53A) invcount = MAXINV;
}

/*
 * inventory: parse an item message.
 */

# define xtr(w,b,e,k) {what=(w);xbeg=mess+(b);xend=mend-(e);xknow|=(k);}

inventory (msgstart, msgend)
char *msgstart, *msgend;
{ register char *p, *q, *mess = msgstart, *mend = msgend;
  char objname[100], *realname();
  int  n, ipos, xknow = 0, newitem = 0, inuse = 0, printed = 0;
  int  plushit = UNKNOWN, plusdam = UNKNOWN, charges = UNKNOWN;
  stuff what; 
  char *xbeg, *xend;

  debuglog ("pack : inventory ('%s', '%s')\n",mess, mend);

  xbeg = xend = "";
  dwait (D_PACK, "inventory: message %s", mess);

  /* Rip surrounding garbage from the message */

  debuglog ("inventory : 1\n");
  if (mess[1] == ')')
  { ipos= DIGIT(*mess); mess+=3;}
  else
  { ipos= DIGIT(mend[-2]); mend -= 4;
    deletestuff (atrow, atcol);
    unsetrc (USELESS, atrow, atcol);
    newitem = 1; }
         
  debuglog ("inventory : 2 : %d/%d : %s\n",ipos, invcount, itemstr(ipos));
  if (ISDIGIT(*mess))
  { n=atoi(mess); mess += 2+(n>9); }
  else 
  { n=1;
    if (*mess == 'a') mess++;   /* Eat the determiner A/An/The */
    if (*mess == 'n') mess++;
    if (*mess == 't') mess++;
    if (*mess == 'h') mess++;
    if (*mess == 'e') mess++;
    if (*mess == ' ') mess++; } /* Eat the space after the determiner */

  debuglog ("inventory : 3 : %d : %s\n",ipos, itemstr(ipos));
  /* Read the plus to hit */
  if (*mess=='+' || *mess=='-')
  { plushit = atoi(mess++); 
    while (ISDIGIT (*mess)) mess++;
    xknow = KNOWN;}

  debuglog ("inventory : 4 : %d : %s\n",ipos, itemstr(ipos));
  /* Eat any comma separating two modifiers */
  if (*mess==',') mess++;

  debuglog ("inventory : 5 : %d : %s\n",ipos, itemstr(ipos));
  /* Read the plus damage */
  if (*mess=='+' || *mess=='-')
  { plusdam = atoi(mess++); 
    while (ISDIGIT (*mess)) mess++;
    xknow = KNOWN;}

  debuglog ("inventory : 6 : %d : %s\n",ipos, itemstr(ipos));
  while (*mess==' ') mess++;		/* Eat any separating spaces */
  debuglog ("inventory : 7\n");
  while (mend[-1]==' ') mend--;		/* Remove trailing blanks */
  debuglog ("inventory : 8\n");
  while (mend[-1]=='.') mend--;		/* Remove trailing periods */

  debuglog ("inventory : 9 : %d : %s\n",ipos, itemstr(ipos));
  /* Read any parenthesized strings at the end of the message */
  while (mend[-1]==')')
  { while (*--mend != '(') ;		/* on exit mend -> '(' */
    if (stlmatch(mend,"(being worn)") )
    { currentarmor = ipos; inuse = INUSE; }
    else if (stlmatch(mend,"(weapon in hand)") )
    { currentweapon = ipos; inuse = INUSE; }
    else if (stlmatch(mend,"(on left hand)") )
    { leftring = ipos; inuse = INUSE; }
    else if (stlmatch(mend,"(on right hand)") )
    { rightring = ipos; inuse = INUSE; }

    while (mend[-1]==' ') mend--;
  }

  debuglog ("inventory : 10 : %d : %s\n",ipos, itemstr(ipos));
  /* Read the charges on a wand (or armor class or ring bonus) */
  if (mend[-1] == ']')
  { while (*--mend != '[');		/* on exit mend -> '[' */
    if (mend[1] == '+')	charges = atoi(mend+2);
    else		charges = atoi(mend+1);
    xknow = KNOWN;
  }

  debuglog ("inventory : 11 : %d : %s\n",ipos, itemstr(ipos));
  /* Undo plurals by removing trailing 's' */
  while (mend[-1] == ' ') mend--;
  debuglog ("inventory : 12\n");
  if (mend[-1]=='s') mend--;

  debuglog ("inventory : 13 : %d : %s\n",ipos, itemstr(ipos));
  /* Now find what we have picked up: */
  if (stlmatch(mend-4,"food")) {what=food;xknow=KNOWN;}
  else if (stlmatch(mess,"amulet")) xtr(amulet,0,0,KNOWN)
  else if (stlmatch(mess,"potion of ")) xtr(potion,10,0,KNOWN)
  else if (stlmatch(mess,"potions of ")) xtr(potion,11,0,KNOWN)
  else if (stlmatch(mess,"scroll of ")) xtr(Scroll,10,0,KNOWN)
  else if (stlmatch(mess,"scrolls of ")) xtr(Scroll,11,0,KNOWN)
  else if (stlmatch(mess,"staff of ")) xtr(wand,9,0,KNOWN)
  else if (stlmatch(mess,"wand of ")) xtr(wand,8,0,KNOWN)
  else if (stlmatch(mess,"ring of "))  xtr(ring,8,0,KNOWN)
  else if (stlmatch(mend-4,"mail")) xtr(armor,0,0,0)
  else if (stlmatch(mend-6,"potion")) xtr(potion,0,7,0)
  else if (stlmatch(mess,"scroll titled '")) xtr(Scroll,15,1,0)
  else if (stlmatch(mess,"scrolls titled '")) xtr(Scroll,16,1,0)
  else if (stlmatch(mend-5,"staff")) xtr(wand,0,6,0)
  else if (stlmatch(mend-4,"wand"))  xtr(wand,0,5,0)
  else if (stlmatch(mend-4,"ring")) xtr(ring,0,5,0)
  else if (stlmatch(mess,"apricot")) xtr(food,0,0,KNOWN)
  else if (stlmatch(mend-5,"sword")) xtr(hitter,0,0,0)
  else if (stlmatch(mend-4,"mace")) xtr(hitter,0,0,0)
  else if (stlmatch(mend-6,"dagger")) xtr(missile,0,0,0)
  else if (stlmatch(mend-5,"spear")) xtr(missile,0,0,0)
  else if (stlmatch(mend-5,"armor")) xtr(armor,0,0,0)
  else if (stlmatch(mend-3,"arm")) xtr(armor,0,0,0)
  else if (stlmatch(mend-3,"bow")) xtr(thrower,0,0,0)
  else if (stlmatch(mend-5,"sling")) xtr(thrower,0,0,0)
  else if (stlmatch(mend-5,"arrow")) xtr(missile,0,0,0)
  else if (stlmatch(mend-4,"dart")) xtr(missile,0,0,0)
  else if (stlmatch(mend-4,"rock")) xtr(missile,0,0,0)
  else if (stlmatch(mend-4,"bolt")) xtr(missile,0,0,0)
  else if (stlmatch(mend-8,"shuriken")) xtr(missile,0,0,0)
  else xtr(strange,0,0,0)

  /* Copy the name of the object into a string */

  debuglog ("inventory : 14 : %d : %s\n",ipos, itemstr(ipos));
  for (p = objname, q = xbeg; q < xend;  p++, q++) *p = *q;
  *p = '\0';

  debuglog ("inventory : 15 : %d : %s\n",ipos, itemstr(ipos));
  dwait (D_PACK, "inv: %s '%s', hit %d, dam %d, chg %d, knw %d",
         stuffmess[(int) what], objname, plushit, plusdam, charges, xknow);

  debuglog ("inventory : 16 : %d : %s\n",ipos, itemstr(ipos));
  /* Ring bonus is printed differently in Rogue 5.3 */
  if (version >= RV53A && what == ring && charges != UNKNOWN)
  { plushit = charges; charges = UNKNOWN; }

  /* If the name of the object matches something in the database, */
  /* slap the real name into the slot and mark it as known */
  debuglog ("inventory : 17 : %d : %s\n",ipos, itemstr(ipos));

  if ((what == potion || what == Scroll || what == wand) && !xknow)
  { char *dbname = realname (objname);
    if (*dbname)
    { strcpy (objname, dbname);
      xknow = KNOWN;
      if (newitem)
      { at (0,0);

        if (n == 1) printw ("a ");
        else printw ("%d ", n);

        printw ("%s%s of %s (%c)",
                what == potion ?    "potion" :
                  what == Scroll ?  "scroll" :
                  what == ring ?    "ring" :
                                    "wand",
                (n == 1) ? "" : "s",
                objname,
                LETTER(ipos));

        clrtoeol ();
        at (row, col);
        refresh ();
	printed++;
      }
    }
  }

  debuglog ("inventory : 18 : %d : %s\n",ipos, itemstr(ipos));
  /* If new item, record the change */
  if (newitem && what == armor) 
    newarmor = 1;
  else if (newitem && what == ring)
    newring = 1;
  else if (newitem && what == food)
  { newring = 1; lastfoodlevel = Level; }
  else if (newitem && (what == hitter || what == missile || what == wand)) 
    newweapon = 1;

  /* If the object is an old object, set its count, else allocate */
  /* a new object and roll the other objects down */

  debuglog ("inventory : 19 : %d : %s\n",ipos, itemstr(ipos));
  if (n > 1 && ipos < invcount && inven[ipos].type == what &&
      n == inven[ipos].count+1 &&
      stlmatch(objname, inven[ipos].str) && 
      inven[ipos].phit == plushit &&
      inven[ipos].pdam == plusdam)
  {
    debuglog ("inventory : 19.1 '%s'\n",itemstr (ipos));
    inven[ipos].count = n;
  }
  /* New item, in older Rogues, open up a spot in the pack */
  else
  {
    debuglog ("inventory : 19.2\n");
    if (version < RV53A)
      {
        debuglog ("inventory : 19.3\n");
        rollpackdown (ipos);		
      }

    inven[ipos].type = what;
    inven[ipos].count = n;
    inven[ipos].phit = plushit;
    inven[ipos].pdam = plusdam;
    inven[ipos].charges = charges;
    remember (ipos, inuse | xknow);
    if (!xknow) ++urocnt;
  }

  debuglog ("inventory : 20 : %d : %s\n",ipos, itemstr(ipos));
  /* Forget enchanted status if item known.  DR UTexas 31 Jan 84 */
  if (itemis (ipos, KNOWN))
    {
      debuglog ("inventory : 20.1\n");
      forget (ipos, ENCHANTED);
    }

  debuglog ("inventory : 21 : %d : %s\n",ipos, itemstr(ipos));
  /* Set the name of the object */
  if (inven[ipos].str != NULL)
    {
      debuglog ("inventory : 21.1\n");
      if (objname == NULL)
        {
          debuglog ("inventory : objname = 'NULL'\n");
        }
      else
        {
          debuglog ("inventory : objname = '%s'\n",objname);
        }
      if (inven[ipos].str == NULL)
        {
          debuglog ("inventory : inven[ipos].str = 'NULL'\n");
        }
      else
        {
          debuglog ("inventory : inven[ipos].str = 'NOT NULL' %d \n",ipos);
        }
      strcpy (inven[ipos].str, objname);
      debuglog ("inventory : 21.2\n");
    }
  else if (!replaying)
    {
      debuglog ("inventory : 21.3\n");
      dwait (D_ERROR, "terpmess: null inven[%d].str, invcount %d.",
             ipos, invcount);
      debuglog ("inventory : 21.4\n");
    }

  debuglog ("inventory : 22\n");
  /* Set cursed attribute for weapon and armor */
  if (cursedarmor && ipos == currentarmor) remember (ipos, CURSED);
  if (cursedweapon && ipos == currentweapon) remember (ipos, CURSED);

  debuglog ("inventory : 23\n");
  /* Keep track of whether we are wielding a trap arrow */
  if (ipos == currentweapon) usingarrow = (what == missile);

  debuglog ("inventory : 24\n");
  countpack ();

  debuglog ("inventory : 25\n");
  /* If we picked up a useless thing, note that fact */
  if (newitem && on (USELESS))	remember (ipos, WORTHLESS);
  else if (newitem)		forget (ipos, WORTHLESS);

  checkrange = 1;
  
  debuglog ("inventory : 26\n");
  return (printed);
}

/* 
 * countpack: Count objects, missiles, and food in the pack.
 */
 
countpack ()
{ register int i, cnt;

  for (objcount=0, larder=0, ammo=0, i=0; i<invcount; i++)
  { if (! (cnt = inven[i].count))	; /* No object here */
    else if (inven[i].type == missile)	{ objcount++; ammo += cnt; }
    else if (inven[i].type == food)	{ objcount += cnt; larder += cnt; }
    else				{ objcount += cnt; }
  }
}
