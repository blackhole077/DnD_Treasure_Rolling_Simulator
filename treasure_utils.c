#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
  NO_CLASS,
  COINS,
  GOODS,
  ITEMS
}TreasureClassification;

//Base structure that all 3 types of treasures use.
typedef struct{
  TreasureClassification t_classification;
  int size_percentages;
  int size_parameters;
  int* percentages;
  int* table;
} Treasure;

//Struct to represent the GOODS section of the treasure table
typedef struct{
  Treasure base_amount_treasure;
  Treasure gems_treasure;
  Treasure art_treasure;
}GoodsTreasure;

void initialize_treasure(Treasure *treasure, TreasureClassification t_classification, int size_percentages, int size_parameters, int* percentages, int* table){
  // printf("Initialize Percent Size.\n");
  treasure->size_percentages = size_percentages;
  // printf("Initialize Size Parameters.\n");
  treasure->size_parameters = size_parameters;
  // printf("Initialize Percentages.\n");
  treasure->percentages = percentages;
  // printf("Initialize Table.\n");
  treasure->table = table;
}

void initialize_empty_base_treasure(Treasure *treasure, TreasureClassification t_classification){
  initialize_treasure(treasure, t_classification, 0, 0, NULL, NULL);
}

void initialize_empty_treasure(Treasure *treasure){
  initialize_treasure(treasure, NO_CLASS, 0, 0, NULL, NULL);
}

void initialize_empty_goods_treasure(GoodsTreasure *treasure, TreasureClassification t_classification){
  Treasure *base_treasure = (Treasure *)malloc(sizeof(Treasure) * 1);
  Treasure *gems_treasure = (Treasure *)malloc(sizeof(Treasure) * 1);
  Treasure *art_treasure = (Treasure *)malloc(sizeof(Treasure) * 1);
  initialize_empty_base_treasure(base_treasure, GOODS);
  initialize_empty_treasure(gems_treasure);
  initialize_empty_treasure(art_treasure);
}

/**
* read_file_to_struct is a function that converts a file into an array of
* Treasure structures of an appropriate classification (designated by t_classification)
* with entries ranging from [0, num_levels-1] (equivalent to levels 1 through num_levels).
* These structures are accessed by later functions to calculate the actual treasure generated
* via simulated dice rolls.
**/
int read_file_to_struct(int num_levels, TreasureClassification t_classification, FILE *file_pointer){
  if(file_pointer == NULL){
    printf("File does not exist. Exiting program.\n");
    return 0;
  }
  //These values will be used as iterators in for loops. Generic naming is to maintain DRY code.
  int level, i, j;
  //Allocate memory for an array of Treasure structures.
  Treasure *treasure = (Treasure *)malloc(sizeof(Treasure) * num_levels);
  for(level = 0; level < num_levels; level++){
      printf("initializing empty treasure...\n");
      initialize_empty_treasure(&treasure[level], t_classification);
      int max_buffer_size = 256;
      char *line_buffer = (char *)malloc(sizeof(char) * max_buffer_size);
      line_buffer[strcspn(line_buffer, "\r\n")] = 0;

      if(line_buffer == NULL){
        printf("Error allocating memory. Exiting program...\n");
        return 0;
      }
      // printf("Fetching Percentage Size...\n");
      //Fetch the size of the percentage array
      if((fgets(line_buffer, sizeof(line_buffer), file_pointer) != NULL)){
        // fputs(line_buffer, stdout);
        // fputs("\n|*\n",stdout);
        treasure[level].size_percentages = (int)strtol(line_buffer, &line_buffer, 10);
        // printf("Value in struct: %d\n", treasure[level].size_percentages);
      }
      else{
        printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
        return 0;
      }
      // printf("Fetching Percentages...\n");
      //Fetch the percentages themselves and store them in the struct
      treasure[level].percentages = (int*) malloc(sizeof(int) * treasure[level].size_percentages);
      if((fgets(line_buffer, max_buffer_size-1, file_pointer) != NULL)){
        char* token;
        // fputs(line_buffer, stdout);
        // fputs("|*\n",stdout);
        token = strtok(line_buffer, ",");
        for(i = 0; i < treasure[level].size_percentages; i++){
          treasure[level].percentages[i] = strtol(token, &token, 10);
          // printf("Value in struct: %d\n", treasure[level].percentages[i]);
          token = strtok(NULL,",");
        }
      }
      else{
        printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
        return 0;
      }
      // printf("Fetching Parameters Size...\n");
      //Fetch the size of the parameters for the struct
      if((fgets(line_buffer, max_buffer_size-1, file_pointer) != NULL)){
        // fputs(line_buffer, stdout);
        // fputs("|*\n",stdout);
        treasure[level].size_parameters = (int) strtol(line_buffer, &line_buffer, 10);
      }
      else{
        printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
        return 0;
      }
      int composite = treasure[level].size_percentages * treasure[level].size_parameters;
      printf("size_percentages * size_parameters = %d\n", composite);
     treasure[level].table = (int*)malloc(sizeof(int) *
    (treasure[level].size_percentages * treasure[level].size_parameters));
    for(i = 0; i < treasure[level].size_percentages; i++){
      if((fgets(line_buffer, max_buffer_size-1, file_pointer) != NULL)){
        printf("Buffer: ");
        fputs(line_buffer,stdout);
        printf("\n");
        char* token;
        token = strtok(line_buffer, ",");
        for(j = 0; j < treasure[level].size_parameters; j++){
          int value = strtol(token, &token, 10);
          printf("Writing value %d to index %d in table...\n", value, ((i*treasure[level].size_parameters) + j));
          treasure[level].table[(i*treasure[level].size_parameters) + j] = value;
          token = strtok(NULL,",");
        }
      }
      else{
        printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
        return 0;
      }
    }
    printf("Marker for GDB\n");
  }//This marks the end of a treasure struct creation.

  //Verify that each struct received the proper information from the file.
  for(level = 0; level < num_levels; level++){
    printf("VERIFICATION STEP: LEVEL %d\n", level+1);
    printf("TREASURE SIZE_PERCENTAGES: %d\n", treasure[level].size_percentages);
    printf("TREASURE_PERCENTAGES: ");
    for(i = 0; i < treasure[level].size_percentages; i++){
      printf("%d ", treasure[level].percentages[i]);
    }
    printf("\n TREASURE size_parameters: %d\n", treasure[level].size_parameters);
    printf("TREASURE_PARAMETERS: \n");
    for(i = 0; i < treasure[level].size_percentages; i++){
      printf("%d", treasure[level].table[(i*treasure[level].size_parameters)+0]);
      for(j = 1; j < treasure[level].size_parameters; j++){
        printf(", %4d", treasure[level].table[(i*treasure[level].size_parameters)+j]);
      }
      printf("\n");
    }
    printf("VERIFICATION FOR LEVEL %d COMPLETE.\n",level+1);
  }
  return 0;
}


int populate_struct(int num_levels, TreasureClassification t_classification, FILE *file_pointer){
  switch(t_classification){
    case NO_CLASS:
    case COINS:
    case GOODS:
    case ITEMS:
    default:
  }
}

int main(void){
  FILE *file_pointer;
  file_pointer = fopen("example_text.txt","r");
  int num_levels = 7;
  Treasure *coin_treasure_by_level = (Treasure *)malloc(sizeof(Treasure) * num_levels);
  GoodsTreasure *goods_treasure_by_level = (GoodsTreasure *)malloc(sizeof(GoodsTreasure) * num_levels);

  read_file_to_struct(7, COINS, file_pointer);
  fclose(file_pointer);
  return 0;
}
