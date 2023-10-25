// modem.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define num_networks 18
#define MAX_LENGTH 20



typedef enum {kData, kWifi, kUnknown} MEDIUM;

typedef struct network {
 char network_name[MAX_LENGTH];
 int signal_strength;
 MEDIUM connection_medium;
 bool password_saved;
} network;

typedef struct best_networks{
  int best_data;
  int best_wifi;
  char best_data_name[MAX_LENGTH];
  char best_wifi_name[MAX_LENGTH];
} best_networks;

struct network cached_networks[num_networks];

struct best_networks best_network;

void ChooseNetwork(char* network) {
  printf("Network chosen: %s\n", network);
}

MEDIUM ConvertIntToMedium(int int_medium) {
  if (int_medium == 1) {
    return kData;
  } else if (int_medium == 2) {
    return kWifi;
  } else {
    return kUnknown;
  }
}

bool ConvertCharToBoolean(char* str) {
    if (strcmp(str, "false") == 0) {
        return false;
    } else{
        return true;
    }
}

void ScanNetworks() {
  // Initialize needed vars
  char temp_name[MAX_LENGTH];
  int medium;
  char password_saved_char[6];

  // Initialize File ptr and open 'experiment_data'
  FILE *ptr = fopen("experiment_data","r");

  // For each network
  for (int i = 0; i < num_networks; i++) {
    // Read temp_name
    fscanf(ptr,"%s", temp_name);

    // Read medium, signal_strength, password_saved
    fscanf(ptr,"%d %d %s", &medium,
                        &cached_networks[i].signal_strength,
                        password_saved_char);

    // Populate password_saved as bool
    cached_networks[i].password_saved = ConvertCharToBoolean(password_saved_char);
    // Copy temp_name to network_name
    strcpy(cached_networks[i].network_name, temp_name);

    // Print network_name
    printf("%s\n", cached_networks[i].network_name);

    // Call Helper function
    cached_networks[i].connection_medium = ConvertIntToMedium(medium);
  }
  // close file ptr
  fclose(ptr);
}


void ScanNetworksV2() {
  char network_name[MAX_LENGTH];
  int signal_strength;
  int medium;
  char password_saved_str[6];
  int i = 0;
  int j = 0;

  FILE *ptr = fopen("experiment_data", "r");
  if (ptr == NULL) {
    perror("Error opening the file");
    return;
  }

  while (i < num_networks && !feof(ptr)) {
    int nread = fscanf(ptr, "%s %d %d %s", network_name, &medium, &signal_strength, password_saved_str);
    if (nread != 4) {
      printf("File read error at line %d. Read %d elements.\n", i+1, nread);
      fclose(ptr);
      return;
    }

    bool password_saved = (strcmp(password_saved_str, "true") == 0);

    if (password_saved) {
      if (j >= num_networks) {
        printf("Array out of bounds. Exiting.\n");
        fclose(ptr);
        return;
      }
      strncpy(cached_networks[j].network_name, network_name, MAX_LENGTH-1);
      cached_networks[j].network_name[MAX_LENGTH-1] = '\0';
      cached_networks[j].connection_medium = ConvertIntToMedium(medium);
      cached_networks[j].signal_strength = signal_strength;
      cached_networks[j].password_saved = password_saved;
      j++;
    }
    i++;
  }
  fclose(ptr);
}

void ScanNetworksV3() {
  FILE *ptr = fopen("experiment_data", "r");
  if (ptr == NULL) {
    printf("File read error. Make sure 'experiment_data' file exists.\n");
    exit(1);
  }

  char network_name[MAX_LENGTH];
  int medium, signal_strength;
  char password_saved_str[6]; 
  int i = 0;
  best_network.best_data=-1;
  best_network.best_wifi=-1;

  while (i < num_networks && fscanf(ptr, "%s %d %d %s", network_name, &medium, &signal_strength, password_saved_str) == 4) {
    bool password_saved = (strcmp(password_saved_str, "true") == 0); // Convert to bool
    if (password_saved) {
      strcpy(cached_networks[i].network_name, network_name);
      cached_networks[i].connection_medium = ConvertIntToMedium(medium);
      cached_networks[i].signal_strength = signal_strength;
      cached_networks[i].password_saved = password_saved;
      if (medium == 1 && signal_strength > best_network.best_data){
        best_network.best_data = signal_strength;
        strcpy(best_network.best_data_name,cached_networks[i].network_name);
      }
      if (medium == 2 && signal_strength > best_network.best_wifi){
        best_network.best_wifi = signal_strength;
        strcpy(best_network.best_wifi_name,cached_networks[i].network_name);
      }
    }
    i++;
  }
  fclose(ptr);
  if (i < num_networks) {
    printf("File read error or fewer networks found. Scanned %d networks.\n", i);
    exit(1);
  }
}



char* DetermineNetworkV3(char* criteria){
  if (strcmp(criteria, "wifi") == 0){
    return best_network.best_wifi_name;
  }
  else if (strcmp(criteria, "data") == 0) {
    return best_network.best_data_name;
  }
  else if(strcmp(criteria, "greedy") == 0){
    if(best_network.best_data > best_network.best_wifi){
      return best_network.best_wifi_name;
    }
    else{
      return best_network.best_wifi_name;
    }
  }
  printf("Unknown criteria provided. Exiting.\n");
  exit(1);
}

char* DetermineNetwork(char* criteria) {
    MEDIUM desired_medium;

    // Determine the desired connection medium based on criteria
    if (strcmp(criteria, "wifi") == 0) {
        desired_medium = kWifi;
    } else if (strcmp(criteria, "data") == 0) {
        desired_medium = kData;
    } else if (strcmp(criteria, "greedy") == 0) {
        desired_medium = kUnknown; 
    } else {
        printf("Unknown criteria provided. Exiting.\n");
        exit(1);
    }

    char* best_network = NULL;
    int max_strength = -1; // Initialize with an impossible value to ensure any real value will be higher

    for (int i = 0; i < num_networks; i++) {
        // Check if network has saved password
        if (!cached_networks[i].password_saved) {
            continue;
        }

        // If we're not being greedy, check if the network's medium matches the desired medium
        if (desired_medium != kUnknown && cached_networks[i].connection_medium != desired_medium) {
            continue;
        }

        // Update the best network if this network's signal strength is greater than the current max
        if (cached_networks[i].signal_strength > max_strength) {
            max_strength = cached_networks[i].signal_strength;
            best_network = cached_networks[i].network_name;
            if (max_strength == 5){
              return best_network;
            }
        }
    }

    // If no suitable network was found, return NULL
    if (!best_network) {
        printf("No suitable network found based on the provided criteria.\n");
        exit(1);
    }

    return best_network;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Incorrect command argument. Please pass in wifi, data, or greedy");
    return 1;
  }

  printf("Starting up modem...\n");
  printf("Scanning nearby network connections...\n");
  ScanNetworksV3();
  printf("Networks cached. Now determining network to connect...\n");
  printf("Connection Medium Criteria: %s\n", argv[1]);
  ChooseNetwork(DetermineNetworkV3(argv[1]));

  return 0;
}
