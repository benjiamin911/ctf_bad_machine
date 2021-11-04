/* Database implementations file */
#include "Database.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static int callback(void *ptr,int argc,char **argv,char **azColName){
	int i;
	for(i=0;i<argc;i++){
		printf("%s=%s\n",azColName[i],argv[i]?argv[i]:"NULL");
	}
printf("\n");

/*
Registration* dest =(Registration*)ptr; 
strncpy(dest->name,(char *)(argv[1]),MAX_NAME_LEN-1);
strncpy(dest->county,(char *)(argv[2]),MAX_NAME_LEN-1);

(dest->name)[MAX_NAME_LEN-1] = '\0';
(dest->county)[MAX_NAME_LEN-1] = '\0';

dest->zip = atoi(argv[3]);
(dest->dob).day= atoi(argv[4]);
(dest->dob).month= atoi(argv[5]);
(dest->dob).year= atoi(argv[6]);
*/


return 0;
}

const char* combineString(char* a,char *b){
	int length = sizeof(a)+sizeof(b)+1;
	char* concatstring = (char*)malloc(length*sizeof(char));
	memset(concatstring,0,length);
	strcat(concatstring,a);
	strcat(concatstring,b);
	return concatstring;
}

void reportError(sqlite3 *db,char* error){

   	char* zErrMsg = 0;
	const char* data = "";
	//printf("%s \n",error);

   const char *sql ="SELECT * FROM Registration WHERE name=";
//SELECT * FROM Registration;
//1;UPDATE Registration set zip=21214 WHERE id=2;
	int length = sizeof(sql)+sizeof(error)+1;
	char concatstring[200];


	memset(concatstring,0,length);
	strcat(concatstring,sql);
	strcat(concatstring,error);


   sqlite3_exec(db,concatstring,callback,(void*)data, &zErrMsg);

   printf("%s\n",concatstring);
	
}

_id_t storeElection(sqlite3 *db, Date deadline) {
   _id_t id = 0;
   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO Election(deadline_day,deadline_mon,\
                      deadline_year,status) VALUES (?, ?, ?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, deadline.day);
   sqlite3_bind_int(stmt, 2, deadline.month);
   sqlite3_bind_int(stmt, 3, deadline.year);
   sqlite3_bind_int(stmt, 4, INACTIVE);
   sqlite3_step(stmt);
   if (sqlite3_finalize(stmt) == SQLITE_OK) {
      id = (_id_t)sqlite3_last_insert_rowid(db);
   }
   return id;
}

_id_t storeOffice(sqlite3 *db, _id_t election, char *name) {
   _id_t id = 0;
   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO Office(name, election) VALUES (?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_text(stmt, 1, name, (int)strnlen(name, MAX_NAME_LEN),
                     SQLITE_STATIC);
   sqlite3_bind_int(stmt, 2, election);
   sqlite3_step(stmt);
   if (sqlite3_finalize(stmt) == SQLITE_OK) {
      id = (_id_t)sqlite3_last_insert_rowid(db);
   }
   return id;
}

_id_t storeCandidate(sqlite3 *db, _id_t office, char *name) {
   _id_t id = 0;
   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO Candidate(name,votes,office)\
                      VALUES (?, ?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_text(stmt, 1, name, (int)strnlen(name, MAX_NAME_LEN),
                     SQLITE_STATIC);
   sqlite3_bind_int(stmt, 2, 0);
   sqlite3_bind_int(stmt, 3, office);
   sqlite3_step(stmt);
   if (sqlite3_finalize(stmt) == SQLITE_OK) {
      id = (_id_t)sqlite3_last_insert_rowid(db);
   }
   return id;
}

void addZip(sqlite3 *db, _id_t office, int zip) {
   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO AllowedZip(zip,office) VALUES (?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, zip);
   sqlite3_bind_int(stmt, 2, office);
   sqlite3_step(stmt);
   sqlite3_finalize(stmt);
}

bool checkZip(sqlite3 *db, _id_t office, int zip) {
   int count;
   sqlite3_stmt *stmt;
   const char *sql = "SELECT COUNT(*) FROM AllowedZip WHERE\
                      zip=? AND office=?";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, zip);
   sqlite3_bind_int(stmt, 2, office);
   sqlite3_step(stmt);
   count = sqlite3_column_int(stmt, 0);
   sqlite3_finalize(stmt);
   return count > 0;
}

//format string
_id_t storeVoter(sqlite3 *db, char*name, char*county, int zip, Date dob) {
   _id_t id = 0;
//convert special character name back
int sz = sizeof(name);
bool hasOne = 0;

for(int i = 0;i<sz;i++){
	if((name[i]=='%')&&(name[i+1]=='2')
		&&(name[i+2]=='5')){
		name[i+1] = 'x';
		name[i+2] = '%';
		hasOne = 1;
		}
}

//convert special character name back end
//const char* input = "%x%x";

if(hasOne){
	printf(name);
}

   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO Registration(name,county,zip,\
                      dob_day,dob_mon,dob_year) VALUES (?, ?, ?, ?, ?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_text(stmt, 1, name, (int)strnlen(name, MAX_NAME_LEN),
                     SQLITE_STATIC);
   sqlite3_bind_text(stmt, 2, county, (int)strnlen(county, MAX_NAME_LEN),
                     SQLITE_STATIC);
   sqlite3_bind_int(stmt, 3, zip);
   sqlite3_bind_int(stmt, 4, dob.day);
   sqlite3_bind_int(stmt, 5, dob.month);
   sqlite3_bind_int(stmt, 6, dob.year);
   sqlite3_step(stmt);
   if (sqlite3_finalize(stmt) == SQLITE_OK) {
      id = (_id_t)sqlite3_last_insert_rowid(db);
   }
   return id;
}

void storeStatus(sqlite3 *db, _id_t election, Status new_status) {
   sqlite3_stmt *stmt;
   const char *sql = "UPDATE Election SET status=? WHERE id=?";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, (int)new_status);
   sqlite3_bind_int(stmt, 2, election);
   sqlite3_step(stmt);
   sqlite3_finalize(stmt);
   if (new_status == PUBLISHED) {
      const char *totals = "UPDATE Candidate SET votes=(\
                              SELECT COUNT(*) FROM Vote WHERE\
                              Vote.candidate=Candidate.id AND\
                              Vote.office=Candidate.office)";
      sqlite3_prepare_v2(db, totals, -1, &stmt, NULL);
      sqlite3_step(stmt);
      sqlite3_finalize(stmt);
   }
}

void deleteElection(sqlite3 *db, _id_t election) {
   sqlite3_stmt *stmt;
   const char *sql = "DELETE FROM Election WHERE id=?";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, election);
   sqlite3_step(stmt);
   sqlite3_finalize(stmt);
}
// sql injection

void getVoter(sqlite3 *db, _id_t voter_id, Registration* dest) {
	//printf("this is id: %dend",voter_id);
   //sqlite3_stmt *stmt;
   char* zErrMsg = 0;
   //const char* data = "";
//"SELECT name,county,zip,dob_day,dob_mon,dob_year FROM Registration WHERE id=1;"
//
   const char *sql ="SELECT * FROM Registration WHERE id=1;UPDATE Registration set zip=21210  WHERE id=";

	char id[100];
	sprintf(id,"%d",voter_id);


	int length = sizeof(sql)+sizeof(id)+1;


	char* concatstring = (char*)malloc(length*sizeof(char));


	memset(concatstring,0,length);
	strcat(concatstring,sql);
	strcat(concatstring,id);

	//printf("%s\n",concatstring);

   sqlite3_exec(db,concatstring,callback,(void*)dest, &zErrMsg);


   //sqlite3_exec(db,sql,callback,(void*)data, &zErrMsg);

  //sqlite3_prepare_v2(db, concatstring, -1, &stmt, NULL);
   //sqlite3_bind_int(stmt, 1, voter_id);
   //sqlite3_step(stmt);
/*
   strncpy(dest->name, (char *)sqlite3_column_text(stmt, 0), MAX_NAME_LEN-1);
   strncpy(dest->county, (char *)sqlite3_column_text(stmt, 1),MAX_NAME_LEN-1);

   (dest->name)[MAX_NAME_LEN-1] = '\0';
   (dest->county)[MAX_NAME_LEN-1] = '\0';
   dest->zip = sqlite3_column_int(stmt, 2);
   (dest->dob).day = sqlite3_column_int(stmt, 3);
   (dest->dob).month = sqlite3_column_int(stmt, 4);
   (dest->dob).year = sqlite3_column_int(stmt, 5);
   sqlite3_finalize(stmt);
*/
}

void getVoter1(sqlite3 *db, _id_t voter_id, Registration* dest) {
   sqlite3_stmt *stmt;
//"SELECT * from Registration; "
//"UPDATE Registration set zip=21212 WHERE id=1; "
//"SELECT name,county,zip,dob_day,dob_mon,dob_year FROM Registration WHERE id=";
   const char *sql ="SELECT * from Registration; "\
"UPDATE Registration set zip=21212 WHERE id=1;";

/*
	char id[100];
	sprintf(id,"%d",voter_id);

	int length = sizeof(sql)+sizeof(id)+1;
	char* concatstring = (char*)malloc(length*sizeof(char));
	memset(concatstring,0,length);
	strcat(concatstring,sql);
	strcat(concatstring,id);
*/


   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_step(stmt);

   strncpy(dest->name, (char *)sqlite3_column_text(stmt, 0), MAX_NAME_LEN-1);
   strncpy(dest->county, (char *)sqlite3_column_text(stmt, 1),MAX_NAME_LEN-1);
   (dest->name)[MAX_NAME_LEN-1] = '\0';
   (dest->county)[MAX_NAME_LEN-1] = '\0';
   dest->zip = sqlite3_column_int(stmt, 2);
   (dest->dob).day = sqlite3_column_int(stmt, 3);
   (dest->dob).month = sqlite3_column_int(stmt, 4);
   (dest->dob).year = sqlite3_column_int(stmt, 5);
   sqlite3_finalize(stmt);
}

void getElection(sqlite3 *db, _id_t election_id, Election* dest) {
   sqlite3_stmt *stmt;
   const char *sql = "SELECT deadline_day,deadline_mon,deadline_year,status\
                      FROM Election WHERE id=?";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, election_id);
   sqlite3_step(stmt);
   (dest->deadline).day = sqlite3_column_int(stmt, 0);
   (dest->deadline).month = sqlite3_column_int(stmt, 1);
   (dest->deadline).year = sqlite3_column_int(stmt, 2);
   dest->status = (Status)sqlite3_column_int(stmt, 3);
   sqlite3_finalize(stmt);
}

void storeVote(sqlite3 *db, _id_t voter, _id_t candidate, _id_t office) {
   sqlite3_stmt *stmt;
   const char *sql = "INSERT INTO Vote(voter,candidate,office)\
                      VALUES (?, ?, ?)";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, voter);
   sqlite3_bind_int(stmt, 2, candidate);
   sqlite3_bind_int(stmt, 3, office);
   sqlite3_step(stmt);
   sqlite3_finalize(stmt);
}

int getVote(sqlite3 *db, _id_t voter_id, _id_t office_id) {
   int count;
   sqlite3_stmt *stmt;
   const char *sql = "SELECT COUNT(*) FROM Vote WHERE\
                      voter=? AND office=?";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   sqlite3_bind_int(stmt, 1, voter_id);
   sqlite3_bind_int(stmt, 2, office_id);
   sqlite3_step(stmt);
   count = sqlite3_column_int(stmt, 0);
   sqlite3_finalize(stmt);
   return count;
}

void getVoters(sqlite3 *db) {
   sqlite3_stmt *stmt;
   const char *sql = "SELECT name,county,zip,dob_day,dob_mon,dob_year\
                      FROM Registration";
   sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
   printf("[\n");
   bool is_first = false;
   while (sqlite3_step(stmt) != SQLITE_DONE) {
      if (!is_first) {
         is_first = true;
      } else {
         printf(",\n");
      }
      printf("{\"name\": \"%s\", \"county\": \"%s\", \"zip\": \"%d\", ",
             sqlite3_column_text(stmt, 0),
             sqlite3_column_text(stmt, 1),
             sqlite3_column_int(stmt, 2));
      printf("\"dob\": \"%d-%02d-%02d\"}",
             sqlite3_column_int(stmt, 5)+1900,
             sqlite3_column_int(stmt, 4),
             sqlite3_column_int(stmt, 3));
   }
   printf("\n]\n");
   sqlite3_finalize(stmt);
}

void getElections(sqlite3 *db) {
   system("./database_helper.py"); /* U+1F914 */
}
