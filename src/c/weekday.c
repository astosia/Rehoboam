#include <pebble.h>
#include "weekday.h"
#include "string.h"
const char* WEEKDAY_ES[] = {
  "DOM",
  "LUN",
  "MAR",
  "MIE",
  "JUE",
  "VIE",
  "SAB",
};
const char* WEEKDAY_EN[] = {
  "SUN",
  "MON",
  "TUE",
  "WED",
  "THU",
  "FRI",
  "SAT",
};
const char* WEEKDAY_DE[] = {
  "SON",
  "MON",
  "DIE",
  "MIT",
  "DON",
  "FRE",
  "SAM",
};
const char* WEEKDAY_FR[] = {
  "DIM",
  "LUN",
  "MAR",
  "MER",
  "JEU",
  "VEN",
  "SAM",
};
const char* WEEKDAY_PT[] = {
  "DOM",
  "SEG",
  "TER",
  "QUA",
  "QUI",
  "SEX",
  "SAB",
};
const char* WEEKDAY_IT[] = {
  "DOM",
  "LUN",
  "MAR",
  "MER",
  "GIO",
  "VEN",
  "SAB",
};//End_Weekday
void fetchwday(int WD, const char* lang, char *iterweekday ){
  if (strcmp(lang,"es_ES")==0) {strcpy(iterweekday, WEEKDAY_ES[WD]);}
  else if (strcmp(lang,"fr_FR")==0) {strcpy(iterweekday, WEEKDAY_FR[WD]);}
  else if (strcmp(lang,"de_DE")==0) {strcpy(iterweekday, WEEKDAY_DE[WD]);}
  else if (strcmp(lang,"it_IT")==0) {strcpy(iterweekday, WEEKDAY_IT[WD]);}
  else if (strcmp(lang,"pt_PT")==0) {strcpy(iterweekday, WEEKDAY_PT[WD]);}
  else {strcpy(iterweekday, WEEKDAY_EN[WD]);};
}