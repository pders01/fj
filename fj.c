#include "uthash.h"
#include <dirent.h>
#include <glob.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wordexp.h>

typedef struct {
  struct alias_entry *entries;
} alias_dictionary;

struct alias_entry {
  char *alias;
  char *path;
  UT_hash_handle hh;
};

void read_alias_file(const char *filename, alias_dictionary *dict) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return;
  }

  char line[256];
  while (fgets(line, sizeof(line), fp) != NULL) {
    if (strncmp(line, "alias ", 6) == 0) {
      char *alias = strtok(line + 6, "=");
      char *path = strtok(NULL, "\n");
      if (alias && path) {
        struct alias_entry *entry = malloc(sizeof(struct alias_entry));
        if (!entry) {
          fprintf(stderr, "Memory allocation error\n");
          free(entry);
          continue;
        }

        entry->alias = strdup(alias);
        if (!entry->alias) {
          fprintf(stderr, "Memory allocation error\n");
          free(entry);
          continue;
        }

        entry->path = strdup(path);
        if (!entry->path) {
          fprintf(stderr, "Memory allocation error\n");
          free(entry->alias);
          free(entry);
          continue;
        }

        HASH_ADD_STR(dict->entries, alias, entry);
      }
    }
  }

  fclose(fp);
}

int process_aliasrc_directory(const char *dir_path, alias_dictionary *dict) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    fprintf(stderr, "Error opening directory: %s\n", dir_path);
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcasecmp(entry->d_name, ".") == 0 ||
        strcasecmp(entry->d_name, "..") == 0) {
      continue;
    }

    if (strcasestr(entry->d_name, "path") != NULL) {
      char full_path[512];
      snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
      read_alias_file(full_path, dict);
    }
  }

  closedir(dir);
  return 0;
}

bool validate_arguments(int argc, char *argv[], const char **filename) {
  if (argc == 2) {
    *filename = NULL;
    return true;
  }

  if (argc == 3 && strcmp(argv[1], "-complete") == 0) {
    *filename = NULL;
    return true;
  }

  if (argc == 4 && strcmp(argv[1], "-file") == 0) {
    *filename = argv[2];
    return true;
  }

  fprintf(
      stderr,
      "Usage: fj <ALIAS> OR fj -complete <ALIAS> OR fj -file <PATH> <ALIAS>\n");
  return false;
}

char *expand_alias(const char *alias, alias_dictionary *dict) {
  struct alias_entry *found_entry;

  HASH_FIND_STR(dict->entries, alias, found_entry);
  if (!found_entry) {
    fprintf(stderr, "Alias not found\n");
    return NULL;
  }

  wordexp_t result;
  if (wordexp(found_entry->path, &result, 0) != 0) {
    fprintf(stderr, "Error during expansion\n");
    return NULL;
  }

  char *expanded_path = strdup(result.we_wordv[0]);
  if (!expanded_path) {
    fprintf(stderr, "Memory allocation error\n");
    wordfree(&result);
    return NULL;
  }

  wordfree(&result);
  return expanded_path;
}

const char *get_partial_path(int argc, char *argv[]) {
  return (argc == 2) ? argv[1] : argv[2];
}

void generate_completions(const char *expanded_path) {
  glob_t globbuf;
  char *pattern = malloc(strlen(expanded_path) + 3);
  if (!pattern) {
    return;
  }

  sprintf(pattern, "%s/*", expanded_path);

  int glob_result = glob(pattern, 0, NULL, &globbuf);
  if (glob_result != 0) {
    return;
  }

  for (size_t i = 0; i < globbuf.gl_pathc; ++i) {
    struct stat s;
    if (stat(globbuf.gl_pathv[i], &s) != 0) {
      continue;
    }

    if (S_ISDIR(s.st_mode)) {
      printf("%s/\n", globbuf.gl_pathv[i]);
    }
  }

  globfree(&globbuf);
  free(pattern);
}

int main(int argc, char *argv[]) {
  alias_dictionary alias_dict = {.entries = NULL};
  const char *filename = NULL;

  if (!validate_arguments(argc, argv, &filename)) {
    return 1;
  }

  if (filename != NULL) {
    if (access(filename, F_OK) != 0) {
      fprintf(stderr, "Error: File not found: %s\n", filename);
      return 1;
    }
    read_alias_file(filename, &alias_dict);
  } else {
    const char *home_dir = getenv("HOME");
    if (!home_dir) {
      fprintf(stderr, "Error: Cannot get HOME directory\n");
      return 1;
    }

    char aliasrc_path[512];
    snprintf(aliasrc_path, sizeof(aliasrc_path), "%s/.aliasrc", home_dir);
    read_alias_file(aliasrc_path, &alias_dict);

    char aliasrc_dir[512];
    snprintf(aliasrc_dir, sizeof(aliasrc_dir), "%s/.aliasrc.d", home_dir);
    if (process_aliasrc_directory(aliasrc_dir, &alias_dict) != 0) {
      fprintf(stderr, "Error: Error while processing .aliasrc.d\n");
      return 1;
    }
  }

  const char *partial_path = get_partial_path(argc, argv);
  char *expanded_path = expand_alias(partial_path, &alias_dict);
  if (!expanded_path) {
    return 1;
  }

  if (argc == 3 && strcmp(argv[1], "-complete") == 0) {
    generate_completions(expanded_path);
  } else {
    printf("%s\n", expanded_path);
  }

  free(expanded_path);
  return 0;
}
