/*
 ============================================================================
 Name        : ip_addresses.c
 Author      : Augustin ROLET
 Version     : 1.0
 Copyright   : Copyright Free
 Description : Network IP addresses tool
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>


struct IPv4 {
    /* Class to interact with ip IPv4 addresses */

    char ip_string[18];
    int *ip;
    int prefix;
    int hostpart;
};

char* int_to_bin(int number) {
	// Converts an integer into a binary string
    static char binary[8]; // 8 bits (1 byte) for binary representation

    for (int i = 7; i >= 0; i--) {
    	// we take binary of number AND 1 to check if they match
    	/*
    	  11000000
    	& 00000001
    	-----------
    	  00000000
    	*/
        binary[i] = (number & 1) ? '1' : '0';
        number >>= 1; // shift of 1 bit to the right
        // 7 >> 1 = 3 111 => 11
    }

    return binary;
};

int* ipStringToArray(const char* ipString) {
    int* ipArray = (int*)malloc(4 * sizeof(int));

    // Split the IP string at each period ('.')
    char *token = strtok((char*)ipString, ".");

    for (int i = 0; i < 4; i++) {
        ipArray[i] = atoi(token);
        token = strtok(NULL, ".");
    }

    return ipArray;
};

char* ipArrayToString(const int* ipArray) {
    /* Returns the IP as a string well formatted */
    char ipString[18];

    snprintf(ipString, sizeof(ipString), "%d.%d.%d.%d", ipArray[0], ipArray[1], ipArray[2], ipArray[3]);

    return strdup(ipString); // Make a copy of the string to return
};

void initialize_IPv4(struct IPv4 *ipv4, const char *ipString) {
    /* Initialize the attributes of the IPv4 address */
    char tempIp[18]; // Temporary buffer to hold the IP address
    strcpy(tempIp, ipString); // Copy the IP string to tempIp

    // Course Point:
    // strtok() returns a pointer to the first character,
    // inserts a NULL('\0') character in the string,
    // and remembers where it stopped.
    char *token = strtok(tempIp, "/"); // Split the char in half
    strcpy(ipv4->ip_string, token); // Copy the IP part to the struct

    token = strtok(NULL, "/");
    int pref = atoi(token); // Convert to an integer
    ipv4->prefix = pref; // Store the prefix part
    ipv4->hostpart = 32 - pref;

    // Convert the str ip into an array
    ipv4->ip = ipStringToArray(ipv4->ip_string);
};

double get_number_ip_available(struct IPv4 *ipv4) {
    /* Returns the number of available ips addresses */
    if (ipv4->prefix == 32) {
        return 1;
    }
    else if (ipv4->prefix == 31) {
        return 0;
    }
    else {
        return pow(2, ipv4->hostpart) - 2;
    }
};

char* get_broadcast(struct IPv4 *ipv4) {
    /* Returns the broadcast ip address */

    if (ipv4->prefix == 32) {
        return ipArrayToString(ipv4->ip);
    }
    int n = ipv4->prefix / 8;
    int broadcast[4] = {};
    
    // Fixed part of the ip
    for (int i=0; i<n; i++) {
        broadcast[i] = ipv4->ip[i];
    }
    // Byte that change
    int network_bits;
    if (ipv4->hostpart % 8 != 0) {
        network_bits = 8 - (ipv4->hostpart % 8);
    }
    else {
        network_bits = 0;
    }

    // the (n)th byte in binary
    char binary_number[8];
    strcpy(binary_number, int_to_bin(ipv4->ip[n])); // Copy the binary representation into binary_number address

    for (int i = network_bits; i <= 7; i++) {
        binary_number[i] = '1';
    }
    broadcast[n] = strtol(binary_number, NULL, 2);

    for (int i = n+1; i <= 4; i++) {
    	broadcast[i] = 255;
    }
    // inside the int_to_bin broadcast[0] pointer is broken
    broadcast[0] = ipv4->ip[0];

    return ipArrayToString(broadcast);
};

char* get_network(struct IPv4 *ipv4) {
    /* Returns the network ip address */

    if (ipv4->prefix == 32) {
        return ipArrayToString(ipv4->ip);
    }
    int n = ipv4->prefix / 8;
    int network[4] = {};

    // Fixed part of the ip
    for (int i=0; i<n; i++) {
    	network[i] = ipv4->ip[i];
    }
    // Byte that change
    int network_bits;
    if (ipv4->hostpart % 8 != 0) {
        network_bits = 8 - (ipv4->hostpart % 8);
    }
    else {
        network_bits = 0;
    }

    // the (n)th byte in binary
    char binary_number[8];
    strcpy(binary_number, int_to_bin(ipv4->ip[n])); // Copy the binary representation into binary_number address

    for (int i = network_bits; i <= 7; i++) {
        binary_number[i] = '0';
    }
    network[n] = strtol(binary_number, NULL, 2);

    for (int i = n+1; i <= 4; i++) {
    	network[i] = 0;
    }
    // inside the int_to_bin broadcast[0] pointer is broken
    network[0] = ipv4->ip[0];

    return ipArrayToString(network);
};

char* get_subnet_mask(struct IPv4 *ipv4) {
    /* Returns the subnet mask */

    if (ipv4->prefix == 32) {
        return "255.255.255.255";
    }

    int n = ipv4->prefix / 8;
    int subnet_mask[4] = {};;
    for (int i = 0; i < n; i++) {
        subnet_mask[i] = 255;
    }

    int remaining_bits = ipv4->prefix - n*8;
    // the (n)th byte in binary
    char binary_number[8] = "00000000";
    for (int i = 0; i < remaining_bits; i++) {
        binary_number[i] = '1';
    }

    subnet_mask[n] = strtol(binary_number, NULL, 2);
    for (int i = n+2; i <= 4; i++) {
        subnet_mask[i] = 0;
    }
    
    return ipArrayToString(subnet_mask);
};

bool is_valid(struct IPv4 *ipv4) {
	/* Returns True if the ip is not the broadcast and not the network ip address else False */

	char* ip = ipArrayToString(ipv4->ip);
	if (ip != get_network(ipv4) && ip != get_broadcast(ipv4)) {
		return true;
	}
	return false;
};

int main(){
    struct IPv4 ip_address;
    const char *ip_address_input = "192.168.1.1/24";
    //printf("Enter your ip adress and its prefix: ")
    //scanf_s("%s", &ip_address_input)

    initialize_IPv4(&ip_address, ip_address_input);
    if (is_valid(&ip_address) == 1) {
        printf("\nThe ip is valid");
    }
    else {
        printf("\nThe ip is not valid");
    }
    printf("\nIP is : %s", ipArrayToString(ip_address.ip));
    printf("\nPrefix is : %i", ip_address.prefix);
    printf("\nBroadcast address is : %s", get_broadcast(&ip_address));
    printf("\nNetwork address is : %s", get_network(&ip_address));
    printf("\nSubnet mask is : %s", get_subnet_mask(&ip_address));
    printf("\nNumber of ip addresses available : %.0lf", get_number_ip_available(&ip_address));
};