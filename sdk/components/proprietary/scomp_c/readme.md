# SCOMP (Simple Communication Protocol)

Protocol for device communication. Slave side only.

## Getting started

1. Include sources to your project
2. Define command by reference commands/template_command files
3. Initialize command before communication run
4. Initialize the protocol
5. Register handler for responses data sending via scomp_register_ready_data_cb function
6. Call scomp_parse when data from master received

## Requirements

For this library building, please use SEGGER or GCC compiler

## Use cases

See the Orgenesis projects

## License

TODO: Choose license type