// modem.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define num_networks 10
#define MAX_LENGTH 20

typedef enum {kData, kWifi, kUnknown} MEDIUM;

struct network {
 char network_name[MAX_LENGTH]; 
 int signal_strength;
 MEDIUM connection_medium;
 bool password_saved;
} network;

struct network cached_networks[num_networks];

void ChooseNetwork(char* network_name) {
  printf("Network chosen: %s", network_name);
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

/**
  * TODO: This function is buggy and not complete
  *
  * We should first fix this function and then work on fixing ScanNetworksV2().
  * The fixes found in this function will help determine how to fix V2.
  */
void ScanNetworks() {
  // Initialize needed vars
  char temp_name[MAX_LENGTH];
  int medium;
  char password_saved_char[6];

  // Initialize File ptr and open 'experiment_data'
  FILE *ptr = fopen("experiment_data","r"); 

  if (ptr == NULL){
    printf("Error opening the file.\n");
    return;
  }
  // For each network
  for (int i = 0; i < num_networks; i++) {
    // Read temp_name
    fscanf(ptr,"%s", temp_name);

    // Read medium, signal_strength, password_saved
    fscanf(ptr,"%d %d %s", &medium,
                        &cached_networks[i].signal_strength,
                        &password_saved_char);
    
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

/**
  * This function early-exits from networks that we don't already have access
  * to. This way we can still scan for 5 networks, but we won't display/attempt
  * to make a decision vs networks we don't have the password for.
  *
  * TODO: This function is buggy and not complete
  */
void ScanNetworksV2() {
  // Define necessary vars
  char network_name[MAX_LENGTH];
  int signal_strength;
  int medium;
  char password_saved_char[5];
  int i = 0;

  FILE *ptr = fopen("experiment_data","r");

  while (i < num_networks) {
    fscanf(ptr,"%s",network_name);
    fscanf(ptr,"%d %d %s", &medium, &signal_strength,
                         password_saved_char);

    
    cached_networks[i].password_saved = ConvertCharToBoolean(password_saved_char);
    // Only cache networks we can't even connect to
    if (cached_networks[i].password_saved) {
      strcpy(cached_networks[i].network_name,network_name);
      cached_networks[i].connection_medium = ConvertIntToMedium(medium);
      cached_networks[i].signal_strength = signal_strength;
    }
    i++;
  }
  fclose(ptr);
}

void ScanNetworksV3() {
  // If you were to iterate and modify ScanNetworksV2 to be even better,
  // what would you do? You DO NOT need to write any code, you can just
  // explain what you would do or write psuedocode.
}

/**
  * This function selects what network we should connect to based on certain
  * criteria.
  *
  * 1. We should only choose networks that we can connect to
  * 2. We should only connect to networks based on what criteria we want
  *    (i.e., Wi-Fi, Data, either).
  * 3. We should pick the network with the highest signal strength
  *
  * criteria    String denoting "wifi", "data", or "greedy"
  * return      String of best network_name
  */
char* DetermineNetwork(char* criteria) {
    MEDIUM desired_medium;

    // Determine the desired connection medium based on criteria
    if (strcmp(criteria, "wifi") == 0) {
        desired_medium = kWifi;
    } else if (strcmp(criteria, "data") == 0) {
        desired_medium = kData;
    } else if (strcmp(criteria, "greedy") == 0) {
        desired_medium = kUnknown; // Set to unknown for greedy so that all mediums are considered
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
  ScanNetworks();
  printf("Networks cached. Now determining network to connect...\n");
  printf("Connection Medium Criteria: %s", argv[1]);
  ChooseNetwork(DetermineNetwork(argv[1]));
  return 0;
}
