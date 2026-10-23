/* Safe standalone clock diagnostic for Ndless. No hook writes. */
#include <os.h>
#include <stdio.h>
#include <time.h>
int main(void) {
  time_t now=time(NULL);
  struct tm *tmv=localtime(&now);
  printf("NspireClock CX II - test v0.1\n");
  if(tmv) printf("Time: %02d:%02d:%02d\nDate: %04d-%02d-%02d\n",tmv->tm_hour,tmv->tm_min,tmv->tm_sec,tmv->tm_year+1900,tmv->tm_mon+1,tmv->tm_mday);
  else puts("Clock unavailable");
  puts("No root writes. No hooks installed.");
  puts("Press ENTER to exit.");
  while (!isKeyPressed(KEY_NSPIRE_ENTER)) {}
  return 0;
}
