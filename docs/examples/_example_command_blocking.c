char hostname[20];

/* Somewhere in thread function */

/* Get device hostname in blocking mode */
/* Function returns actual result */
if (esp_hostname_get(hostname, sizeof(hostname), NULL, NULL, 1 /* 1 means blocking call */) == espOK) {
	/* At this point we have valid result and parameters from API function */
	printf("ESP hostname is...");
} else {
	printf("Error reading ESP hostname...");
}