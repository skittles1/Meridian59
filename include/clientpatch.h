// Meridian 59, Copyright 1994-2012 Andrew Kirmse and Chris Kirmse.
// All rights reserved.
//
// This software is distributed under a license that is described in
// the LICENSE file that accompanies it.
//
// Meridian is a registered trademark.
/*
 * clientpatch.h:  Header file containing client patching routines.
 */

#ifndef _CLIENTPATCH_H
#define _CLIENTPATCH_H

#include <jansson.h>
#include <md5.h>

#define MAX_FILE_READ 1536
#define HASHABLE_BYTES 16
#define HEX_HASH 33

#define CLIENTPATCH_FILE_VERSION 2

void ClientPatchGetValidBasepath(json_t **CacheFile, char *retpath);
void ClientPatchGetAbsPath(json_t **CacheFile, char *retpath);
bool CompareCacheToLocalFile(json_t **CacheFile);
static void GenerateCache();
json_t * GenerateCacheFile(const char *fullpath, const char *basepath, const char *file);
void GenerateCacheMD5(const char *fullpath, const char *file, json_t **CacheFile);

/*
 * ClientPatchGetValidBasepath: Get the base path including the current
 *   directory '.', put the result into retpath.
 */
void ClientPatchGetValidBasepath(json_t **CacheFile, char *retpath)
{
   const char *path = json_string_value(json_object_get(*CacheFile, "Basepath"));
   strcpy(retpath, ".");
   strcat(retpath, path);
}

/*
 * ClientPatchGetValidFilepath: Get the path (base and file) including
 *   the current directory '.', put the result into retpath.
 */
void ClientPatchGetValidFilepath(json_t **CacheFile, char *retpath)
{
   const char *file = json_string_value(json_object_get(*CacheFile, "Filename"));
   const char *path = json_string_value(json_object_get(*CacheFile, "Basepath"));

   strcpy(retpath, ".");
   strcat(retpath, path);
   strcat(retpath, file);
}

/*
 * CompareCacheToLocalFile: Takes a cachefile, compares it to a local one.
 *                          Returns true if they are the same, false otherwise.
 */
bool CompareCacheToLocalFile(json_t **CacheFile)
{
   FILE *fp;
   char buffer[MAX_FILE_READ];
   unsigned char hash[HASHABLE_BYTES];
   char myhash[HEX_HASH];
   unsigned long size, filename_len;
   const char *file = json_string_value(json_object_get(*CacheFile, "Filename"));
   const char *path = json_string_value(json_object_get(*CacheFile, "Basepath"));

   char combine[MAX_PATH + FILENAME_MAX];
   strcpy(combine, ".");
   strcat(combine, path);
   strcat(combine, file);

   // Lookup local file
   if (!(fp = fopen(combine, "rb")))
      return false;

   fseek(fp, 0, SEEK_END);
   size = ftell(fp);
   if (size != json_integer_value(json_object_get(*CacheFile, "Length")))
   {
      fclose(fp);
      return false;
   }

   fseek(fp, 0, SEEK_SET);
   if (size == 0)
   {
      fclose(fp);
      return false;
   }
   else if (size > 1024)
      size = 1024; // Only hash the first 1024 bytes.

   fread(buffer, size, 1, fp);
   fclose(fp);
   filename_len = strlen(file);

   for (unsigned long i = size; i < size + filename_len; ++i)
      buffer[i] = file[i - size];
   MDStringBytes(buffer, hash, size + filename_len);
   for (int i = 0; i < 16; ++i)
      sprintf(myhash + i * 2, "%02X", hash[i]);

   myhash[32] = 0;
   if (strcmp(myhash, json_string_value(json_object_get(*CacheFile, "MyHash"))) != 0)
      return false;
   return true;
}

/*
 * GenerateCacheFile: Takes a full path to a file, a relative path (from client root)
 *   and a filename, generates a json_t object containing Basepath, Filename, Version,
 *   Length, MyHash and whether to download (zip false, all else true). Returns the
 *   object.
 */
json_t * GenerateCacheFile(const char *fullpath, const char *basepath, const char *file)
{
   json_t *CacheFile;
   CacheFile = json_object();

   json_object_set(CacheFile, "Basepath", json_string(basepath));
   json_object_set(CacheFile, "Filename", json_string(file));
   json_object_set(CacheFile, "Version", json_pack("i", CLIENTPATCH_FILE_VERSION));

   if (strcmp(strrchr(file, '.'), ".zip") == 0)
      json_object_set(CacheFile, "Download", json_pack("b", 0));
   else
      json_object_set(CacheFile, "Download", json_pack("b", 1));

   GenerateCacheMD5(fullpath, file, &CacheFile);

   return CacheFile;
}

/*
 * GenerateCacheMD5: Takes a json_t file containing Filename and Basepath.
 *                   Adds the MD5 hash and file length.
 */
void GenerateCacheMD5(const char *fullpath, const char *file, json_t **CacheFile)
{
   FILE *fp;
   char buffer[MAX_FILE_READ];
   unsigned char hash[HASHABLE_BYTES];
   char myhash[HEX_HASH];
   unsigned long size, filename_len;
   char combine[MAX_PATH + FILENAME_MAX];

   strcpy(combine, fullpath);
   strcat(combine, file);

   if (!(fp = fopen(combine, "rb")))
      return;

   fseek(fp, 0, SEEK_END);
   size = ftell(fp);
   json_object_set(*CacheFile, "Length", json_pack("i", size));
   fseek(fp, 0, SEEK_SET);

   if (size == 0)
   {
      fclose(fp);
      return;
   }
   else if (size > 1024)
      size = 1024;

   fread(buffer, size, 1, fp);

   fclose(fp);

   filename_len = strlen(file);

   for (unsigned long i = size; i < size + filename_len; ++i)
      buffer[i] = file[i - size];
   MDStringBytes(buffer, hash, size + filename_len);
   for (int i = 0; i < 16; ++i)
      sprintf(myhash + i * 2, "%02X", hash[i]);

   myhash[32] = 0;
   json_object_set(*CacheFile, "MyHash", json_string(myhash));
}

#endif /* #ifndef _CLIENTPATCH_H */
