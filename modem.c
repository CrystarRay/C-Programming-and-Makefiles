// modem.c
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define num_networks 10

typedef enum {kData, kWifi, kUnknown} MEDIUM;

struct network {
 char network_name[10]; 
 int signal_strength;
 MEDIUM connection_medium;
 bool password_saved;
} network;

struct network cached_networks[num_networks];

void ChooseNetwork(char* network) {
  printf("Network chosen: %s", network);
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

/**
  * TODO: This function is buggy and not complete
  *
  * We should first fix this function and then work on fixing ScanNetworksV2().
  * The fixes found in this function will help determine how to fix V2.
  */
void ScanNetworks() {
  // Initialize needed vars
  char temp_name[10];
  int medium;

  // Initialize File ptr and open 'experiment_data'
  FILE *ptr = fopen("experiment_data","r"); 

  // For each network
  for (int i = 0; i < num_networks; i++) {
    // Read temp_name
    fscanf(ptr,"%s", temp_name);

    // Read medium, signal_strength, password_saved
    fscanf(ptr,"%d %d %d", &medium,
                        &cached_networks[i].signal_strength,
                        &cached_networks[i].password_saved);
    
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
  char network_name[10];
  int signal_strength;
  int medium;
  bool password_saved;
  int i = 0;

  FILE *ptr = fopen("experiment_data","r");

  while (i < num_networks) {
    fscanf(ptr,"%s",network_name);
    fscanf(ptr,"%d %d %d", &medium, &signal_strength,
                         &password_saved);

    // Only cache networks we can't even connect to
    if (password_saved) {
      strcpy(cached_networks[i].network_name,network_name);
      cached_networks[i].connection_medium = ConvertIntToMedium(medium);
      cached_networks[i].signal_strength = signal_strength;
      cached_networks[i].password_saved = password_saved;
    }
    i++;
  }
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
  // Iterate through cached_networks to choose the best network
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
