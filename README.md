# Socket-programming

Implementation of a FTP server/client architecture that can accept file transfer requests from multiple clients with additional inputs. The transfer may happen in binary or character mode depending on the choice of the particular client. The file transfer is bidirectional in nature. It means, a client can get a file from the server or the client can upload a file into the server. However, since there are multiple clients involved in the system, you will have to come-up with a naïve user authentication system that will ensure that the files sent/received by/from the client are placed in the right directory of the server and no overwriting happens with the files of the other clients. Try to improvise to include the authentication mechanism. You need not use the authentication mechanism already available in the Linux operating system. The client side is expected to be a command driven interface depending on the type of the operation being handled. See the typical ftp client behavior in a Linux OS for reference. You will find the client side supports commands such as GET, PUT, MODE, etc. Try to make your implementation more individualistic than mimicking the functionalities provided in the Linux FTP implementations. Try to print as much statistics as possible for an ongoing transfer on the client side output terminal. Actually, the server-side outputs are not important except for debugging purpose!


///////////////////////////////////////////////////////////////////////////////////////

Apart the above functionalities there is also an admin user whose basic work is to create users.
Once these users are created each user is allotted a separate folder on the sever side(server folder).

#drawbacks
If the same file is sent mulriple times then the when we run the list function we get multiple files as they are being written in the txt file.
The same goes to users having same Username.
