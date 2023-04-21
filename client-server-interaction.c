/*
 * Client-Server illustration with low-level networking functions
 *
 *
 *                           Request
 *         send()   ----------------------->   recv()
 *           ^                                   |
 *           |                                   v
 *      +==========+                        +==========+
 *      |  Client  |                        |  Server  |
 *      +==========+                        +==========+
 *           ^                                   |
 *           |                                   v
 *         recv()   <-----------------------   send()
 *                          Response
 */
