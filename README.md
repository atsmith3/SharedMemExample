# Shared Memory Producer/Consumer
Simple producer, consumer shared memory example. Makes use of a shared memory region to pass data between two processes.

### Build
To build the example:
`make`

### Run
To run the example, in one teminal first run `./producer` this will initialize the shared memory region and wait for the consumer to begin reading. Next in a new window, execute `./consumer` this will read from the shared memory region and let the producer know it is ready for new data.
