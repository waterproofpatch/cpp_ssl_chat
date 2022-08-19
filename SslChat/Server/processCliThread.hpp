#ifndef PROCESSCLITHREAD_HPP
#define PROCESSCLITHREAD_HPP

/**
 * @brief handle the CLI
 *
 * @param sock the master socket. Closed when 'quit' is entered at the CLI.
 */
void processCliThread(int serverSocket);

#endif