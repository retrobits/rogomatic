#ifndef __GETROGUETOKEN_H__
#define __GETROGUETOKEN_H__

extern int number1;
extern int number2;

void open_frogue (const char *file);
void open_frogue_fd (int frogue_fd);
void close_frogue ();
char getroguetoken (void);

#endif /* __GETROGUETOKEN_H__ */
