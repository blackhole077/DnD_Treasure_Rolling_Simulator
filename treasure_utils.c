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
  Treasure *base_amount_treasure;
  Treasure *gems_treasure;
  Treasure *art_treasure;
}GoodsTreasure;

void initialize_treasure(Treasure *treasure, TreasureClassification t_classification, int size_percentages, int size_parameters, int* percentages, int* table){
  treasure->size_percentages = size_percentages;
  treasure->size_parameters = size_parameters;
  treasure->percentages = percentages;
  treasure->table = table;
}

void initialize_goods_treasure(GoodsTreasure *treasure, Treasure *base_treasure, Treasure *gems_treasure, Treasure *art_treasure){
  treasure->base_amount_treasure = base_treasure;
  treasure->gems_treasure = gems_treasure;
  treasure->art_treasure = art_treasure;
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
  initialize_goods_treasure(treasure, base_treasure, gems_treasure, art_treasure);
}

int write_file_to_table(int* size_pointer_row, int* size_pointer_column, int** array_pointer, FILE *file_pointer){
  int i,j,value, num_rows;
  int max_buffer_size = 256;
  char *line_buffer = (char *)malloc(sizeof(char) * max_buffer_size);
  if(line_buffer == NULL){
    printf("Error allocating memory. Exiting program...\n");
    return 0;
  }
  line_buffer[strcspn(line_buffer, "\r\n")] = 0;
  if(size_pointer_row == NULL){
    num_rows = 1;
  }
  else{
    num_rows = *size_pointer_row;
  }
  if((fgets(line_buffer, sizeof(line_buffer), file_pointer) != NULL)){
    // fputs(line_buffer, stdout);
    // fputs("\n|*\n",stdout);
    // treasure[level].size_percentages = (int)strtol(line_buffer, &line_buffer, 10);
    *size_pointer_column = (int) strtol(line_buffer, &line_buffer, 10);
    // printf("Value in struct: %d\n", treasure[level].size_percentages);
  }
  else{
    printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
    return 0;
  }
  // printf("Fetching Percentages...\n");
  //Fetch the percentages themselves and store them in the struct
  (*array_pointer) = (int*) malloc(sizeof(int) * (num_rows * (*size_pointer_column)));
  // printf("Allocated %lf integers worth of memory.\n", (sizeof(array_pointer)/(double)(num_rows * (*size_pointer_column))));
  for(i = 0; i < num_rows; i++){
    if((fgets(line_buffer, max_buffer_size-1, file_pointer) != NULL)){
      // printf("Buffer: ");
      // fputs(line_buffer,stdout);
      // printf("\n");
      char* token;
      token = strtok(line_buffer, ",");
      for(j = 0; j < *size_pointer_column; j++){
        value = strtol(token, &token, 10);
        // printf("Writing value %d to index %d in table...\n", value, ((i*(*size_pointer_column)) + j));
        (*array_pointer)[(i*(*size_pointer_column)) + j] = value;
        token = strtok(NULL,",");
      }
    }
    else{
      printf("line_buffer ran into a NULL POINTER EXCEPTION.\n");
      return 0;
    }
  }
  return 0;
}

int verify_treasure_struct(Treasure treasure){
  int i,j;
  printf("TREASURE SIZE_PERCENTAGES: %d\n", treasure.size_percentages);
  printf("TREASURE_PERCENTAGES: ");
  for(i = 0; i < treasure.size_percentages; i++){
    if(&treasure.percentages[i] == NULL){
      printf("TREASURE_PERCENTAGES AT %d IS NULL. ABORTING SEQUENCE.\n",i);
      return 0;
    }
    printf("%d ", treasure.percentages[i]);
  }
  printf("\n TREASURE size_parameters: %d\n", treasure.size_parameters);
  printf("TREASURE_PARAMETERS: \n");
  for(i = 0; i < treasure.size_percentages; i++){
    if(&treasure.table[(i*treasure.size_parameters)+0] == NULL){
      printf("MEMORY_ADDRESS AT INDEX %d OF TREASURE.TABLE IS NULL. ABORTING SEQUENCE.\n",((i*treasure.size_parameters)+0));
      return 0;
    }
    printf("%d", treasure.table[(i*treasure.size_parameters)+0]);
    for(j = 1; j < treasure.size_parameters; j++){
      if(&treasure.table[(i*treasure.size_parameters)+j] == NULL){
        printf("MEMORY_ADDRESS AT INDEX %d OF TREASURE.TABLE IS NULL. ABORTING SEQUENCE.\n",((i*treasure.size_parameters)+j));
        return 0;
      }
      printf(", %4d", treasure.table[(i*treasure.size_parameters)+j]);
    }
    printf("\n");
  }
  return 0;
}

/**
* read_file_to_struct is a function that converts a file into an array of
* Treasure structures of an appropriate classification (designated by t_classification)
* with entries ranging from [0, num_levels-1] (equivalent to levels 1 through num_levels).
* These structures are accessed by later functions to calculate the actual treasure generated
* via simulated dice rolls.
**/
int read_file_to_struct(int num_levels, TreasureClassification t_classification, void** struct_array_pointer, FILE *file_pointer){
  if(file_pointer == NULL){
    printf("File does not exist. Exiting program.\n");
    return 0;
  }
  if(num_levels < 0){
    return 0;
  }
  if(t_classification < 0 || t_classification > 2){
    return 0;
  }
  int level;
  if(t_classification == COINS){
    *struct_array_pointer = malloc(sizeof(Treasure) * num_levels);
    /**For each level in the coins column of the treasure table, populate a Treasure struct with the appropriate values taken from the file.*/
    for(level = 0; level < num_levels; level++){
      initialize_empty_base_treasure(&((Treasure*)*struct_array_pointer)[level], t_classification);
      write_file_to_table(NULL, &((Treasure*)*struct_array_pointer)[level].size_percentages, &((Treasure*)*struct_array_pointer)[level].percentages, file_pointer);
      write_file_to_table(&((Treasure*)*struct_array_pointer)[level].size_percentages, &((Treasure*)*struct_array_pointer)[level].size_parameters, &((Treasure*)*struct_array_pointer)[level].table, file_pointer);
    }/**This marks the end of a treasure struct creation.*/
    /**Verify that the structs were populated properly*/
    for(level = 0; level < num_levels; level++){
      printf("VERIFICATION STEP: LEVEL %d\n", level+1);
      verify_treasure_struct(((Treasure*)*struct_array_pointer)[level]);
      printf("VERIFICATION FOR LEVEL %d COMPLETE.\n",level+1);
    }
  }
  //TODO: THIS PORTION OF CODE IS NOT TESTED IN ANY CAPACITY. BEWARE.
  else if(t_classification == GOODS){
    *struct_array_pointer = malloc(sizeof(GoodsTreasure) * num_levels);
    for(level = 0; level < num_levels; level++){
      initialize_empty_goods_treasure(&((GoodsTreasure*)*struct_array_pointer)[level], t_classification);
      write_file_to_table(NULL, &((GoodsTreasure*)*struct_array_pointer)[level].base_amount_treasure->size_percentages, &((GoodsTreasure*)*struct_array_pointer)[level].base_amount_treasure->percentages, file_pointer);
      write_file_to_table(&((GoodsTreasure*)*struct_array_pointer)[level].base_amount_treasure->size_percentages, &((GoodsTreasure*)*struct_array_pointer)[level].base_amount_treasure->size_parameters, &((GoodsTreasure*)*struct_array_pointer)[level].base_amount_treasure->table, file_pointer);
      // printf("Marker for GDB\n");
    }//This marks the end of a treasure struct creation.
    for(level = 0; level < num_levels; level++){
      printf("VERIFICATION STEP: LEVEL %d\n", level+1);
      verify_treasure_struct(((Treasure*)*struct_array_pointer)[level]);
      printf("VERIFICATION FOR LEVEL %d COMPLETE.\n",level+1);
    }

  }
  return 0;
}

int main(void){
  FILE *file_pointer;
  file_pointer = fopen("example_text.txt","r");
  int num_levels = 7;
  void* coins_by_level_array;
  read_file_to_struct(num_levels, COINS, &coins_by_level_array, file_pointer);
  /**
  int level;
  for(level = 0; level < num_levels; level++){
    printf("VERIFICATION STEP: LEVEL %d\n", level+1);
    verify_treasure_struct(((Treasure*)coins_by_level_array)[level]);
    printf("VERIFICATION FOR LEVEL %d COMPLETE.\n",level+1);
  }
  */
  fclose(file_pointer);
  return 0;
}
